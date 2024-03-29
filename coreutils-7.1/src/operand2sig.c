/* operand2sig.c -- common function for parsing signal specifications
   Copyright (C) 2008 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* Extracted from kill.c/timeout.c by Pádraig Brady.
   FIXME: Move this to gnulib/str2sig.c */


/* Convert OPERAND to a signal number with printable representation SIGNAME.
   Return the signal number, or -1 if unsuccessful.  */

#include <config.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>

#if HAVE_SYS_WAIT_H
# include <sys/wait.h>
#endif
#ifndef WIFSIGNALED
# define WIFSIGNALED(s) (((s) & 0xFFFF) - 1 < (unsigned int) 0xFF)
#endif
#ifndef WTERMSIG
# define WTERMSIG(s) ((s) & 0x7F)
#endif

#include "system.h"
#include "error.h"
#include "sig2str.h"
#include "operand2sig.h"

extern int
operand2sig (char const *operand, char *signame)
{
  int signum;

  if (ISDIGIT (*operand))
    {
      char *endp;
      long int l = (errno = 0, strtol (operand, &endp, 10));
      int i = l;
      signum = (operand == endp || *endp || errno || i != l ? -1
                : WIFSIGNALED (i) ? WTERMSIG (i) : i);
    }
  else
    {
      /* Convert signal to upper case in the C locale, not in the
         current locale.  Don't assume ASCII; it might be EBCDIC.  */
      char *upcased = xstrdup (operand);
      char *p;
      for (p = upcased; *p; p++)
        if (strchr ("abcdefghijklmnopqrstuvwxyz", *p))
          *p += 'A' - 'a';

      /* Look for the signal name, possibly prefixed by "SIG",
         and possibly lowercased.  */
      if (!(str2sig (upcased, &signum) == 0
            || (upcased[0] == 'S' && upcased[1] == 'I' && upcased[2] == 'G'
                && str2sig (upcased + 3, &signum) == 0)))
        signum = -1;

      free (upcased);
    }

  if (signum < 0 || sig2str (signum, signame) != 0)
    {
      error (0, 0, _("%s: invalid signal"), operand);
      return -1;
    }

  return signum;
}
