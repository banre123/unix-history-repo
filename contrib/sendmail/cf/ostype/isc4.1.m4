divert(-1)
#
# Copyright (c) 1998 Sendmail, Inc.  All rights reserved.
# Copyright (c) 1983 Eric P. Allman.  All rights reserved.
# Copyright (c) 1988, 1993
#	The Regents of the University of California.  All rights reserved.
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the sendmail distribution.
#
#
#

divert(0)
VERSIONID(`@(#)isc4.1.m4	8.10 (Berkeley) 5/19/98')
define(`ALIAS_FILE', /usr/lib/aliases)dnl
ifdef(`HELP_FILE',, `define(`HELP_FILE', /usr/lib/sendmail.hf)')dnl
ifdef(`LOCAL_MAILER_ARGS',, `define(`LOCAL_MAILER_ARGS', `lmail -s $u')')dnl
ifdef(`LOCAL_MAILER_FLAGS',, `define(`LOCAL_MAILER_FLAGS', `humS9')')dnl
ifdef(`LOCAL_MAILER_PATH',, `define(`LOCAL_MAILER_PATH', /bin/lmail)')dnl
ifdef(`QUEUE_DIR',, `define(`QUEUE_DIR', /usr/spool/mqueue)')dnl
ifdef(`STATUS_FILE',, `define(`STATUS_FILE', /usr/lib/sendmail.st)')dnl
ifdef(`UUCP_MAILER_ARGS',, `define(`UUCP_MAILER_ARGS', `uux - -r -gC $h!rmail ($u)')')dnl
ifdef(`UUCP_MAILER_PATH',, `define(`UUCP_MAILER_PATH', /usr/bin/uux)')dnl
define(`confTIME_ZONE', `USE_TZ')dnl
define(`confEBINDIR', `/usr/lib')dnl
