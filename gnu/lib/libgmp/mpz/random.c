/* mpz_random -- Generate a random mpz_t of specified size.
   This function is non-portable and generates poor random numbers.

Copyright (C) 1991, 1993, 1994, 1995 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Library General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
License for more details.

You should have received a copy of the GNU Library General Public License
along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include "gmp.h"
#include "gmp-impl.h"
#include "urandom.h"

void
#if __STDC__
mpz_random (mpz_ptr x, mp_size_t size)
#else
mpz_random (x, size)
     mpz_ptr x;
     mp_size_t size;
#endif
{
  mp_size_t i;
  mp_limb_t ran;
  mp_ptr xp;
  mp_size_t abs_size;

  abs_size = ABS (size);

  if (x->_mp_alloc < abs_size)
    _mpz_realloc (x, abs_size);

  xp = x->_mp_d;

  for (i = 0; i < abs_size; i++)
    {
      ran = urandom ();
      xp[i] = ran;
    }

  MPN_NORMALIZE (xp, abs_size);
  x->_mp_size = size < 0 ? -abs_size : abs_size;
}
