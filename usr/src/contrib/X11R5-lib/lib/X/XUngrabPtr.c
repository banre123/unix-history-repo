/* $XConsortium: XUngrabPtr.c,v 11.9 91/01/06 11:48:37 rws Exp $ */
/* Copyright    Massachusetts Institute of Technology    1986	*/

/*
Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation, and that the name of M.I.T. not be used in advertising or
publicity pertaining to distribution of the software without specific,
written prior permission.  M.I.T. makes no representations about the
suitability of this software for any purpose.  It is provided "as is"
without express or implied warranty.
*/

#include "Xlibint.h"

XUngrabPointer(dpy, time)
register Display *dpy;
Time time;
{
    register xResourceReq *req;

    LockDisplay(dpy);
    GetResReq(UngrabPointer, time, req);
    UnlockDisplay(dpy);
    SyncHandle();
}
