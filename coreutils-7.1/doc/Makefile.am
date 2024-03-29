# Make coreutils documentation.				-*-Makefile-*-

# Copyright (C) 1995-1998, 2001-2009 Free Software Foundation, Inc.

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

info_TEXINFOS = coreutils.texi

EXTRA_DIST = perm.texi getdate.texi constants.texi fdl.texi

# The following is necessary if the package name is 8 characters or longer.
# If the info documentation would be split into 10 or more separate files,
# then this is necessary even if the package name is 7 characters long.
#
# Tell makeinfo to put everything in a single info file: <package>.info.
# Otherwise, it would also generate files with names like <package>.info-[123],
# and those names all map to one 14-byte name (<package>.info-) on some crufty
# old systems.
AM_MAKEINFOFLAGS = --no-split

constants.texi: $(top_srcdir)/src/tail.c $(top_srcdir)/src/shred.c
	LC_ALL=C \
	sed -n -e 's/^#define \(DEFAULT_MAX[_A-Z]*\) \(.*\)/@set \1 \2/p' \
	  $(top_srcdir)/src/tail.c > t-$@
	LC_ALL=C \
	sed -n -e 's/.*\(DEFAULT_PASSES\)[ =]* \([0-9]*\).*/@set SHRED_\1 \2/p'\
	  $(top_srcdir)/src/shred.c >> t-$@
	mv t-$@ $@

MAINTAINERCLEANFILES = constants.texi

$(DVIS): $(EXTRA_DIST)
$(INFO_DEPS): $(EXTRA_DIST)

# Extended regular expressions to match word starts and ends.
_W = (^|[^A-Za-z0-9_])
W_ = ([^A-Za-z0-9_]|$$)

syntax_checks =		\
  sc-avoid-io		\
  sc-avoid-non-zero	\
  sc-avoid-timezone	\
  sc-avoid-zeroes	\
  sc-exponent-grouping	\
  sc-lower-case-var	\
  sc-use-small-caps-NUL

.PHONY: $(syntax_checks) check-texinfo

# List words/regexps here that should not appear in the texinfo documentation.
check-texinfo: $(syntax_checks)
	@fail=0; \
	grep '@url{' $(srcdir)/*.texi && fail=1; \
	grep '\$$@"' $(srcdir)/*.texi && fail=1; \
	grep -n '[^[:punct:]]@footnote' $(srcdir)/*.texi && fail=1; \
	grep -n filename $(srcdir)/*.texi \
	    | $(EGREP) -v 'setfilename|[{]filename[}]' \
	  && fail=1; \
	$(PERL) -e 1 2> /dev/null && { $(PERL) -ne \
	  '/\bPOSIX\b/ && !/\@acronym{POSIX}/ && !/^\* / || /{posix}/ and print,exit 1' \
	  $(srcdir)/*.texi 2> /dev/null || fail=1; }; \
	$(EGREP) -i '$(_W)builtins?$(W_)' $(srcdir)/*.texi && fail=1; \
	$(EGREP) -i '$(_W)path(name)?s?$(W_)' $(srcdir)/*.texi \
	  | $(EGREP) -v 'search path|@vindex PATH$$|@env[{]PATH[}]' && fail=1; \
	exit $$fail

# Use `time zone', not `timezone'.
sc-avoid-timezone:
	@$(EGREP) timezone $(srcdir)/*.texi && exit 1 || :

# Check for insufficient exponent grouping, e.g.,
# @math{2^64} should be @math{2^{64}}.
sc-exponent-grouping:
	@$(EGREP) '\{.*\^[0-9][0-9]' $(srcdir)/*.texi && exit 1 || :

# E.g., use @sc{nul}, not NUL.
sc-use-small-caps-NUL:
	@$(EGREP) '$(_W)NUL$(W_)' $(srcdir)/*.texi && exit 1 || :

# Say I/O, not IO.
sc-avoid-io:
	@$(EGREP) '$(_W)IO$(W_)' $(srcdir)/*.texi && exit 1 || :

# I prefer nonzero over non-zero.
sc-avoid-non-zero:
	@$(EGREP) non-zero $(srcdir)/*.texi && exit 1 || :

# Use `zeros', not `zeroes' (nothing wrong with `zeroes'. just be consistent).
sc-avoid-zeroes:
	@$(EGREP) -i '$(_W)zeroes$(W_)' $(srcdir)/*.texi && exit 1 || :

# ME = $(subdir)/$(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))
ME = doc/Makefile

# The quantity inside @var{...} should not contain upper case letters.
# The leading backslash exemption is to permit in-macro uses like
# @var{\varName\} where the upper case letter is part of a parameter name.
find_upper_case_var =		\
  '/\@var{/ or next;		\
   while (/\@var{(.+?)}/g)	\
     {				\
       $$v = $$1;		\
       $$v =~ /[A-Z]/ && $$v !~ /^\\/ and (print "$$ARGV:$$.:$$_"), $$m = 1 \
     }				\
   END {$$m and (warn "$(ME): do not use upper case in \@var{...}\n"), exit 1}'
sc-lower-case-var:
	@$(PERL) -e 1 2> /dev/null && \
	  $(PERL) -lne $(find_upper_case_var) $(srcdir)/*.texi

check: check-texinfo
