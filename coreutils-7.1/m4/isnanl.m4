# isnanl.m4 serial 11
dnl Copyright (C) 2007-2009 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_ISNANL],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  ISNANL_LIBM=
  gl_HAVE_ISNANL_NO_LIBM
  if test $gl_cv_func_isnanl_no_libm = no; then
    gl_HAVE_ISNANL_IN_LIBM
    if test $gl_cv_func_isnanl_in_libm = yes; then
      ISNANL_LIBM=-lm
    fi
  fi
  if test $gl_cv_func_isnanl_no_libm = yes \
     || test $gl_cv_func_isnanl_in_libm = yes; then
    save_LIBS="$LIBS"
    LIBS="$LIBS $ISNANL_LIBM"
    gl_FUNC_ISNANL_WORKS
    LIBS="$save_LIBS"
    case "$gl_cv_func_isnanl_works" in
      *yes) gl_func_isnanl=yes ;;
      *)    gl_func_isnanl=no; ISNANL_LIBM= ;;
    esac
  else
    gl_func_isnanl=no
  fi
  if test $gl_func_isnanl != yes; then
    HAVE_ISNANL=0
    gl_BUILD_ISNANL
  fi
  AC_SUBST([ISNANL_LIBM])
])

AC_DEFUN([gl_FUNC_ISNANL_NO_LIBM],
[
  gl_HAVE_ISNANL_NO_LIBM
  gl_func_isnanl_no_libm=$gl_cv_func_isnanl_no_libm
  if test $gl_func_isnanl_no_libm = yes; then
    gl_FUNC_ISNANL_WORKS
    case "$gl_cv_func_isnanl_works" in
      *yes) ;;
      *)    gl_func_isnanl_no_libm=no ;;
    esac
  fi
  if test $gl_func_isnanl_no_libm = yes; then
    AC_DEFINE([HAVE_ISNANL_IN_LIBC], [1],
      [Define if the isnan(long double) function is available in libc.])
  else
    gl_BUILD_ISNANL
  fi
])

dnl Pull in replacement isnanl definition. It does not need -lm.
AC_DEFUN([gl_BUILD_ISNANL],
[
  AC_LIBOBJ([isnanl])
  gl_LONG_DOUBLE_EXPONENT_LOCATION
])

dnl Test whether isnanl() can be used without libm.
AC_DEFUN([gl_HAVE_ISNANL_NO_LIBM],
[
  AC_CACHE_CHECK([whether isnan(long double) can be used without linking with libm],
    [gl_cv_func_isnanl_no_libm],
    [
      AC_TRY_LINK([#include <math.h>
                   #if __GNUC__ >= 4
                   # undef isnanl
                   # define isnanl(x) __builtin_isnanl ((long double)(x))
                   #elif defined isnan
                   # undef isnanl
                   # define isnanl(x) isnan ((long double)(x))
                   #endif
                   long double x;],
                  [return isnanl (x);],
        [gl_cv_func_isnanl_no_libm=yes],
        [gl_cv_func_isnanl_no_libm=no])
    ])
])

dnl Test whether isnanl() can be used with libm.
AC_DEFUN([gl_HAVE_ISNANL_IN_LIBM],
[
  AC_CACHE_CHECK([whether isnan(long double) can be used with libm],
    [gl_cv_func_isnanl_in_libm],
    [
      save_LIBS="$LIBS"
      LIBS="$LIBS -lm"
      AC_TRY_LINK([#include <math.h>
                   #if __GNUC__ >= 4
                   # undef isnanl
                   # define isnanl(x) __builtin_isnanl ((long double)(x))
                   #elif defined isnan
                   # undef isnanl
                   # define isnanl(x) isnan ((long double)(x))
                   #endif
                   long double x;],
                  [return isnanl (x);],
        [gl_cv_func_isnanl_in_libm=yes],
        [gl_cv_func_isnanl_in_libm=no])
      LIBS="$save_LIBS"
    ])
])

dnl Test whether isnanl() recognizes all numbers which are neither finite nor
dnl infinite. This test fails e.g. on NetBSD/i386 and on glibc/ia64.
dnl Also, the GCC >= 4.0 built-in __builtin_isnanl does not pass the tests
dnl - for pseudo-denormals on i686 and x86_64,
dnl - for pseudo-zeroes, unnormalized numbers, and pseudo-denormals on ia64.
AC_DEFUN([gl_FUNC_ISNANL_WORKS],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([gl_BIGENDIAN])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CACHE_CHECK([whether isnanl works], [gl_cv_func_isnanl_works],
    [
      AC_TRY_RUN([
#include <float.h>
#include <limits.h>
#include <math.h>
#if __GNUC__ >= 4
# undef isnanl
# define isnanl(x) __builtin_isnanl ((long double)(x))
#elif defined isnan
# undef isnanl
# define isnanl(x) isnan ((long double)(x))
#endif
#define NWORDS \
  ((sizeof (long double) + sizeof (unsigned int) - 1) / sizeof (unsigned int))
typedef union { unsigned int word[NWORDS]; long double value; }
        memory_long_double;
int main ()
{
  memory_long_double m;
  unsigned int i;

  /* gcc-3.4.3 on IRIX 6.5 appears to have a problem with this.  */
  if (!isnanl (0.0L / 0.0L))
    return 1;

  /* The isnanl function should be immune against changes in the sign bit and
     in the mantissa bits.  The xor operation twiddles a bit that can only be
     a sign bit or a mantissa bit (since the exponent never extends to
     bit 31).  */
  m.value = 0.0L / 0.0L;
  m.word[NWORDS / 2] ^= (unsigned int) 1 << (sizeof (unsigned int) * CHAR_BIT - 1);
  for (i = 0; i < NWORDS; i++)
    m.word[i] |= 1;
  if (!isnanl (m.value))
    return 1;

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
    if (!isnanl (x.value))
      return 1;
  }
  {
    /* Signalling NaN.  */
    static memory_long_double x =
      { LDBL80_WORDS (0xFFFF, 0x83333333, 0x00000000) };
    if (!isnanl (x.value))
      return 1;
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
    if (!isnanl (x.value))
      return 1;
  }
  { /* Pseudo-Infinity.  */
    static memory_long_double x =
      { LDBL80_WORDS (0xFFFF, 0x00000000, 0x00000000) };
    if (!isnanl (x.value))
      return 1;
  }
  { /* Pseudo-Zero.  */
    static memory_long_double x =
      { LDBL80_WORDS (0x4004, 0x00000000, 0x00000000) };
    if (!isnanl (x.value))
      return 1;
  }
  { /* Unnormalized number.  */
    static memory_long_double x =
      { LDBL80_WORDS (0x4000, 0x63333333, 0x00000000) };
    if (!isnanl (x.value))
      return 1;
  }
  { /* Pseudo-Denormal.  */
    static memory_long_double x =
      { LDBL80_WORDS (0x0000, 0x83333333, 0x00000000) };
    if (!isnanl (x.value))
      return 1;
  }
#endif

  return 0;
}], [gl_cv_func_isnanl_works=yes], [gl_cv_func_isnanl_works=no],
      [case "$host_cpu" in
                               # Guess no on ia64, x86_64, i386.
         ia64 | x86_64 | i*86) gl_cv_func_isnanl_works="guessing no";;
         *)
           case "$host_os" in
             netbsd*) gl_cv_func_isnanl_works="guessing no";;
             *)       gl_cv_func_isnanl_works="guessing yes";;
           esac
           ;;
       esac
      ])
    ])
])
