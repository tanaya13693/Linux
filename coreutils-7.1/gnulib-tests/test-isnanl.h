/* -*- buffer-read-only: t -*- vi: set ro: */
/* DO NOT EDIT! GENERATED AUTOMATICALLY! */
#line 1
/* Test of isnanl() substitute.
   Copyright (C) 2007-2009 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2007.  */

#include <float.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#define ASSERT(expr) \
  do									     \
    {									     \
      if (!(expr))							     \
        {								     \
          fprintf (stderr, "%s:%d: assertion failed\n", __FILE__, __LINE__); \
          fflush (stderr);						     \
          abort ();							     \
        }								     \
    }									     \
  while (0)

/* On HP-UX 10.20, negating 0.0L does not yield -0.0L.
   So we use minus_zero instead.
   IRIX cc can't put -0.0L into .data, but can compute at runtime.
   Note that the expression -LDBL_MIN * LDBL_MIN does not work on other
   platforms, such as when cross-compiling to PowerPC on MacOS X 10.5.  */
#if defined __hpux || defined __sgi
static long double
compute_minus_zero (void)
{
  return -LDBL_MIN * LDBL_MIN;
}
# define minus_zero compute_minus_zero ()
#else
long double minus_zero = -0.0L;
#endif

int
main ()
{
  #define NWORDS \
    ((sizeof (long double) + sizeof (unsigned int) - 1) / sizeof (unsigned int))
  typedef union { unsigned int word[NWORDS]; long double value; }
          memory_long_double;

  /* Finite values.  */
  ASSERT (!isnanl (3.141L));
  ASSERT (!isnanl (3.141e30L));
  ASSERT (!isnanl (3.141e-30L));
  ASSERT (!isnanl (-2.718L));
  ASSERT (!isnanl (-2.718e30L));
  ASSERT (!isnanl (-2.718e-30L));
  ASSERT (!isnanl (0.0L));
  ASSERT (!isnanl (minus_zero));
  /* Infinite values.  */
  ASSERT (!isnanl (1.0L / 0.0L));
  ASSERT (!isnanl (-1.0L / 0.0L));
  /* Quiet NaN.  */
  ASSERT (isnanl (0.0L / 0.0L));

#if defined LDBL_EXPBIT0_WORD && defined LDBL_EXPBIT0_BIT
  /* A bit pattern that is different from a Quiet NaN.  With a bit of luck,
     it's a Signalling NaN.  */
  {
    memory_long_double m;
    m.value = 0.0L / 0.0L;
# if LDBL_EXPBIT0_BIT > 0
    m.word[LDBL_EXPBIT0_WORD] ^= (unsigned int) 1 << (LDBL_EXPBIT0_BIT - 1);
# else
    m.word[LDBL_EXPBIT0_WORD + (LDBL_EXPBIT0_WORD < NWORDS / 2 ? 1 : - 1)]
      ^= (unsigned int) 1 << (sizeof (unsigned int) * CHAR_BIT - 1);
# endif
    m.word[LDBL_EXPBIT0_WORD + (LDBL_EXPBIT0_WORD < NWORDS / 2 ? 1 : - 1)]
      |= (unsigned int) 1 << LDBL_EXPBIT0_BIT;
    ASSERT (isnanl (m.value));
  }
#endif

#if ((defined __ia64 && LDBL_MANT_DIG == 64) || (defined __x86_64__ || defined __amd64__) || (defined __i386 || defined __i386__ || defined _I386 || defined _M_IX86 || defined _X86_))
/* Representation of an 80-bit 'long double' as an initializer for a sequence
   of 'unsigned int' words.  */
# ifdef WORDS_BIGENDIAN
#  define LDBL80_WORDS(exponent,manthi,mantlo) \
     { ((unsigned int) (exponent) << 16) | ((unsigned int) (manthi) >> 16), \
       ((unsigned int) (manthi) << 16) | (unsigned int) (mantlo) >> 16),    \
       (unsigned int) (mantlo) << 16                                        \
     }
# else
#  define LDBL80_WORDS(exponent,manthi,mantlo) \
     { mantlo, manthi, exponent }
# endif
  { /* Quiet NaN.  */
    static memory_long_double x =
      { LDBL80_WORDS (0xFFFF, 0xC3333333, 0x00000000) };
    ASSERT (isnanl (x.value));
  }
  {
    /* Signalling NaN.  */
    static memory_long_double x =
      { LDBL80_WORDS (0xFFFF, 0x83333333, 0x00000000) };
    ASSERT (isnanl (x.value));
  }
  /* The isnanl function should recognize Pseudo-NaNs, Pseudo-Infinities,
     Pseudo-Zeroes, Unnormalized Numbers, and Pseudo-Denormals, as defined in
       Intel IA-64 Architecture Software Developer's Manual, Volume 1:
       Application Architecture.
       Table 5-2 "Floating-Point Register Encodings"
       Figure 5-6 "Memory to Floating-Point Register Data Translation"
   */
  { /* Pseudo-NaN.  */
    static memory_long_double x =
      { LDBL80_WORDS (0xFFFF, 0x40000001, 0x00000000) };
    ASSERT (isnanl (x.value));
  }
  { /* Pseudo-Infinity.  */
    static memory_long_double x =
      { LDBL80_WORDS (0xFFFF, 0x00000000, 0x00000000) };
    ASSERT (isnanl (x.value));
  }
  { /* Pseudo-Zero.  */
    static memory_long_double x =
      { LDBL80_WORDS (0x4004, 0x00000000, 0x00000000) };
    ASSERT (isnanl (x.value));
  }
  { /* Unnormalized number.  */
    static memory_long_double x =
      { LDBL80_WORDS (0x4000, 0x63333333, 0x00000000) };
    ASSERT (isnanl (x.value));
  }
  { /* Pseudo-Denormal.  */
    static memory_long_double x =
      { LDBL80_WORDS (0x0000, 0x83333333, 0x00000000) };
    ASSERT (isnanl (x.value));
  }
#endif

  return 0;
}
