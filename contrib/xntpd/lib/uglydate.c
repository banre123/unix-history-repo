/* uglydate.c,v 3.1 1993/07/06 01:08:53 jbj Exp
 * uglydate - convert a time stamp to something barely readable
 *	      The string returned is 37 characters long.
 */
#include <stdio.h>

#include "ntp_fp.h"
#include "ntp_unixtime.h"
#include "lib_strbuf.h"
#include "ntp_stdlib.h"
#ifdef NTP_POSIX_SOURCE
#include <time.h>
#endif

char *
uglydate(ts)
	l_fp *ts;
{
	char *bp;
	char *timep;
	struct tm *tm;
	U_LONG sec;
	U_LONG msec;
	int year;

	timep = ulfptoa(ts, 6);		/* returns max 17 characters */

	LIB_GETBUF(bp);
	
	sec = ts->l_ui - JAN_1970;
	msec = ts->l_uf / 4294967;	/* fract / (2**32/1000) */
	tm = gmtime((LONG *)&sec);
	if (ts->l_ui == 0) {
		/*
		 * Probably not a real good thing to do.  Oh, well.
		 */
		year = 0;
		tm->tm_yday = 0;
		tm->tm_hour = 0;
		tm->tm_min = 0;
		tm->tm_sec = 0;
	} else {
		year = tm->tm_year;
		while (year >= 100)
			year -= 100;
	}

	(void) sprintf(bp, "%17s %02d:%03d:%02d:%02d:%02d.%03d",
	    timep, year, tm->tm_yday, tm->tm_hour, tm->tm_min,
	    tm->tm_sec, msec);

	return bp;
}
