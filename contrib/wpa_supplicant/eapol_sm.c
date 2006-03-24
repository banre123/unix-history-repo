/*
 * WPA Supplicant / EAPOL state machines
 * Copyright (c) 2004-2005, Jouni Malinen <jkmaline@cc.hut.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "eapol_sm.h"
#include "eap.h"
#include "eloop.h"
#include "l2_packet.h"
#include "wpa.h"
#include "md5.h"
#include "rc4.h"


/* IEEE 802.1X-2004 - Supplicant - EAPOL state machines */

/**
 * struct eapol_sm - Internal data for EAPOL state machines
 */
struct eapol_sm {
	/* Timers */
	unsigned int authWhile;
	unsigned int heldWhile;
	unsigned int startWhen;
	unsigned int idleWhile; /* for EAP state machine */

	/* Global variables */
	Boolean eapFail;
	Boolean eapolEap;
	Boolean eapSuccess;
	Boolean initialize;
	Boolean keyDone;
	Boolean keyRun;
	PortControl portControl;
	Boolean portEnabled;
	PortStatus suppPortStatus;  /* dot1xSuppControlledPortStatus */
	Boolean portValid;
	Boolean suppAbort;
	Boolean suppFail;
	Boolean suppStart;
	Boolean suppSuccess;
	Boolean suppTimeout;

	/* Supplicant PAE state machine */
	enum {
		SUPP_PAE_UNKNOWN = 0,
		SUPP_PAE_DISCONNECTED = 1,
		SUPP_PAE_LOGOFF = 2,
		SUPP_PAE_CONNECTING = 3,
		SUPP_PAE_AUTHENTICATING = 4,
		SUPP_PAE_AUTHENTICATED = 5,
		/* unused(6) */
		SUPP_PAE_HELD = 7,
		SUPP_PAE_RESTART = 8,
		SUPP_PAE_S_FORCE_AUTH = 9,
		SUPP_PAE_S_FORCE_UNAUTH = 10
	} SUPP_PAE_state; /* dot1xSuppPaeState */
	/* Variables */
	Boolean userLogoff;
	Boolean logoffSent;
	unsigned int startCount;
	Boolean eapRestart;
	PortControl sPortMode;
	/* Constants */
	unsigned int heldPeriod; /* dot1xSuppHeldPeriod */
	unsigned int startPeriod; /* dot1xSuppStartPeriod */
	unsigned int maxStart; /* dot1xSuppMaxStart */

	/* Key Receive state machine */
	enum {
		KEY_RX_UNKNOWN = 0,
		KEY_RX_NO_KEY_RECEIVE, KEY_RX_KEY_RECEIVE
	} KEY_RX_state;
	/* Variables */
	Boolean rxKey;

	/* Supplicant Backend state machine */
	enum {
		SUPP_BE_UNKNOWN = 0,
		SUPP_BE_INITIALIZE = 1,
		SUPP_BE_IDLE = 2,
		SUPP_BE_REQUEST = 3,
		SUPP_BE_RECEIVE = 4,
		SUPP_BE_RESPONSE = 5,
		SUPP_BE_FAIL = 6,
		SUPP_BE_TIMEOUT = 7, 
		SUPP_BE_SUCCESS = 8
	} SUPP_BE_state; /* dot1xSuppBackendPaeState */
	/* Variables */
	Boolean eapNoResp;
	Boolean eapReq;
	Boolean eapResp;
	/* Constants */
	unsigned int authPeriod; /* dot1xSuppAuthPeriod */

	/* Statistics */
	unsigned int dot1xSuppEapolFramesRx;
	unsigned int dot1xSuppEapolFramesTx;
	unsigned int dot1xSuppEapolStartFramesTx;
	unsigned int dot1xSuppEapolLogoffFramesTx;
	unsigned int dot1xSuppEapolRespFramesTx;
	unsigned int dot1xSuppEapolReqIdFramesRx;
	unsigned int dot1xSuppEapolReqFramesRx;
	unsigned int dot1xSuppInvalidEapolFramesRx;
	unsigned int dot1xSuppEapLengthErrorFramesRx;
	unsigned int dot1xSuppLastEapolFrameVersion;
	unsigned char dot1xSuppLastEapolFrameSource[6];

	/* Miscellaneous variables (not defined in IEEE 802.1X-2004) */
	Boolean changed;
	struct eap_sm *eap;
	struct wpa_ssid *config;
	Boolean initial_req;
	u8 *last_rx_key;
	size_t last_rx_key_len;
	u8 *eapReqData; /* for EAP */
	size_t eapReqDataLen; /* for EAP */
	Boolean altAccept; /* for EAP */
	Boolean altReject; /* for EAP */
	Boolean replay_counter_valid;
	u8 last_replay_counter[16];
	struct eapol_config conf;
	struct eapol_ctx *ctx;
	enum { EAPOL_CB_IN_PROGRESS = 0, EAPOL_CB_SUCCESS, EAPOL_CB_FAILURE }
		cb_status;
	Boolean cached_pmk;

	Boolean unicast_key_received, broadcast_key_received;
};


#define IEEE8021X_REPLAY_COUNTER_LEN 8
#define IEEE8021X_KEY_SIGN_LEN 16
#define IEEE8021X_KEY_IV_LEN 16

#define IEEE8021X_KEY_INDEX_FLAG 0x80
#define IEEE8021X_KEY_INDEX_MASK 0x03

struct ieee802_1x_eapol_key {
	u8 type;
	/* Note: key_length is unaligned */
	u8 key_length[2];
	/* does not repeat within the life of the keying material used to
	 * encrypt the Key field; 64-bit NTP timestamp MAY be used here */
	u8 replay_counter[IEEE8021X_REPLAY_COUNTER_LEN];
	u8 key_iv[IEEE8021X_KEY_IV_LEN]; /* cryptographically random number */
	u8 key_index; /* key flag in the most significant bit:
		       * 0 = broadcast (default key),
		       * 1 = unicast (key mapping key); key index is in the
		       * 7 least significant bits */
	/* HMAC-MD5 message integrity check computed with MS-MPPE-Send-Key as
	 * the key */
	u8 key_signature[IEEE8021X_KEY_SIGN_LEN];

	/* followed by key: if packet body length = 44 + key length, then the
	 * key field (of key_length bytes) contains the key in encrypted form;
	 * if packet body length = 44, key field is absent and key_length
	 * represents the number of least significant octets from
	 * MS-MPPE-Send-Key attribute to be used as the keying material;
	 * RC4 key used in encryption = Key-IV + MS-MPPE-Recv-Key */
} __attribute__ ((packed));


static void eapol_sm_txLogoff(struct eapol_sm *sm);
static void eapol_sm_txStart(struct eapol_sm *sm);
static void eapol_sm_processKey(struct eapol_sm *sm);
static void eapol_sm_getSuppRsp(struct eapol_sm *sm);
static void eapol_sm_txSuppRsp(struct eapol_sm *sm);
static void eapol_sm_abortSupp(struct eapol_sm *sm);
static void eapol_sm_abort_cached(struct eapol_sm *sm);
static void eapol_sm_step_timeout(void *eloop_ctx, void *timeout_ctx);


/* Definitions for clarifying state machine implementation */
#define SM_STATE(machine, state) \
static void sm_ ## machine ## _ ## state ## _Enter(struct eapol_sm *sm, \
	int global)

#define SM_ENTRY(machine, state) \
if (!global || sm->machine ## _state != machine ## _ ## state) { \
	sm->changed = TRUE; \
	wpa_printf(MSG_DEBUG, "EAPOL: " #machine " entering state " #state); \
} \
sm->machine ## _state = machine ## _ ## state;

#define SM_ENTER(machine, state) \
sm_ ## machine ## _ ## state ## _Enter(sm, 0)
#define SM_ENTER_GLOBAL(machine, state) \
sm_ ## machine ## _ ## state ## _Enter(sm, 1)

#define SM_STEP(machine) \
static void sm_ ## machine ## _Step(struct eapol_sm *sm)

#define SM_STEP_RUN(machine) sm_ ## machine ## _Step(sm)


/* Port Timers state machine - implemented as a function that will be called
 * once a second as a registered event loop timeout */
static void eapol_port_timers_tick(void *eloop_ctx, void *timeout_ctx)
{
	struct eapol_sm *sm = timeout_ctx;

	if (sm->authWhile > 0) {
		sm->authWhile--;
		if (sm->authWhile == 0)
			wpa_printf(MSG_DEBUG, "EAPOL: authWhile --> 0");
	}
	if (sm->heldWhile > 0) {
		sm->heldWhile--;
		if (sm->heldWhile == 0)
			wpa_printf(MSG_DEBUG, "EAPOL: heldWhile --> 0");
	}
	if (sm->startWhen > 0) {
		sm->startWhen--;
		if (sm->startWhen == 0)
			wpa_printf(MSG_DEBUG, "EAPOL: startWhen --> 0");
	}
	if (sm->idleWhile > 0) {
		sm->idleWhile--;
		if (sm->idleWhile == 0)
			wpa_printf(MSG_DEBUG, "EAPOL: idleWhile --> 0");
	}

	eloop_register_timeout(1, 0, eapol_port_timers_tick, eloop_ctx, sm);
	eapol_sm_step(sm);
}


SM_STATE(SUPP_PAE, LOGOFF)
{
	SM_ENTRY(SUPP_PAE, LOGOFF);
	eapol_sm_txLogoff(sm);
	sm->logoffSent = TRUE;
	sm->suppPortStatus = Unauthorized;
}


SM_STATE(SUPP_PAE, DISCONNECTED)
{
	SM_ENTRY(SUPP_PAE, DISCONNECTED);
	sm->sPortMode = Auto;
	sm->startCount = 0;
	sm->logoffSent = FALSE;
	sm->suppPortStatus = Unauthorized;
	sm->suppAbort = TRUE;

	sm->unicast_key_received = FALSE;
	sm->broadcast_key_received = FALSE;
}


SM_STATE(SUPP_PAE, CONNECTING)
{
	int send_start = sm->SUPP_PAE_state == SUPP_PAE_CONNECTING;
	SM_ENTRY(SUPP_PAE, CONNECTING);
	if (send_start) {
		sm->startWhen = sm->startPeriod;
		sm->startCount++;
	} else {
		/*
		 * Do not send EAPOL-Start immediately since in most cases,
		 * Authenticator is going to start authentication immediately
		 * after association and an extra EAPOL-Start is just going to
		 * delay authentication. Use a short timeout to send the first
		 * EAPOL-Start if Authenticator does not start authentication.
		 */
		sm->startWhen = 3;
	}
	sm->eapolEap = FALSE;
	if (send_start)
		eapol_sm_txStart(sm);
}


SM_STATE(SUPP_PAE, AUTHENTICATING)
{
	SM_ENTRY(SUPP_PAE, AUTHENTICATING);
	sm->startCount = 0;
	sm->suppSuccess = FALSE;
	sm->suppFail = FALSE;
	sm->suppTimeout = FALSE;
	sm->keyRun = FALSE;
	sm->keyDone = FALSE;
	sm->suppStart = TRUE;
}


SM_STATE(SUPP_PAE, HELD)
{
	SM_ENTRY(SUPP_PAE, HELD);
	sm->heldWhile = sm->heldPeriod;
	sm->suppPortStatus = Unauthorized;
	sm->cb_status = EAPOL_CB_FAILURE;
}


SM_STATE(SUPP_PAE, AUTHENTICATED)
{
	SM_ENTRY(SUPP_PAE, AUTHENTICATED);
	sm->suppPortStatus = Authorized;
	sm->cb_status = EAPOL_CB_SUCCESS;
}


SM_STATE(SUPP_PAE, RESTART)
{
	SM_ENTRY(SUPP_PAE, RESTART);
	sm->eapRestart = TRUE;
}


SM_STATE(SUPP_PAE, S_FORCE_AUTH)
{
	SM_ENTRY(SUPP_PAE, S_FORCE_AUTH);
	sm->suppPortStatus = Authorized;
	sm->sPortMode = ForceAuthorized;
}


SM_STATE(SUPP_PAE, S_FORCE_UNAUTH)
{
	SM_ENTRY(SUPP_PAE, S_FORCE_UNAUTH);
	sm->suppPortStatus = Unauthorized;
	sm->sPortMode = ForceUnauthorized;
	eapol_sm_txLogoff(sm);
}


SM_STEP(SUPP_PAE)
{
	if ((sm->userLogoff && !sm->logoffSent) &&
	    !(sm->initialize || !sm->portEnabled))
		SM_ENTER_GLOBAL(SUPP_PAE, LOGOFF);
	else if (((sm->portControl == Auto) &&
		  (sm->sPortMode != sm->portControl)) ||
		 sm->initialize || !sm->portEnabled)
		SM_ENTER_GLOBAL(SUPP_PAE, DISCONNECTED);
	else if ((sm->portControl == ForceAuthorized) &&
		 (sm->sPortMode != sm->portControl) &&
		 !(sm->initialize || !sm->portEnabled))
		SM_ENTER_GLOBAL(SUPP_PAE, S_FORCE_AUTH);
	else if ((sm->portControl == ForceUnauthorized) &&
		 (sm->sPortMode != sm->portControl) &&
		 !(sm->initialize || !sm->portEnabled))
		SM_ENTER_GLOBAL(SUPP_PAE, S_FORCE_UNAUTH);
	else switch (sm->SUPP_PAE_state) {
	case SUPP_PAE_UNKNOWN:
		break;
	case SUPP_PAE_LOGOFF:
		if (!sm->userLogoff)
			SM_ENTER(SUPP_PAE, DISCONNECTED);
		break;
	case SUPP_PAE_DISCONNECTED:
		SM_ENTER(SUPP_PAE, CONNECTING);
		break;
	case SUPP_PAE_CONNECTING:
		if (sm->startWhen == 0 && sm->startCount < sm->maxStart)
			SM_ENTER(SUPP_PAE, CONNECTING);
		else if (sm->startWhen == 0 &&
			 sm->startCount >= sm->maxStart &&
			 sm->portValid)
			SM_ENTER(SUPP_PAE, AUTHENTICATED);
		else if (sm->eapSuccess || sm->eapFail)
			SM_ENTER(SUPP_PAE, AUTHENTICATING);
		else if (sm->eapolEap)
			SM_ENTER(SUPP_PAE, RESTART);
		else if (sm->startWhen == 0 &&
			 sm->startCount >= sm->maxStart &&
			 !sm->portValid)
			SM_ENTER(SUPP_PAE, HELD);
		break;
	case SUPP_PAE_AUTHENTICATING:
		if (sm->eapSuccess && !sm->portValid &&
		    sm->conf.accept_802_1x_keys &&
		    sm->conf.required_keys == 0) {
			wpa_printf(MSG_DEBUG, "EAPOL: IEEE 802.1X for "
				   "plaintext connection; no EAPOL-Key frames "
				   "required");
			sm->portValid = TRUE;
			if (sm->ctx->eapol_done_cb)
				sm->ctx->eapol_done_cb(sm->ctx->ctx);
		}
		if (sm->eapSuccess && sm->portValid)
			SM_ENTER(SUPP_PAE, AUTHENTICATED);
		else if (sm->eapFail || (sm->keyDone && !sm->portValid))
			SM_ENTER(SUPP_PAE, HELD);
		else if (sm->suppTimeout)
			SM_ENTER(SUPP_PAE, CONNECTING);
		break;
	case SUPP_PAE_HELD:
		if (sm->heldWhile == 0)
			SM_ENTER(SUPP_PAE, CONNECTING);
		else if (sm->eapolEap)
			SM_ENTER(SUPP_PAE, RESTART);
		break;
	case SUPP_PAE_AUTHENTICATED:
		if (sm->eapolEap && sm->portValid)
			SM_ENTER(SUPP_PAE, RESTART);
		else if (!sm->portValid)
			SM_ENTER(SUPP_PAE, DISCONNECTED);
		break;
	case SUPP_PAE_RESTART:
		if (!sm->eapRestart)
			SM_ENTER(SUPP_PAE, AUTHENTICATING);
		break;
	case SUPP_PAE_S_FORCE_AUTH:
		break;
	case SUPP_PAE_S_FORCE_UNAUTH:
		break;
	}
}


SM_STATE(KEY_RX, NO_KEY_RECEIVE)
{
	SM_ENTRY(KEY_RX, NO_KEY_RECEIVE);
}


SM_STATE(KEY_RX, KEY_RECEIVE)
{
	SM_ENTRY(KEY_RX, KEY_RECEIVE);
	eapol_sm_processKey(sm);
	sm->rxKey = FALSE;
}


SM_STEP(KEY_RX)
{
	if (sm->initialize || !sm->portEnabled)
		SM_ENTER_GLOBAL(KEY_RX, NO_KEY_RECEIVE);
	switch (sm->KEY_RX_state) {
	case KEY_RX_UNKNOWN:
		break;
	case KEY_RX_NO_KEY_RECEIVE:
		if (sm->rxKey)
			SM_ENTER(KEY_RX, KEY_RECEIVE);
		break;
	case KEY_RX_KEY_RECEIVE:
		if (sm->rxKey)
			SM_ENTER(KEY_RX, KEY_RECEIVE);
		break;
	}
}


SM_STATE(SUPP_BE, REQUEST)
{
	SM_ENTRY(SUPP_BE, REQUEST);
	sm->authWhile = 0;
	sm->eapReq = TRUE;
	eapol_sm_getSuppRsp(sm);
}


SM_STATE(SUPP_BE, RESPONSE)
{
	SM_ENTRY(SUPP_BE, RESPONSE);
	eapol_sm_txSuppRsp(sm);
	sm->eapResp = FALSE;
}


SM_STATE(SUPP_BE, SUCCESS)
{
	SM_ENTRY(SUPP_BE, SUCCESS);
	sm->keyRun = TRUE;
	sm->suppSuccess = TRUE;

	if (eap_key_available(sm->eap)) {
		/* New key received - clear IEEE 802.1X EAPOL-Key replay
		 * counter */
		sm->replay_counter_valid = FALSE;
	}
}


SM_STATE(SUPP_BE, FAIL)
{
	SM_ENTRY(SUPP_BE, FAIL);
	sm->suppFail = TRUE;
}


SM_STATE(SUPP_BE, TIMEOUT)
{
	SM_ENTRY(SUPP_BE, TIMEOUT);
	sm->suppTimeout = TRUE;
}


SM_STATE(SUPP_BE, IDLE)
{
	SM_ENTRY(SUPP_BE, IDLE);
	sm->suppStart = FALSE;
	sm->initial_req = TRUE;
}


SM_STATE(SUPP_BE, INITIALIZE)
{
	SM_ENTRY(SUPP_BE, INITIALIZE);
	eapol_sm_abortSupp(sm);
	sm->suppAbort = FALSE;
}


SM_STATE(SUPP_BE, RECEIVE)
{
	SM_ENTRY(SUPP_BE, RECEIVE);
	sm->authWhile = sm->authPeriod;
	sm->eapolEap = FALSE;
	sm->eapNoResp = FALSE;
	sm->initial_req = FALSE;
}


SM_STEP(SUPP_BE)
{
	if (sm->initialize || sm->suppAbort)
		SM_ENTER_GLOBAL(SUPP_BE, INITIALIZE);
	else switch (sm->SUPP_BE_state) {
	case SUPP_BE_UNKNOWN:
		break;
	case SUPP_BE_REQUEST:
		if (sm->eapResp && sm->eapNoResp) {
			wpa_printf(MSG_DEBUG, "EAPOL: SUPP_BE REQUEST: both "
				   "eapResp and eapNoResp set?!");
		}
		if (sm->eapResp)
			SM_ENTER(SUPP_BE, RESPONSE);
		else if (sm->eapNoResp)
			SM_ENTER(SUPP_BE, RECEIVE);
		break;
	case SUPP_BE_RESPONSE:
		SM_ENTER(SUPP_BE, RECEIVE);
		break;
	case SUPP_BE_SUCCESS:
		SM_ENTER(SUPP_BE, IDLE);
		break;
	case SUPP_BE_FAIL:
		SM_ENTER(SUPP_BE, IDLE);
		break;
	case SUPP_BE_TIMEOUT:
		SM_ENTER(SUPP_BE, IDLE);
		break;
	case SUPP_BE_IDLE:
		if (sm->eapFail && sm->suppStart)
			SM_ENTER(SUPP_BE, FAIL);
		else if (sm->eapolEap && sm->suppStart)
			SM_ENTER(SUPP_BE, REQUEST);
		else if (sm->eapSuccess && sm->suppStart)
			SM_ENTER(SUPP_BE, SUCCESS);
		break;
	case SUPP_BE_INITIALIZE:
		SM_ENTER(SUPP_BE, IDLE);
		break;
	case SUPP_BE_RECEIVE:
		if (sm->eapolEap)
			SM_ENTER(SUPP_BE, REQUEST);
		else if (sm->eapFail)
			SM_ENTER(SUPP_BE, FAIL);
		else if (sm->authWhile == 0)
			SM_ENTER(SUPP_BE, TIMEOUT);
		else if (sm->eapSuccess)
			SM_ENTER(SUPP_BE, SUCCESS);
		break;
	}
}


static void eapol_sm_txLogoff(struct eapol_sm *sm)
{
	wpa_printf(MSG_DEBUG, "EAPOL: txLogoff");
	sm->ctx->eapol_send(sm->ctx->eapol_send_ctx,
			    IEEE802_1X_TYPE_EAPOL_LOGOFF, (u8 *) "", 0);
	sm->dot1xSuppEapolLogoffFramesTx++;
	sm->dot1xSuppEapolFramesTx++;
}


static void eapol_sm_txStart(struct eapol_sm *sm)
{
	wpa_printf(MSG_DEBUG, "EAPOL: txStart");
	sm->ctx->eapol_send(sm->ctx->eapol_send_ctx,
			    IEEE802_1X_TYPE_EAPOL_START, (u8 *) "", 0);
	sm->dot1xSuppEapolStartFramesTx++;
	sm->dot1xSuppEapolFramesTx++;
}


#define IEEE8021X_ENCR_KEY_LEN 32
#define IEEE8021X_SIGN_KEY_LEN 32

struct eap_key_data {
	u8 encr_key[IEEE8021X_ENCR_KEY_LEN];
	u8 sign_key[IEEE8021X_SIGN_KEY_LEN];
};


static void eapol_sm_processKey(struct eapol_sm *sm)
{
	struct ieee802_1x_hdr *hdr;
	struct ieee802_1x_eapol_key *key;
	struct eap_key_data keydata;
	u8 orig_key_sign[IEEE8021X_KEY_SIGN_LEN], datakey[32];
	u8 ekey[IEEE8021X_KEY_IV_LEN + IEEE8021X_ENCR_KEY_LEN];
	int key_len, res, sign_key_len, encr_key_len;
	u16 rx_key_length;

	wpa_printf(MSG_DEBUG, "EAPOL: processKey");
	if (sm->last_rx_key == NULL)
		return;

	if (!sm->conf.accept_802_1x_keys) {
		wpa_printf(MSG_WARNING, "EAPOL: Received IEEE 802.1X EAPOL-Key"
			   " even though this was not accepted - "
			   "ignoring this packet");
		return;
	}

	hdr = (struct ieee802_1x_hdr *) sm->last_rx_key;
	key = (struct ieee802_1x_eapol_key *) (hdr + 1);
	if (sizeof(*hdr) + be_to_host16(hdr->length) > sm->last_rx_key_len) {
		wpa_printf(MSG_WARNING, "EAPOL: Too short EAPOL-Key frame");
		return;
	}
	rx_key_length = WPA_GET_BE16(key->key_length);
	wpa_printf(MSG_DEBUG, "EAPOL: RX IEEE 802.1X ver=%d type=%d len=%d "
		   "EAPOL-Key: type=%d key_length=%d key_index=0x%x",
		   hdr->version, hdr->type, be_to_host16(hdr->length),
		   key->type, rx_key_length, key->key_index);

	eapol_sm_notify_lower_layer_success(sm);
	sign_key_len = IEEE8021X_SIGN_KEY_LEN;
	encr_key_len = IEEE8021X_ENCR_KEY_LEN;
	res = eapol_sm_get_key(sm, (u8 *) &keydata, sizeof(keydata));
	if (res < 0) {
		wpa_printf(MSG_DEBUG, "EAPOL: Could not get master key for "
			   "decrypting EAPOL-Key keys");
		return;
	}
	if (res == 16) {
		/* LEAP derives only 16 bytes of keying material. */
		res = eapol_sm_get_key(sm, (u8 *) &keydata, 16);
		if (res) {
			wpa_printf(MSG_DEBUG, "EAPOL: Could not get LEAP "
				   "master key for decrypting EAPOL-Key keys");
			return;
		}
		sign_key_len = 16;
		encr_key_len = 16;
		memcpy(keydata.sign_key, keydata.encr_key, 16);
	} else if (res) {
		wpa_printf(MSG_DEBUG, "EAPOL: Could not get enough master key "
			   "data for decrypting EAPOL-Key keys (res=%d)", res);
		return;
	}

	/* The key replay_counter must increase when same master key */
	if (sm->replay_counter_valid &&
	    memcmp(sm->last_replay_counter, key->replay_counter,
		   IEEE8021X_REPLAY_COUNTER_LEN) >= 0) {
		wpa_printf(MSG_WARNING, "EAPOL: EAPOL-Key replay counter did "
			   "not increase - ignoring key");
		wpa_hexdump(MSG_DEBUG, "EAPOL: last replay counter",
			    sm->last_replay_counter,
			    IEEE8021X_REPLAY_COUNTER_LEN);
		wpa_hexdump(MSG_DEBUG, "EAPOL: received replay counter",
			    key->replay_counter, IEEE8021X_REPLAY_COUNTER_LEN);
		return;
	}

	/* Verify key signature (HMAC-MD5) */
	memcpy(orig_key_sign, key->key_signature, IEEE8021X_KEY_SIGN_LEN);
	memset(key->key_signature, 0, IEEE8021X_KEY_SIGN_LEN);
	hmac_md5(keydata.sign_key, sign_key_len,
		 sm->last_rx_key, sizeof(*hdr) + be_to_host16(hdr->length),
		 key->key_signature);
	if (memcmp(orig_key_sign, key->key_signature, IEEE8021X_KEY_SIGN_LEN)
	    != 0) {
		wpa_printf(MSG_DEBUG, "EAPOL: Invalid key signature in "
			   "EAPOL-Key packet");
		memcpy(key->key_signature, orig_key_sign,
		       IEEE8021X_KEY_SIGN_LEN);
		return;
	}
	wpa_printf(MSG_DEBUG, "EAPOL: EAPOL-Key key signature verified");

	key_len = be_to_host16(hdr->length) - sizeof(*key);
	if (key_len > 32 || rx_key_length > 32) {
		wpa_printf(MSG_WARNING, "EAPOL: Too long key data length %d",
			   key_len ? key_len : rx_key_length);
		return;
	}
	if (key_len == rx_key_length) {
		memcpy(ekey, key->key_iv, IEEE8021X_KEY_IV_LEN);
		memcpy(ekey + IEEE8021X_KEY_IV_LEN, keydata.encr_key,
		       encr_key_len);
		memcpy(datakey, key + 1, key_len);
		rc4(datakey, key_len, ekey,
		    IEEE8021X_KEY_IV_LEN + encr_key_len);
		wpa_hexdump_key(MSG_DEBUG, "EAPOL: Decrypted(RC4) key",
				datakey, key_len);
	} else if (key_len == 0) {
		/*
		 * IEEE 802.1X-2004 specifies that least significant Key Length
		 * octets from MS-MPPE-Send-Key are used as the key if the key
		 * data is not present. This seems to be meaning the beginning
		 * of the MS-MPPE-Send-Key. In addition, MS-MPPE-Send-Key in
		 * Supplicant corresponds to MS-MPPE-Recv-Key in Authenticator.
		 * Anyway, taking the beginning of the keying material from EAP
		 * seems to interoperate with Authenticators.
		 */
		key_len = rx_key_length;
		memcpy(datakey, keydata.encr_key, key_len);
		wpa_hexdump_key(MSG_DEBUG, "EAPOL: using part of EAP keying "
				"material data encryption key",
				datakey, key_len);
	} else {
		wpa_printf(MSG_DEBUG, "EAPOL: Invalid key data length %d "
			   "(key_length=%d)", key_len, rx_key_length);
		return;
	}

	sm->replay_counter_valid = TRUE;
	memcpy(sm->last_replay_counter, key->replay_counter,
	       IEEE8021X_REPLAY_COUNTER_LEN);

	wpa_printf(MSG_DEBUG, "EAPOL: Setting dynamic WEP key: %s keyidx %d "
		   "len %d",
		   key->key_index & IEEE8021X_KEY_INDEX_FLAG ?
		   "unicast" : "broadcast",
		   key->key_index & IEEE8021X_KEY_INDEX_MASK, key_len);

	if (sm->ctx->set_wep_key &&
	    sm->ctx->set_wep_key(sm->ctx->ctx,
				 key->key_index & IEEE8021X_KEY_INDEX_FLAG,
				 key->key_index & IEEE8021X_KEY_INDEX_MASK,
				 datakey, key_len) < 0) {
		wpa_printf(MSG_WARNING, "EAPOL: Failed to set WEP key to the "
			   " driver.");
	} else {
		if (key->key_index & IEEE8021X_KEY_INDEX_FLAG)
			sm->unicast_key_received = TRUE;
		else
			sm->broadcast_key_received = TRUE;

		if ((sm->unicast_key_received ||
		     !(sm->conf.required_keys & EAPOL_REQUIRE_KEY_UNICAST)) &&
		    (sm->broadcast_key_received ||
		     !(sm->conf.required_keys & EAPOL_REQUIRE_KEY_BROADCAST)))
		{
			wpa_printf(MSG_DEBUG, "EAPOL: all required EAPOL-Key "
				   "frames received");
			sm->portValid = TRUE;
			if (sm->ctx->eapol_done_cb)
				sm->ctx->eapol_done_cb(sm->ctx->ctx);
		}
	}
}


static void eapol_sm_getSuppRsp(struct eapol_sm *sm)
{
	wpa_printf(MSG_DEBUG, "EAPOL: getSuppRsp");
	/* EAP layer processing; no special code is needed, since Supplicant
	 * Backend state machine is waiting for eapNoResp or eapResp to be set
	 * and these are only set in the EAP state machine when the processing
	 * has finished. */
}


static void eapol_sm_txSuppRsp(struct eapol_sm *sm)
{
	u8 *resp;
	size_t resp_len;

	wpa_printf(MSG_DEBUG, "EAPOL: txSuppRsp");
	resp = eap_get_eapRespData(sm->eap, &resp_len);
	if (resp == NULL) {
		wpa_printf(MSG_WARNING, "EAPOL: txSuppRsp - EAP response data "
			   "not available");
		return;
	}

	/* Send EAP-Packet from the EAP layer to the Authenticator */
	sm->ctx->eapol_send(sm->ctx->eapol_send_ctx,
			    IEEE802_1X_TYPE_EAP_PACKET, resp, resp_len);

	/* eapRespData is not used anymore, so free it here */
	free(resp);

	if (sm->initial_req)
		sm->dot1xSuppEapolReqIdFramesRx++;
	else
		sm->dot1xSuppEapolReqFramesRx++;
	sm->dot1xSuppEapolRespFramesTx++;
	sm->dot1xSuppEapolFramesTx++;
}


static void eapol_sm_abortSupp(struct eapol_sm *sm)
{
	/* release system resources that may have been allocated for the
	 * authentication session */
	free(sm->last_rx_key);
	sm->last_rx_key = NULL;
	free(sm->eapReqData);
	sm->eapReqData = NULL;
	eap_sm_abort(sm->eap);
}


static void eapol_sm_step_timeout(void *eloop_ctx, void *timeout_ctx)
{
	eapol_sm_step(timeout_ctx);
}


/**
 * eapol_sm_step - EAPOL state machine step function
 * @sm: Pointer to EAPOL state machine allocated with eapol_sm_init()
 *
 * This function is called to notify the state machine about changed external
 * variables. It will step through the EAPOL state machines in loop to process
 * all triggered state changes.
 */
void eapol_sm_step(struct eapol_sm *sm)
{
	int i;

	/* In theory, it should be ok to run this in loop until !changed.
	 * However, it is better to use a limit on number of iterations to
	 * allow events (e.g., SIGTERM) to stop the program cleanly if the
	 * state machine were to generate a busy loop. */
	for (i = 0; i < 100; i++) {
		sm->changed = FALSE;
		SM_STEP_RUN(SUPP_PAE);
		SM_STEP_RUN(KEY_RX);
		SM_STEP_RUN(SUPP_BE);
		if (eap_sm_step(sm->eap))
			sm->changed = TRUE;
	}

	if (sm->changed) {
		/* restart EAPOL state machine step from timeout call in order
		 * to allow other events to be processed. */
		eloop_cancel_timeout(eapol_sm_step_timeout, NULL, sm);
		eloop_register_timeout(0, 0, eapol_sm_step_timeout, NULL, sm);
	}

	if (sm->ctx->cb && sm->cb_status != EAPOL_CB_IN_PROGRESS) {
		int success = sm->cb_status == EAPOL_CB_SUCCESS ? 1 : 0;
		sm->cb_status = EAPOL_CB_IN_PROGRESS;
		sm->ctx->cb(sm, success, sm->ctx->cb_ctx);
	}
}


static const char *eapol_supp_pae_state(int state)
{
	switch (state) {
	case SUPP_PAE_LOGOFF:
		return "LOGOFF";
	case SUPP_PAE_DISCONNECTED:
		return "DISCONNECTED";
	case SUPP_PAE_CONNECTING:
		return "CONNECTING";
	case SUPP_PAE_AUTHENTICATING:
		return "AUTHENTICATING";
	case SUPP_PAE_HELD:
		return "HELD";
	case SUPP_PAE_AUTHENTICATED:
		return "AUTHENTICATED";
	case SUPP_PAE_RESTART:
		return "RESTART";
	default:
		return "UNKNOWN";
	}
}


static const char *eapol_supp_be_state(int state)
{
	switch (state) {
	case SUPP_BE_REQUEST:
		return "REQUEST";
	case SUPP_BE_RESPONSE:
		return "RESPONSE";
	case SUPP_BE_SUCCESS:
		return "SUCCESS";
	case SUPP_BE_FAIL:
		return "FAIL";
	case SUPP_BE_TIMEOUT:
		return "TIMEOUT";
	case SUPP_BE_IDLE:
		return "IDLE";
	case SUPP_BE_INITIALIZE:
		return "INITIALIZE";
	case SUPP_BE_RECEIVE:
		return "RECEIVE";
	default:
		return "UNKNOWN";
	}
}


static const char * eapol_port_status(PortStatus status)
{
	if (status == Authorized)
		return "Authorized";
	else
		return "Unauthorized";
}


static const char * eapol_port_control(PortControl ctrl)
{
	switch (ctrl) {
	case Auto:
		return "Auto";
	case ForceUnauthorized:
		return "ForceUnauthorized";
	case ForceAuthorized:
		return "ForceAuthorized";
	default:
		return "Unknown";
	}
}


/**
 * eapol_sm_configure - Set EAPOL variables
 * @sm: Pointer to EAPOL state machine allocated with eapol_sm_init()
 * @heldPeriod: dot1xSuppHeldPeriod
 * @authPeriod: dot1xSuppAuthPeriod
 * @startPeriod: dot1xSuppStartPeriod
 * @maxStart: dot1xSuppMaxStart
 *
 * Set configurable EAPOL state machine variables. Each variable can be set to
 * the given value or ignored if set to -1 (to set only some of the variables).
 */
void eapol_sm_configure(struct eapol_sm *sm, int heldPeriod, int authPeriod,
			int startPeriod, int maxStart)
{
	if (sm == NULL)
		return;
	if (heldPeriod >= 0)
		sm->heldPeriod = heldPeriod;
	if (authPeriod >= 0)
		sm->authPeriod = authPeriod;
	if (startPeriod >= 0)
		sm->startPeriod = startPeriod;
	if (maxStart >= 0)
		sm->maxStart = maxStart;
}


/**
 * eapol_sm_get_status - Get EAPOL state machine status
 * @sm: Pointer to EAPOL state machine allocated with eapol_sm_init()
 * @buf: Buffer for status information
 * @buflen: Maximum buffer length
 * @verbose: Whether to include verbose status information
 * Returns: Number of bytes written to buf.
 *
 * Query EAPOL state machine for status information. This function fills in a
 * text area with current status information from the EAPOL state machine. If
 * the buffer (buf) is not large enough, status information will be truncated
 * to fit the buffer.
 */
int eapol_sm_get_status(struct eapol_sm *sm, char *buf, size_t buflen,
			int verbose)
{
	int len;
	if (sm == NULL)
		return 0;

	len = snprintf(buf, buflen,
		       "Supplicant PAE state=%s\n"
		       "suppPortStatus=%s\n",
		       eapol_supp_pae_state(sm->SUPP_PAE_state),
		       eapol_port_status(sm->suppPortStatus));

	if (verbose) {
		len += snprintf(buf + len, buflen - len,
				"heldPeriod=%u\n"
				"authPeriod=%u\n"
				"startPeriod=%u\n"
				"maxStart=%u\n"
				"portControl=%s\n"
				"Supplicant Backend state=%s\n",
				sm->heldPeriod,
				sm->authPeriod,
				sm->startPeriod,
				sm->maxStart,
				eapol_port_control(sm->portControl),
				eapol_supp_be_state(sm->SUPP_BE_state));
	}

	len += eap_sm_get_status(sm->eap, buf + len, buflen - len, verbose);

	return len;
}


/**
 * eapol_sm_get_mib - Get EAPOL state machine MIBs
 * @sm: Pointer to EAPOL state machine allocated with eapol_sm_init()
 * @buf: Buffer for MIB information
 * @buflen: Maximum buffer length
 * Returns: Number of bytes written to buf.
 *
 * Query EAPOL state machine for MIB information. This function fills in a
 * text area with current MIB information from the EAPOL state machine. If
 * the buffer (buf) is not large enough, MIB information will be truncated to
 * fit the buffer.
 */
int eapol_sm_get_mib(struct eapol_sm *sm, char *buf, size_t buflen)
{
	int len;
	if (sm == NULL)
		return 0;
	len = snprintf(buf, buflen,
		       "dot1xSuppPaeState=%d\n"
		       "dot1xSuppHeldPeriod=%u\n"
		       "dot1xSuppAuthPeriod=%u\n"
		       "dot1xSuppStartPeriod=%u\n"
		       "dot1xSuppMaxStart=%u\n"
		       "dot1xSuppSuppControlledPortStatus=%s\n"
		       "dot1xSuppBackendPaeState=%d\n"
		       "dot1xSuppEapolFramesRx=%u\n"
		       "dot1xSuppEapolFramesTx=%u\n"
		       "dot1xSuppEapolStartFramesTx=%u\n"
		       "dot1xSuppEapolLogoffFramesTx=%u\n"
		       "dot1xSuppEapolRespFramesTx=%u\n"
		       "dot1xSuppEapolReqIdFramesRx=%u\n"
		       "dot1xSuppEapolReqFramesRx=%u\n"
		       "dot1xSuppInvalidEapolFramesRx=%u\n"
		       "dot1xSuppEapLengthErrorFramesRx=%u\n"
		       "dot1xSuppLastEapolFrameVersion=%u\n"
		       "dot1xSuppLastEapolFrameSource=" MACSTR "\n",
		       sm->SUPP_PAE_state,
		       sm->heldPeriod,
		       sm->authPeriod,
		       sm->startPeriod,
		       sm->maxStart,
		       sm->suppPortStatus == Authorized ?
		       "Authorized" : "Unauthorized",
		       sm->SUPP_BE_state,
		       sm->dot1xSuppEapolFramesRx,
		       sm->dot1xSuppEapolFramesTx,
		       sm->dot1xSuppEapolStartFramesTx,
		       sm->dot1xSuppEapolLogoffFramesTx,
		       sm->dot1xSuppEapolRespFramesTx,
		       sm->dot1xSuppEapolReqIdFramesRx,
		       sm->dot1xSuppEapolReqFramesRx,
		       sm->dot1xSuppInvalidEapolFramesRx,
		       sm->dot1xSuppEapLengthErrorFramesRx,
		       sm->dot1xSuppLastEapolFrameVersion,
		       MAC2STR(sm->dot1xSuppLastEapolFrameSource));
	return len;
}


/**
 * eapol_sm_rx_eapol - Process received EAPOL frames
 * @sm: Pointer to EAPOL state machine allocated with eapol_sm_init()
 * @src: Source MAC address of the EAPOL packet
 * @buf: Pointer to the beginning of the EAPOL data (EAPOL header)
 * @len: Length of the EAPOL frame
 * Returns: 1 = EAPOL frame processed, 0 = not for EAPOL state machine,
 * -1 failure
 */
int eapol_sm_rx_eapol(struct eapol_sm *sm, const u8 *src, const u8 *buf,
		      size_t len)
{
	const struct ieee802_1x_hdr *hdr;
	const struct ieee802_1x_eapol_key *key;
	int plen, data_len;
	int res = 1;

	if (sm == NULL)
		return 0;
	sm->dot1xSuppEapolFramesRx++;
	if (len < sizeof(*hdr)) {
		sm->dot1xSuppInvalidEapolFramesRx++;
		return 0;
	}
	hdr = (const struct ieee802_1x_hdr *) buf;
	sm->dot1xSuppLastEapolFrameVersion = hdr->version;
	memcpy(sm->dot1xSuppLastEapolFrameSource, src, ETH_ALEN);
	if (hdr->version < EAPOL_VERSION) {
		/* TODO: backwards compatibility */
	}
	plen = be_to_host16(hdr->length);
	if (plen > len - sizeof(*hdr)) {
		sm->dot1xSuppEapLengthErrorFramesRx++;
		return 0;
	}
	data_len = plen + sizeof(*hdr);

	switch (hdr->type) {
	case IEEE802_1X_TYPE_EAP_PACKET:
		if (sm->cached_pmk) {
			/* Trying to use PMKSA caching, but Authenticator did
			 * not seem to have a matching entry. Need to restart
			 * EAPOL state machines.
			 */
			eapol_sm_abort_cached(sm);
		}
		free(sm->eapReqData);
		sm->eapReqDataLen = plen;
		sm->eapReqData = malloc(sm->eapReqDataLen);
		if (sm->eapReqData) {
			wpa_printf(MSG_DEBUG, "EAPOL: Received EAP-Packet "
				   "frame");
			memcpy(sm->eapReqData, (u8 *) (hdr + 1),
			       sm->eapReqDataLen);
			sm->eapolEap = TRUE;
			eapol_sm_step(sm);
		}
		break;
	case IEEE802_1X_TYPE_EAPOL_KEY:
		if (plen < sizeof(*key)) {
			wpa_printf(MSG_DEBUG, "EAPOL: Too short EAPOL-Key "
				   "frame received");
			break;
		}
		key = (const struct ieee802_1x_eapol_key *) (hdr + 1);
		if (key->type == EAPOL_KEY_TYPE_WPA ||
		    key->type == EAPOL_KEY_TYPE_RSN) {
			/* WPA Supplicant takes care of this frame. */
			wpa_printf(MSG_DEBUG, "EAPOL: Ignoring WPA EAPOL-Key "
				   "frame in EAPOL state machines");
			res = 0;
			break;
		}
		if (key->type != EAPOL_KEY_TYPE_RC4) {
			wpa_printf(MSG_DEBUG, "EAPOL: Ignored unknown "
				   "EAPOL-Key type %d", key->type);
			break;
		}
		free(sm->last_rx_key);
		sm->last_rx_key = malloc(data_len);
		if (sm->last_rx_key) {
			wpa_printf(MSG_DEBUG, "EAPOL: Received EAPOL-Key "
				   "frame");
			memcpy(sm->last_rx_key, buf, data_len);
			sm->last_rx_key_len = data_len;
			sm->rxKey = TRUE;
			eapol_sm_step(sm);
		}
		break;
	default:
		wpa_printf(MSG_DEBUG, "EAPOL: Received unknown EAPOL type %d",
			   hdr->type);
		sm->dot1xSuppInvalidEapolFramesRx++;
		break;
	}

	return res;
}


/**
 * eapol_sm_notify_tx_eapol_key - Notification about transmitted EAPOL packet
 * @sm: Pointer to EAPOL state machine allocated with eapol_sm_init()
 *
 * Notify EAPOL station machine about transmitted EAPOL packet from an external
 * component, e.g., WPA. This will update the statistics.
 */
void eapol_sm_notify_tx_eapol_key(struct eapol_sm *sm)
{
	if (sm)
		sm->dot1xSuppEapolFramesTx++;
}


/**
 * eapol_sm_notify_portEnabled - Notification about portEnabled change
 * @sm: Pointer to EAPOL state machine allocated with eapol_sm_init()
 * @enabled: New portEnabled value
 *
 * Notify EAPOL station machine about new portEnabled value.
 */
void eapol_sm_notify_portEnabled(struct eapol_sm *sm, Boolean enabled)
{
	if (sm == NULL)
		return;
	wpa_printf(MSG_DEBUG, "EAPOL: External notification - "
		   "portEnabled=%d", enabled);
	sm->portEnabled = enabled;
	eapol_sm_step(sm);
}


/**
 * eapol_sm_notify_portValid - Notification about portValid change
 * @sm: Pointer to EAPOL state machine allocated with eapol_sm_init()
 * @valid: New portValid value
 *
 * Notify EAPOL station machine about new portValid value.
 */
void eapol_sm_notify_portValid(struct eapol_sm *sm, Boolean valid)
{
	if (sm == NULL)
		return;
	wpa_printf(MSG_DEBUG, "EAPOL: External notification - "
		   "portValid=%d", valid);
	sm->portValid = valid;
	eapol_sm_step(sm);
}


/**
 * eapol_sm_notify_eap_success - Notification of external EAP success trigger
 * @sm: Pointer to EAPOL state machine allocated with eapol_sm_init()
 * @success: %TRUE = set success, %FALSE = clear success
 *
 * Notify EAPOL station machine that external event has forced EAP state to
 * success (success = %TRUE). This can be cleared by setting success = %FALSE.
 *
 * This function is called to update EAP state when WPA-PSK key handshake has
 * been completed successfully since WPA-PSK does not use EAP state machine.
 */
void eapol_sm_notify_eap_success(struct eapol_sm *sm, Boolean success)
{
	if (sm == NULL)
		return;
	wpa_printf(MSG_DEBUG, "EAPOL: External notification - "
		   "EAP success=%d", success);
	sm->eapSuccess = success;
	sm->altAccept = success;
	if (success)
		eap_notify_success(sm->eap);
	eapol_sm_step(sm);
}


/**
 * eapol_sm_notify_eap_fail - Notification of external EAP failure trigger
 * @sm: Pointer to EAPOL state machine allocated with eapol_sm_init()
 * @fail: %TRUE = set failure, %FALSE = clear failure
 *
 * Notify EAPOL station machine that external event has forced EAP state to
 * failure (fail = %TRUE). This can be cleared by setting fail = %FALSE.
 */
void eapol_sm_notify_eap_fail(struct eapol_sm *sm, Boolean fail)
{
	if (sm == NULL)
		return;
	wpa_printf(MSG_DEBUG, "EAPOL: External notification - "
		   "EAP fail=%d", fail);
	sm->eapFail = fail;
	sm->altReject = fail;
	eapol_sm_step(sm);
}


/**
 * eapol_sm_notify_config - Notification of EAPOL configuration change
 * @sm: Pointer to EAPOL state machine allocated with eapol_sm_init()
 * @config: Pointer to current network configuration
 * @conf: Pointer to EAPOL configuration data
 *
 * Notify EAPOL station machine that configuration has changed. config will be
 * stored as a backpointer to network configuration. This can be %NULL to clear
 * the stored pointed. conf will be copied to local EAPOL/EAP configuration
 * data. If conf is %NULL, this part of the configuration change will be
 * skipped.
 */
void eapol_sm_notify_config(struct eapol_sm *sm, struct wpa_ssid *config,
			    const struct eapol_config *conf)
{
	if (sm == NULL)
		return;

	sm->config = config;

	if (conf == NULL)
		return;

	sm->conf.accept_802_1x_keys = conf->accept_802_1x_keys;
	sm->conf.required_keys = conf->required_keys;
	sm->conf.fast_reauth = conf->fast_reauth;
	if (sm->eap) {
		eap_set_fast_reauth(sm->eap, conf->fast_reauth);
		eap_set_workaround(sm->eap, conf->workaround);
		eap_set_force_disabled(sm->eap, conf->eap_disabled);
	}
}


/**
 * eapol_sm_get_key - Get master session key (MSK) from EAP
 * @sm: Pointer to EAPOL state machine allocated with eapol_sm_init()
 * @key: Pointer for key buffer
 * @len: Number of bytes to copy to key
 * Returns: 0 on success (len of key available), maximum available key len
 * (>0) if key is available but it is shorter than len, or -1 on failure.
 *
 * Fetch EAP keying material (MSK, eapKeyData) from EAP state machine. The key
 * is available only after a successful authentication.
 */
int eapol_sm_get_key(struct eapol_sm *sm, u8 *key, size_t len)
{
	const u8 *eap_key;
	size_t eap_len;

	if (sm == NULL || !eap_key_available(sm->eap))
		return -1;
	eap_key = eap_get_eapKeyData(sm->eap, &eap_len);
	if (eap_key == NULL)
		return -1;
	if (len > eap_len)
		return eap_len;
	memcpy(key, eap_key, len);
	return 0;
}


/**
 * eapol_sm_notify_logoff - Notification of logon/logoff commands
 * @sm: Pointer to EAPOL state machine allocated with eapol_sm_init()
 * @logoff: Whether command was logoff
 *
 * Notify EAPOL state machines that user requested logon/logoff.
 */
void eapol_sm_notify_logoff(struct eapol_sm *sm, Boolean logoff)
{
	if (sm) {
		sm->userLogoff = logoff;
		eapol_sm_step(sm);
	}
}


/**
 * eapol_sm_notify_pmkid_attempt - Notification of successful PMKSA caching
 * @sm: Pointer to EAPOL state machine allocated with eapol_sm_init()
 *
 * Notify EAPOL state machines that PMKSA caching was successful. This is used
 * to move EAPOL and EAP state machines into authenticated/successful state.
 */
void eapol_sm_notify_cached(struct eapol_sm *sm)
{
	if (sm == NULL)
		return;
	sm->SUPP_PAE_state = SUPP_PAE_AUTHENTICATED;
	sm->suppPortStatus = Authorized;
	eap_notify_success(sm->eap);
}


/**
 * eapol_sm_notify_pmkid_attempt - Notification of PMKSA caching
 * @sm: Pointer to EAPOL state machine allocated with eapol_sm_init()
 * @attempt: Whether PMKSA caching is tried
 *
 * Notify EAPOL state machines whether PMKSA caching is used.
 */
void eapol_sm_notify_pmkid_attempt(struct eapol_sm *sm, int attempt)
{
	if (sm == NULL)
		return;
	if (attempt) {
		wpa_printf(MSG_DEBUG, "RSN: Trying to use cached PMKSA");
		sm->cached_pmk = TRUE;
	} else {
		wpa_printf(MSG_DEBUG, "RSN: Do not try to use cached PMKSA");
		sm->cached_pmk = FALSE;
	}
}


static void eapol_sm_abort_cached(struct eapol_sm *sm)
{
	wpa_printf(MSG_DEBUG, "RSN: Authenticator did not accept PMKID, "
		   "doing full EAP authentication");
	if (sm == NULL)
		return;
	sm->cached_pmk = FALSE;
	sm->SUPP_PAE_state = SUPP_PAE_CONNECTING;
	sm->suppPortStatus = Unauthorized;
	sm->eapRestart= TRUE;
}


/**
 * eapol_sm_register_scard_ctx - Notification of smart card context
 * @sm: Pointer to EAPOL state machine allocated with eapol_sm_init()
 * @ctx: Context data for smart card operations
 *
 * Notify EAPOL state machines of context data for smart card operations. This
 * context data will be used as a parameter for scard_*() functions.
 */
void eapol_sm_register_scard_ctx(struct eapol_sm *sm, void *ctx)
{
	if (sm) {
		sm->ctx->scard_ctx = ctx;
		eap_register_scard_ctx(sm->eap, ctx);
	}
}


/**
 * eapol_sm_notify_portControl - Notification of portControl changes
 * @sm: Pointer to EAPOL state machine allocated with eapol_sm_init()
 * @portControl: New value for portControl variable
 *
 * Notify EAPOL state machines that portControl variable has changed.
 */
void eapol_sm_notify_portControl(struct eapol_sm *sm, PortControl portControl)
{
	if (sm == NULL)
		return;
	wpa_printf(MSG_DEBUG, "EAPOL: External notification - "
		   "portControl=%s", eapol_port_control(portControl));
	sm->portControl = portControl;
	eapol_sm_step(sm);
}


/**
 * eapol_sm_notify_ctrl_attached - Notification of attached monitor
 * @sm: Pointer to EAPOL state machine allocated with eapol_sm_init()
 *
 * Notify EAPOL state machines that a monitor was attached to the control
 * interface to trigger re-sending of pending requests for user input.
 */
void eapol_sm_notify_ctrl_attached(struct eapol_sm *sm)
{
	if (sm == NULL)
		return;
	eap_sm_notify_ctrl_attached(sm->eap);
}


/**
 * eapol_sm_notify_ctrl_response - Notification of received user input
 * @sm: Pointer to EAPOL state machine allocated with eapol_sm_init()
 *
 * Notify EAPOL state machines that a control response, i.e., user
 * input, was received in order to trigger retrying of a pending EAP request.
 */
void eapol_sm_notify_ctrl_response(struct eapol_sm *sm)
{
	if (sm == NULL)
		return;
	if (sm->eapReqData && !sm->eapReq) {
		wpa_printf(MSG_DEBUG, "EAPOL: received control response (user "
			   "input) notification - retrying pending EAP "
			   "Request");
		sm->eapolEap = TRUE;
		sm->eapReq = TRUE;
		eapol_sm_step(sm);
	}
}


/**
 * eapol_sm_request_reauth - Request reauthentication
 * @sm: Pointer to EAPOL state machine allocated with eapol_sm_init()
 *
 * This function can be used to request EAPOL reauthentication, e.g., when the
 * current PMKSA entry is nearing expiration.
 */
void eapol_sm_request_reauth(struct eapol_sm *sm)
{
	if (sm == NULL || sm->SUPP_PAE_state != SUPP_PAE_AUTHENTICATED)
		return;
	eapol_sm_txStart(sm);
}


/**
 * eapol_sm_notify_lower_layer_success - Notification of lower layer success
 * @sm: Pointer to EAPOL state machine allocated with eapol_sm_init()
 *
 * Notify EAPOL (and EAP) state machines that a lower layer has detected a
 * successful authentication. This is used to recover from dropped EAP-Success
 * messages.
 */
void eapol_sm_notify_lower_layer_success(struct eapol_sm *sm)
{
	if (sm == NULL)
		return;
	eap_notify_lower_layer_success(sm->eap);
}


static struct wpa_ssid * eapol_sm_get_config(void *ctx)
{
	struct eapol_sm *sm = ctx;
	return sm ? sm->config : NULL;
}


static u8 * eapol_sm_get_eapReqData(void *ctx, size_t *len)
{
	struct eapol_sm *sm = ctx;
	if (sm == NULL || sm->eapReqData == NULL) {
		*len = 0;
		return NULL;
	}

	*len = sm->eapReqDataLen;
	return sm->eapReqData;
}


static Boolean eapol_sm_get_bool(void *ctx, enum eapol_bool_var variable)
{
	struct eapol_sm *sm = ctx;
	if (sm == NULL)
		return FALSE;
	switch (variable) {
	case EAPOL_eapSuccess:
		return sm->eapSuccess;
	case EAPOL_eapRestart:
		return sm->eapRestart;
	case EAPOL_eapFail:
		return sm->eapFail;
	case EAPOL_eapResp:
		return sm->eapResp;
	case EAPOL_eapNoResp:
		return sm->eapNoResp;
	case EAPOL_eapReq:
		return sm->eapReq;
	case EAPOL_portEnabled:
		return sm->portEnabled;
	case EAPOL_altAccept:
		return sm->altAccept;
	case EAPOL_altReject:
		return sm->altReject;
	}
	return FALSE;
}


static void eapol_sm_set_bool(void *ctx, enum eapol_bool_var variable,
			      Boolean value)
{
	struct eapol_sm *sm = ctx;
	if (sm == NULL)
		return;
	switch (variable) {
	case EAPOL_eapSuccess:
		sm->eapSuccess = value;
		break;
	case EAPOL_eapRestart:
		sm->eapRestart = value;
		break;
	case EAPOL_eapFail:
		sm->eapFail = value;
		break;
	case EAPOL_eapResp:
		sm->eapResp = value;
		break;
	case EAPOL_eapNoResp:
		sm->eapNoResp = value;
		break;
	case EAPOL_eapReq:
		sm->eapReq = value;
		break;
	case EAPOL_portEnabled:
		sm->portEnabled = value;
		break;
	case EAPOL_altAccept:
		sm->altAccept = value;
		break;
	case EAPOL_altReject:
		sm->altReject = value;
		break;
	}
}


static unsigned int eapol_sm_get_int(void *ctx, enum eapol_int_var variable)
{
	struct eapol_sm *sm = ctx;
	if (sm == NULL)
		return 0;
	switch (variable) {
	case EAPOL_idleWhile:
		return sm->idleWhile;
	}
	return 0;
}


static void eapol_sm_set_int(void *ctx, enum eapol_int_var variable,
			     unsigned int value)
{
	struct eapol_sm *sm = ctx;
	if (sm == NULL)
		return;
	switch (variable) {
	case EAPOL_idleWhile:
		sm->idleWhile = value;
		break;
	}
}


static void eapol_sm_set_config_blob(void *ctx, struct wpa_config_blob *blob)
{
	struct eapol_sm *sm = ctx;
	if (sm && sm->ctx && sm->ctx->set_config_blob)
		sm->ctx->set_config_blob(sm->ctx->ctx, blob);
}


static const struct wpa_config_blob *
eapol_sm_get_config_blob(void *ctx, const char *name)
{
	struct eapol_sm *sm = ctx;
	if (sm && sm->ctx && sm->ctx->get_config_blob)
		return sm->ctx->get_config_blob(sm->ctx->ctx, name);
	else
		return NULL;
}


static struct eapol_callbacks eapol_cb =
{
	.get_config = eapol_sm_get_config,
	.get_bool = eapol_sm_get_bool,
	.set_bool = eapol_sm_set_bool,
	.get_int = eapol_sm_get_int,
	.set_int = eapol_sm_set_int,
	.get_eapReqData = eapol_sm_get_eapReqData,
	.set_config_blob = eapol_sm_set_config_blob,
	.get_config_blob = eapol_sm_get_config_blob,
};


/**
 * eapol_sm_init - Initialize EAPOL state machine
 * @ctx: Pointer to EAPOL context data; this needs to be an allocated buffer
 * and EAPOL state machine will free it in eapol_sm_deinit()
 * Returns: Pointer to the allocated EAPOL state machine or %NULL on failure
 *
 * Allocate and initialize an EAPOL state machine.
 */
struct eapol_sm *eapol_sm_init(struct eapol_ctx *ctx)
{
	struct eapol_sm *sm;
	struct eap_config conf;
	sm = malloc(sizeof(*sm));
	if (sm == NULL)
		return NULL;
	memset(sm, 0, sizeof(*sm));
	sm->ctx = ctx;

	sm->portControl = Auto;

	/* Supplicant PAE state machine */
	sm->heldPeriod = 60;
	sm->startPeriod = 30;
	sm->maxStart = 3;

	/* Supplicant Backend state machine */
	sm->authPeriod = 30;

	memset(&conf, 0, sizeof(conf));
	conf.opensc_engine_path = ctx->opensc_engine_path;
	conf.pkcs11_engine_path = ctx->pkcs11_engine_path;
	conf.pkcs11_module_path = ctx->pkcs11_module_path;

	sm->eap = eap_sm_init(sm, &eapol_cb, sm->ctx->msg_ctx, &conf);
	if (sm->eap == NULL) {
		free(sm);
		return NULL;
	}

	/* Initialize EAPOL state machines */
	sm->initialize = TRUE;
	eapol_sm_step(sm);
	sm->initialize = FALSE;
	eapol_sm_step(sm);

	eloop_register_timeout(1, 0, eapol_port_timers_tick, NULL, sm);

	return sm;
}


/**
 * eapol_sm_deinit - Deinitialize EAPOL state machine
 * @sm: Pointer to EAPOL state machine allocated with eapol_sm_init()
 *
 * Deinitialize and free EAPOL state machine.
 */
void eapol_sm_deinit(struct eapol_sm *sm)
{
	if (sm == NULL)
		return;
	eloop_cancel_timeout(eapol_sm_step_timeout, NULL, sm);
	eloop_cancel_timeout(eapol_port_timers_tick, NULL, sm);
	eap_sm_deinit(sm->eap);
	free(sm->last_rx_key);
	free(sm->eapReqData);
	free(sm->ctx);
	free(sm);
}
