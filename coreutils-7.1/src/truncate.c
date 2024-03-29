/* truncate -- truncate or extend the length of files.
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

/* Written by Pádraig Brady

   This is backwards compatible with the FreeBSD utility, but is more
   flexible wrt the size specifications and the use of long options,
   to better fit the "GNU" environment.

   Note if !defined(HAVE_FTRUNCATE) then the --skip-ftruncate configure flag
   was specified or we're in a mingw environment. In these cases gnulib
   emulation will be used and GNULIB_FTRUNCATE is defined. Note if emulation
   can't even be provided ftruncate() will return EIO.  */

#include <config.h>             /* sets _FILE_OFFSET_BITS=64 etc. */
#include <stdio.h>
#include <getopt.h>
#include <sys/types.h>

#include "system.h"
#include "error.h"
#include "posixver.h"
#include "quote.h"
#include "xstrtol.h"

/* The official name of this program (e.g., no `g' prefix).  */
#define PROGRAM_NAME "truncate"

#define AUTHORS proper_name_utf8 ("Padraig Brady", "P\303\241draig Brady")

/* (-c) If true, don't create if not already there */
static bool no_create;

/* (-o) If true, --size refers to blocks not bytes */
static bool block_mode;

/* (-r) Reference file to use size from */
static char const *ref_file;

static struct option const longopts[] =
{
  {"no-create", no_argument, NULL, 'c'},
  {"io-blocks", no_argument, NULL, 'o'},
  {"reference", required_argument, NULL, 'r'},
  {"size", required_argument, NULL, 's'},
  {GETOPT_HELP_OPTION_DECL},
  {GETOPT_VERSION_OPTION_DECL},
  {NULL, 0, NULL, 0}
};

typedef enum
{ rm_abs = 0, rm_rel, rm_min, rm_max, rm_rdn, rm_rup } rel_mode_t;

/* Set size to the value of STR, interpreted as a decimal integer,
   optionally multiplied by various values.
   Return -1 on error, 0 on success.

   This supports dd BLOCK size suffixes + lowercase g,t,m for bsd compat
   Note we don't support dd's b=512, c=1, w=2 or 21x512MiB formats.  */
static int
parse_len (char const *str, off_t *size)
{
  enum strtol_error e;
  intmax_t tmp_size;
  e = xstrtoimax (str, NULL, 10, &tmp_size, "EgGkKmMPtTYZ0");
  if (e == LONGINT_OK
      && !(OFF_T_MIN <= tmp_size && tmp_size <= OFF_T_MAX))
    e = LONGINT_OVERFLOW;

  if (e == LONGINT_OK)
    {
      errno = 0;
      *size = tmp_size;
      return 0;
    }

  errno = (e == LONGINT_OVERFLOW ? EOVERFLOW : 0);
  return -1;
}

void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
             program_name);
  else
    {
      printf (_("Usage: %s OPTION... FILE...\n"), program_name);
      fputs (_("\
Shrink or extend the size of each FILE to the specified size\n\
\n\
A FILE argument that does not exist is created.\n\
\n\
If a FILE is larger than the specified size, the extra data is lost.\n\
If a FILE is shorter, it is extended and the extended part (hole)\n\
reads as zero bytes.\n\
\n\
"), stdout);
      fputs (_("\
Mandatory arguments to long options are mandatory for short options too.\n\
"), stdout);
      fputs (_("\
  -c, --no-create        do not create any files\n\
"), stdout);
      fputs (_("\
  -o, --io-blocks        Treat SIZE as number of IO blocks instead of bytes\n\
"), stdout);
      fputs (_("\
  -r, --reference=FILE   use this FILE's size\n\
  -s, --size=SIZE        use this SIZE\n"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      fputs (_("\n\
SIZE is a number which may be followed by one of the following suffixes:\n\
KB 1000, K 1024, MB 1000*1000, M 1024*1024, and so on for G, T, P, E, Z, Y.\n\
"), stdout);
      fputs (_("\n\
SIZE may also be prefixed by one of the following modifying characters:\n\
`+' extend by, `-' reduce by, `<' at most, `>' at least,\n\
`/' round down to multiple of, `%' round up to multiple of.\n"), stdout);
      fputs (_("\
\n\
Note that the -r and -s options are mutually exclusive.\n\
"), stdout);
      emit_bug_reporting_address ();
    }
  exit (status);
}

/* return 1 on error, 0 on success */
static int
do_ftruncate (int fd, char const *fname, off_t ssize, rel_mode_t rel_mode)
{
  struct stat sb;
  off_t nsize;

  if ((block_mode || rel_mode) && fstat (fd, &sb) != 0)
    {
      error (0, errno, _("cannot fstat %s"), quote (fname));
      return 1;
    }
  if (block_mode)
    {
      off_t const blksize = ST_BLKSIZE (sb);
      if (ssize < OFF_T_MIN / blksize || ssize > OFF_T_MAX / blksize)
        {
          error (0, 0,
                 _("overflow in %" PRIdMAX
                   " * %" PRIdMAX " byte blocks for file %s"),
                 (intmax_t) ssize, (intmax_t) blksize,
                 quote (fname));
          return 1;
        }
      ssize *= blksize;
    }
  if (rel_mode)
    {
      uintmax_t const fsize = sb.st_size;

      if (sb.st_size < 0)
        {
          /* Complain only for a regular file, a directory,
             or a shared memory object, as POSIX 1003.1-2004 specifies
             ftruncate's behavior only for these file types.  */
          if (S_ISREG (sb.st_mode) || S_ISDIR (sb.st_mode)
              || S_TYPEISSHM (&sb))
            {
              /* overflow is the only reason I can think
                 this would ever go negative for the above types */
              error (0, 0, _("%s has unusable, apparently negative size"),
                     quote (fname));
              return 1;
            }
          return 0;
        }

      if (rel_mode == rm_min)
        nsize = MAX (fsize, (uintmax_t) ssize);
      else if (rel_mode == rm_max)
        nsize = MIN (fsize, (uintmax_t) ssize);
      else if (rel_mode == rm_rdn)
        /* 0..ssize-1 -> 0 */
        nsize = (fsize / ssize) * ssize;
      else if (rel_mode == rm_rup)
        /* 1..ssize -> ssize */
        {
          /* Here ssize>=1 && fsize>=0 */
          uintmax_t const overflow = ((fsize + ssize - 1) / ssize) * ssize;
          if (overflow > OFF_T_MAX)
            {
              error (0, 0, _("overflow rounding up size of file %s"),
                     quote (fname));
              return 1;
            }
          nsize = overflow;
        }
      else
        {
          if (ssize > OFF_T_MAX - (off_t)fsize)
            {
              error (0, 0, _("overflow extending size of file %s"),
                     quote (fname));
              return 1;
            }
          nsize = fsize + ssize;
        }
    }
  else
    nsize = ssize;
  if (nsize < 0)
    nsize = 0;

  if (ftruncate (fd, nsize) == -1)      /* note updates mtime & ctime */
    {
      /* Complain only when ftruncate fails on a regular file, a
         directory, or a shared memory object, as POSIX 1003.1-2004
         specifies ftruncate's behavior only for these file types.
         For example, do not complain when Linux 2.4 ftruncate
         fails on /dev/fd0.  */
      int const ftruncate_errno = errno;
      if (fstat (fd, &sb) != 0)
        {
          error (0, errno, _("cannot fstat %s"), quote (fname));
          return 1;
        }
      else if (S_ISREG (sb.st_mode) || S_ISDIR (sb.st_mode)
               || S_TYPEISSHM (&sb))
        {
          error (0, ftruncate_errno,
                 _("truncating %s at %" PRIdMAX " bytes"), quote (fname),
                 (intmax_t) nsize);
          return 1;
        }
      return 0;
    }

  return 0;
}

int
main (int argc, char **argv)
{
  bool got_size = false;
  off_t size IF_LINT (= 0);
  rel_mode_t rel_mode = rm_abs;
  mode_t omode;
  int c, errors = 0, fd = -1, oflags;
  char const *fname;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  while ((c = getopt_long (argc, argv, "cor:s:", longopts, NULL)) != -1)
    {
      switch (c)
        {
        case 'c':
          no_create = true;
          break;

        case 'o':
          block_mode = true;
          break;

        case 'r':
          ref_file = optarg;
          break;

        case 's':
          /* skip any whitespace */
          while (isspace (*optarg))
            optarg++;
          switch (*optarg)
            {
            case '<':
              rel_mode = rm_max;
              optarg++;
              break;
            case '>':
              rel_mode = rm_min;
              optarg++;
              break;
            case '/':
              rel_mode = rm_rdn;
              optarg++;
              break;
            case '%':
              rel_mode = rm_rup;
              optarg++;
              break;
            }
          /* skip any whitespace */
          while (isspace (*optarg))
            optarg++;
          if (*optarg == '+' || *optarg == '-')
            {
              if (rel_mode)
                {
                  error (0, 0, _("multiple relative modifiers specified"));
                  /* Note other combinations are flagged as invalid numbers */
                  usage (EXIT_FAILURE);
                }
              rel_mode = rm_rel;
            }
          if (parse_len (optarg, &size) == -1)
            error (EXIT_FAILURE, errno, _("invalid number %s"),
                   quote (optarg));
          /* Rounding to multiple of 0 is nonsensical */
          if ((rel_mode == rm_rup || rel_mode == rm_rdn) && size == 0)
            error (EXIT_FAILURE, 0, _("division by zero"));
          got_size = true;
          break;

        case_GETOPT_HELP_CHAR;

        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);

        default:
          usage (EXIT_FAILURE);
        }
    }

  argv += optind;
  argc -= optind;

  /* must specify either size or reference file */
  if ((ref_file && got_size) || (!ref_file && !got_size))
    {
      error (0, 0, _("you must specify one of %s or %s"),
             quote_n (0, "--size"), quote_n (1, "--reference"));
      usage (EXIT_FAILURE);
    }
  /* block_mode without size is not valid */
  if (block_mode && !got_size)
    {
      error (0, 0, _("%s was specified but %s was not"),
             quote_n (0, "--io-blocks"), quote_n (1, "--size"));
      usage (EXIT_FAILURE);
    }
  /* must specify at least 1 file */
  if (argc < 1)
    {
      error (0, 0, _("missing file operand"));
      usage (EXIT_FAILURE);
    }

  if (ref_file)
    {
      struct stat sb;
      if (stat (ref_file, &sb) != 0)
        error (EXIT_FAILURE, errno, _("cannot stat %s"), quote (ref_file));
      size = sb.st_size;
    }

  oflags = O_WRONLY | (no_create ? 0 : O_CREAT) | O_NONBLOCK;
  omode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

  while ((fname = *argv++) != NULL)
    {
      if ((fd = open (fname, oflags, omode)) == -1)
        {
          /* `truncate -s0 -c no-such-file`  shouldn't gen error
             `truncate -s0 no-such-dir/file` should gen ENOENT error
             `truncate -s0 no-such-dir/` should gen EISDIR error
             `truncate -s0 .` should gen EISDIR error */
          if (!(no_create && errno == ENOENT))
            {
              int const open_errno = errno;
              struct stat sb;
              if (stat (fname, &sb) == 0)
                {
                  /* Complain only for a regular file, a directory,
                     or a shared memory object, as POSIX 1003.1-2004 specifies
                     ftruncate's behavior only for these file types.  */
                  if (!S_ISREG (sb.st_mode) && !S_ISDIR (sb.st_mode)
                      && !S_TYPEISSHM (&sb))
                    continue;
                }
              error (0, open_errno, _("cannot open %s for writing"),
                     quote (fname));
              errors++;
            }
          continue;
        }


      if (fd != -1)
        {
          errors += do_ftruncate (fd, fname, size, rel_mode);
          if (close (fd) != 0)
            {
              error (0, errno, _("closing %s"), quote (fname));
              errors++;
            }
        }
    }

  return errors ? EXIT_FAILURE : EXIT_SUCCESS;
}

/*
 * Local variables:
 *  indent-tabs-mode: nil
 * End:
 */
