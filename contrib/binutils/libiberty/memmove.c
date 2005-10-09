/* Wrapper to implement ANSI C's memmove using BSD's bcopy. */
/* This function is in the public domain.  --Per Bothner. */

/*

@deftypefn Supplemental void* memmove (void *@var{from}, const void *@var{to}, size_t @var{count})

Copies @var{count} bytes from memory area @var{from} to memory area
@var{to}, returning a pointer to @var{to}.

@end deftypefn

*/

#include <ansidecl.h>
#ifdef ANSI_PROTOTYPES
#include <stddef.h>
#else
#define size_t unsigned long
#endif

void bcopy PARAMS ((const void*, void*, size_t));

PTR
memmove (s1, s2, n)
     PTR s1;
     const PTR s2;
     size_t n;
{
  bcopy (s2, s1, n);
  return s1;
}
