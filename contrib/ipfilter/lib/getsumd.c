/*	$FreeBSD$	*/

#include "ipf.h"

char *getsumd(sum)
u_32_t sum;
{
	static char sumdbuf[17];

	if (sum & NAT_HW_CKSUM)
		sprintf(sumdbuf, "hw(%#0x)", sum & 0xffff);
	else
		sprintf(sumdbuf, "%#0x", sum);
	return sumdbuf;
}
