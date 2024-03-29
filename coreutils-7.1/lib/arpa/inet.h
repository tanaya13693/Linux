/* DO NOT EDIT! GENERATED AUTOMATICALLY! */
/* -*- buffer-read-only: t -*- vi: set ro: */
/* DO NOT EDIT! GENERATED AUTOMATICALLY! */
#line 1
/* A GNU-like <arpa/inet.h>.

   Copyright (C) 2005-2006, 2008 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

#ifndef _GL_ARPA_INET_H

/* Gnulib's sys/socket.h is responsible for pulling in winsock2.h etc
   under MinGW. */
#include <sys/socket.h>

#if 1

# if __GNUC__ >= 3
#pragma GCC system_header
# endif

/* The include_next requires a split double-inclusion guard.  */
# include_next <arpa/inet.h>

#endif

#ifndef _GL_ARPA_INET_H
#define _GL_ARPA_INET_H

/* The definition of GL_LINK_WARNING is copied here.  */
/* GL_LINK_WARNING("literal string") arranges to emit the literal string as
   a linker warning on most glibc systems.
   We use a linker warning rather than a preprocessor warning, because
   #warning cannot be used inside macros.  */
#ifndef GL_LINK_WARNING
  /* This works on platforms with GNU ld and ELF object format.
     Testing __GLIBC__ is sufficient for asserting that GNU ld is in use.
     Testing __ELF__ guarantees the ELF object format.
     Testing __GNUC__ is necessary for the compound expression syntax.  */
# if defined __GLIBC__ && defined __ELF__ && defined __GNUC__
#  define GL_LINK_WARNING(message) \
     GL_LINK_WARNING1 (__FILE__, __LINE__, message)
#  define GL_LINK_WARNING1(file, line, message) \
     GL_LINK_WARNING2 (file, line, message)  /* macroexpand file and line */
#  define GL_LINK_WARNING2(file, line, message) \
     GL_LINK_WARNING3 (file ":" #line ": warning: " message)
#  define GL_LINK_WARNING3(message) \
     ({ static const char warning[sizeof (message)]		\
          __attribute__ ((__unused__,				\
                          __section__ (".gnu.warning"),		\
                          __aligned__ (1)))			\
          = message "\n";					\
        (void)0;						\
     })
# else
#  define GL_LINK_WARNING(message) ((void) 0)
# endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if 1
# if !1
/* Converts an internet address from internal format to a printable,
   presentable format.
   AF is an internet address family, such as AF_INET or AF_INET6.
   SRC points to a 'struct in_addr' (for AF_INET) or 'struct in6_addr'
   (for AF_INET6).
   DST points to a buffer having room for CNT bytes.
   The printable representation of the address (in numeric form, not
   surrounded by [...], no reverse DNS is done) is placed in DST, and
   DST is returned.  If an error occurs, the return value is NULL and
   errno is set.  If CNT bytes are not sufficient to hold the result,
   the return value is NULL and errno is set to ENOSPC.  A good value
   for CNT is 46.

   For more details, see the POSIX:2001 specification
   <http://www.opengroup.org/susv3xsh/inet_ntop.html>.  */
extern const char *inet_ntop (int af, const void *restrict src,
			      char *restrict dst, socklen_t cnt);
# endif
#elif defined GNULIB_POSIXCHECK
# undef inet_ntop
# define inet_ntop(af,src,dst,cnt) \
    (GL_LINK_WARNING ("inet_ntop is unportable - " \
                      "use gnulib module inet_ntop for portability"), \
     inet_ntop (af, src, dst, cnt))
#endif

#if 0
# if !1
extern int inet_pton (int af, const char *restrict src, void *restrict dst);
# endif
#elif defined GNULIB_POSIXCHECK
# undef inet_pton
# define inet_pton(af,src,dst) \
  (GL_LINK_WARNING ("inet_pton is unportable - " \
		    "use gnulib module inet_pton for portability"), \
   inet_pton (af, src, dst))
#endif

#ifdef __cplusplus
}
#endif

#endif /* _GL_ARPA_INET_H */
#endif /* _GL_ARPA_INET_H */
