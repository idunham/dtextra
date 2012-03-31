dnl aclocal.m4 generated automatically by aclocal 1.3

dnl Copyright (C) 1994, 1995, 1996, 1997, 1998 Free Software Foundation, Inc.
dnl This Makefile.in is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY, to the extent permitted by law; without
dnl even the implied warranty of MERCHANTABILITY or FITNESS FOR A
dnl PARTICULAR PURPOSE.

dnl @(#)$Mu: mgv/acinclude.m4 1.5 1998/08/25 21:34:32 $
dnl
dnl acinclude.m4
dnl	local automake/autoconf/m4 fun for mgv.
dnl
dnl Copyright (C) 1998 Eric A. Howe
dnl
dnl This program is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 2 of the License, or
dnl (at your option) any later version.
dnl
dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl
dnl You should have received a copy of the GNU General Public License
dnl along with this program; if not, write to the Free Software
dnl Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
dnl
dnl   Authors:	Eric A. Howe (mu@trends.net)
dnl

dnl
dnl ICE_FIND_XPM
dnl ---------------
dnl
dnl Find Xpm libraries and headers.
dnl Put Xpm include directory in xpm_includes,
dnl put Xpm library directory in xpm_libraries,
dnl and add appropriate flags to X_CFLAGS and X_LIBS.
dnl
dnl Borrowed from DDD.
dnl
AC_DEFUN(ICE_FIND_XPM,
[
AC_REQUIRE([AC_PATH_XTRA])
xpm_includes=
xpm_libraries=
AC_ARG_WITH(xpm,
[  --without-xpm                    do not use the Xpm library])
dnl Treat --without-xpm like
dnl --without-xpm-includes --without-xpm-libraries.
if test "$with_xpm" = "no"
then
xpm_includes=no
xpm_libraries=no
fi
AC_ARG_WITH(xpm-includes,
[  --with-xpm-includes=DIR          Xpm include files are in DIR],
xpm_includes="$withval")
AC_ARG_WITH(xpm-libraries,
[  --with-xpm-libraries=DIR         Xpm libraries are in DIR],
xpm_libraries="$withval")
AC_MSG_CHECKING(for Xpm)
#
#
# Search the include files.  Note that XPM can come in <X11/xpm.h> (as
# in X11R6) or in <xpm.h> if installed locally.
#
if test "$xpm_includes" = ""; then
AC_CACHE_VAL(ice_cv_xpm_includes,
[
ice_xpm_save_LIBS="$LIBS"
ice_xpm_save_CFLAGS="$CFLAGS"
ice_xpm_save_CPPFLAGS="$CPPFLAGS"
ice_xpm_save_LDFLAGS="$LDFLAGS"
#
LIBS="$X_PRE_LIBS -lXpm -lXt -lX11 $X_EXTRA_LIBS $LIBS"
CFLAGS="$X_CFLAGS $CFLAGS"
CPPFLAGS="$X_CFLAGS $CPPFLAGS"
LDFLAGS="$X_LIBS $LDFLAGS"
#
AC_TRY_COMPILE([
#include <X11/xpm.h>
],[int a;],
[
# X11/xpm.h is in the standard search path.
ice_cv_xpm_includes=
],
[
# X11/xpm.h is not in the standard search path.
# Locate it and put its directory in `xpm_includes'
#
# /usr/include/Motif* are used on HP-UX (Motif).
# /usr/include/X11* are used on HP-UX (X and Xaw).
# /usr/dt is used on Solaris (Motif).
# /usr/openwin is used on Solaris (X and Xaw).
# Other directories are just guesses.
for dir in "$x_includes" "${prefix}/include" /usr/include /usr/local/include \
           /usr/include/Motif2.0 /usr/include/Motif1.2 /usr/include/Motif1.1 \
           /usr/include/X11R6 /usr/include/X11R5 /usr/include/X11R4 \
           /usr/dt/include /usr/openwin/include \
           /usr/dt/*/include /opt/*/include /usr/include/Motif* \
	   "${prefix}"/*/include /usr/*/include /usr/local/*/include \
	   "${prefix}"/include/* /usr/include/* /usr/local/include/*; do
if test -f "$dir/X11/xpm.h" || test -f "$dir/xpm.h"; then
ice_cv_xpm_includes="$dir"
break
fi
done
])
#
LIBS="$ice_xpm_save_LIBS"
CFLAGS="$ice_xpm_save_CFLAGS"
CPPFLAGS="$ice_xpm_save_CPPFLAGS"
LDFLAGS="$ice_xpm_save_LDFLAGS"
])
xpm_includes="$ice_cv_xpm_includes"
fi
#
#
# Now for the libraries.
#
if test "$xpm_libraries" = ""; then
AC_CACHE_VAL(ice_cv_xpm_libraries,
[
ice_xpm_save_LIBS="$LIBS"
ice_xpm_save_CFLAGS="$CFLAGS"
ice_xpm_save_CPPFLAGS="$CPPFLAGS"
ice_xpm_save_LDFLAGS="$LDFLAGS"
#
LIBS="$X_PRE_LIBS -lXpm -lXt -lX11 $X_EXTRA_LIBS $LIBS"
CFLAGS="$X_CFLAGS $CFLAGS"
CPPFLAGS="$X_CFLAGS $CPPFLAGS"
LDFLAGS="$X_LIBS $LDFLAGS"
#
#
# We use XtToolkitInitialize() here since it takes no arguments
# and thus also works with a C++ compiler.
AC_TRY_LINK([
#include <X11/Intrinsic.h>
#include <X11/xpm.h>
],[XtToolkitInitialize();],
[
# libxpm.a is in the standard search path.
ice_cv_xpm_libraries=
],
[
# libXpm.a is not in the standard search path.
# Locate it and put its directory in `xpm_libraries'
#
#
# /usr/lib/Motif* are used on HP-UX (Motif).
# /usr/lib/X11* are used on HP-UX (X and Xpm).
# /usr/dt is used on Solaris (Motif).
# /usr/openwin is used on Solaris (X and Xpm).
# Other directories are just guesses.
for dir in "$x_libraries" "${prefix}/lib" /usr/lib /usr/local/lib \
	   /usr/lib/Motif2.0 /usr/lib/Motif1.2 /usr/lib/Motif1.1 \
	   /usr/lib/X11R6 /usr/lib/X11R5 /usr/lib/X11R4 /usr/lib/X11 \
           /usr/dt/lib /usr/openwin/lib \
	   /usr/dt/*/lib /opt/*/lib /usr/lib/Motif* \
	   "${prefix}"/*/lib /usr/*/lib /usr/local/*/lib \
	   "${prefix}"/lib/* /usr/lib/* /usr/local/lib/*; do
if test -d "$dir" && test "`ls $dir/libXpm.* 2> /dev/null`" != ""; then
ice_cv_xpm_libraries="$dir"
break
fi
done
])
#
LIBS="$ice_xpm_save_LIBS"
CFLAGS="$ice_xpm_save_CFLAGS"
CPPFLAGS="$ice_xpm_save_CPPFLAGS"
LDFLAGS="$ice_xpm_save_LDFLAGS"
])
#
xpm_libraries="$ice_cv_xpm_libraries"
fi
#
# Add Xpm definitions to X flags
#
if test "$xpm_includes" != "" && test "$xpm_includes" != "$x_includes" && test "$xpm_includes" != "no"
then
X_CFLAGS="-I$xpm_includes $X_CFLAGS"
fi
if test "$xpm_libraries" != "" && test "$xpm_libraries" != "$x_libraries" && test "$xpm_libraries" != "no"
then
case "$X_LIBS" in
  *-R\ *) X_LIBS="-L$xpm_libraries -R $xpm_libraries $X_LIBS";;
  *-R*)   X_LIBS="-L$xpm_libraries -R$xpm_libraries $X_LIBS";;
  *)      X_LIBS="-L$xpm_libraries $X_LIBS";;
esac
fi
#
#
xpm_libraries_result="$xpm_libraries"
xpm_includes_result="$xpm_includes"
test "$xpm_libraries_result" = "" && 
  xpm_libraries_result="in default path"
test "$xpm_includes_result" = "" && 
  xpm_includes_result="in default path"
test "$xpm_libraries_result" = "no" && 
  xpm_libraries_result="(none)"
test "$xpm_includes_result" = "no" && 
  xpm_includes_result="(none)"
AC_MSG_RESULT(
  [libraries $xpm_libraries_result, headers $xpm_includes_result])
])dnl

dnl
dnl ICE_FIND_MOTIF
dnl --------------
dnl
dnl Find Motif libraries and headers
dnl Put Motif include directory in motif_includes,
dnl put Motif library directory in motif_libraries,
dnl and add appropriate flags to X_CFLAGS and X_LIBS.
dnl
dnl Borrowed from DDD.
dnl
AC_DEFUN(ICE_FIND_MOTIF,
[
AC_REQUIRE([AC_PATH_XTRA])
motif_includes=
motif_libraries=
AC_ARG_WITH(motif-includes,
[  --with-motif-includes=DIR        Motif include files are in DIR],
motif_includes="$withval")
AC_ARG_WITH(motif-libraries,
[  --with-motif-libraries=DIR       Motif libraries are in DIR],
motif_libraries="$withval")
AC_MSG_CHECKING(for Motif)
#
#
# Search the include files.
#
if test "$motif_includes" = ""; then
AC_CACHE_VAL(ice_cv_motif_includes,
[
ice_motif_save_LIBS="$LIBS"
ice_motif_save_CFLAGS="$CFLAGS"
ice_motif_save_CPPFLAGS="$CPPFLAGS"
ice_motif_save_LDFLAGS="$LDFLAGS"
#
LIBS="$X_PRE_LIBS -lXm -lXt -lX11 $X_EXTRA_LIBS $LIBS"
CFLAGS="$X_CFLAGS $CFLAGS"
CPPFLAGS="$X_CFLAGS $CPPFLAGS"
LDFLAGS="$X_LIBS $LDFLAGS"
#
AC_TRY_COMPILE([#include <Xm/Xm.h>],[int a;],
[
# Xm/Xm.h is in the standard search path.
ice_cv_motif_includes=
],
[
# Xm/Xm.h is not in the standard search path.
# Locate it and put its directory in `motif_includes'
#
# /usr/include/Motif* are used on HP-UX (Motif).
# /usr/include/X11* are used on HP-UX (X and Athena).
# /usr/dt is used on Solaris (Motif).
# /usr/openwin is used on Solaris (X and Athena).
# Other directories are just guesses.
for dir in "$x_includes" "${prefix}/include" /usr/include /usr/local/include \
           /usr/include/Motif2.0 /usr/include/Motif1.2 /usr/include/Motif1.1 \
           /usr/include/X11R6 /usr/include/X11R5 /usr/include/X11R4 \
           /usr/dt/include /usr/openwin/include \
           /usr/dt/*/include /opt/*/include /usr/include/Motif* \
	   "${prefix}"/*/include /usr/*/include /usr/local/*/include \
	   "${prefix}"/include/* /usr/include/* /usr/local/include/*; do
if test -f "$dir/Xm/Xm.h"; then
ice_cv_motif_includes="$dir"
break
fi
done
if test "$ice_cv_motif_includes" = ""; then
ice_cv_motif_includes=no
fi
])
#
LIBS="$ice_motif_save_LIBS"
CFLAGS="$ice_motif_save_CFLAGS"
CPPFLAGS="$ice_motif_save_CPPFLAGS"
LDFLAGS="$ice_motif_save_LDFLAGS"
])
motif_includes="$ice_cv_motif_includes"
fi
#
#
# Now for the libraries.
#
if test "$motif_libraries" = ""; then
AC_CACHE_VAL(ice_cv_motif_libraries,
[
ice_motif_save_LIBS="$LIBS"
ice_motif_save_CFLAGS="$CFLAGS"
ice_motif_save_CPPFLAGS="$CPPFLAGS"
ice_motif_save_LDFLAGS="$LDFLAGS"
#
LIBS="$X_PRE_LIBS -lXm -lXt -lX11 $X_EXTRA_LIBS $LIBS"
CFLAGS="$X_CFLAGS $CFLAGS"
CPPFLAGS="$X_CFLAGS $CPPFLAGS"
LDFLAGS="$X_LIBS $LDFLAGS"
#
# We use XtToolkitInitialize() here since it takes no arguments
# and thus also works with a C++ compiler.
AC_TRY_LINK([
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
],[XtToolkitInitialize();],
[
# libXm.a is in the standard search path.
ice_cv_motif_libraries=
],
[
# libXm.a is not in the standard search path.
# Locate it and put its directory in `motif_libraries'
#
# /usr/lib/Motif* are used on HP-UX (Motif).
# /usr/lib/X11* are used on HP-UX (X and Athena).
# /usr/dt is used on Solaris (Motif).
# /usr/lesstif is used on Linux (Lesstif).
# /usr/openwin is used on Solaris (X and Athena).
# Other directories are just guesses.
for dir in "$x_libraries" "${prefix}/lib" /usr/lib /usr/local/lib \
	   /usr/lib/Motif2.0 /usr/lib/Motif1.2 /usr/lib/Motif1.1 \
	   /usr/lib/X11R6 /usr/lib/X11R5 /usr/lib/X11R4 /usr/lib/X11 \
           /usr/dt/lib /usr/openwin/lib \
	   /usr/dt/*/lib /opt/*/lib /usr/lib/Motif* \
           /usr/lesstif*/lib /usr/lib/Lesstif* \
	   "${prefix}"/*/lib /usr/*/lib /usr/local/*/lib \
	   "${prefix}"/lib/* /usr/lib/* /usr/local/lib/*; do
if test -d "$dir" && test "`ls $dir/libXm.* 2> /dev/null`" != ""; then
ice_cv_motif_libraries="$dir"
break
fi
done
if test "$ice_cv_motif_libraries" = ""; then
ice_cv_motif_libraries=no
fi
])
#
LIBS="$ice_motif_save_LIBS"
CFLAGS="$ice_motif_save_CFLAGS"
CPPFLAGS="$ice_motif_save_CPPFLAGS"
LDFLAGS="$ice_motif_save_LDFLAGS"
])
#
motif_libraries="$ice_cv_motif_libraries"
fi
# Add Motif definitions to X flags
#
if test "$motif_includes" != "" && test "$motif_includes" != "$x_includes" && test "$motif_includes" != "no"
then
X_CFLAGS="-I$motif_includes $X_CFLAGS"
fi
if test "$motif_libraries" != "" && test "$motif_libraries" != "$x_libraries" && test "$motif_libraries" != "no"
then
case "$X_LIBS" in
  *-R\ *) X_LIBS="-L$motif_libraries -R $motif_libraries $X_LIBS";;
  *-R*)   X_LIBS="-L$motif_libraries -R$motif_libraries $X_LIBS";;
  *)      X_LIBS="-L$motif_libraries $X_LIBS";;
esac
fi
#
#
motif_libraries_result="$motif_libraries"
motif_includes_result="$motif_includes"
test "$motif_libraries_result" = "" && 
  motif_libraries_result="in default path"
test "$motif_includes_result" = "" && 
  motif_includes_result="in default path"
test "$motif_libraries_result" = "no" && 
  motif_libraries_result="(none)"
test "$motif_includes_result" = "no" && 
  motif_includes_result="(none)"
AC_MSG_RESULT(
  [libraries $motif_libraries_result, headers $motif_includes_result])
])dnl

dnl
dnl MGV_WITH_DMALLOC
dnl ---------------
dnl
dnl Check for dmalloc requests and such.
dnl
dnl Borrowed from LessTif.
dnl
AC_DEFUN(MGV_WITH_DMALLOC, [
AC_MSG_CHECKING(if malloc debugging is wanted)
AC_ARG_WITH(dmalloc,
[  --with-dmalloc          use dmalloc, as in
                          ftp://ftp.letters.com/src/dmalloc/dmalloc.tar.gz],
[if test "$withval" = yes; then
	AC_MSG_RESULT(yes)
	AC_DEFINE(USE_DMALLOC, 1)
	LIBS="$LIBS -ldmalloc"
else
	AC_DEFINE(USE_DMALLOC, 0)
	AC_MSG_RESULT(no)
fi
], [
	AC_DEFINE(USE_DMALLOC, 0)
	AC_MSG_RESULT(no)
])])

# Do all the work for Automake.  This macro actually does too much --
# some checks are only needed if your package does certain things.
# But this isn't really a big deal.

# serial 1

dnl Usage:
dnl AM_INIT_AUTOMAKE(package,version, [no-define])

AC_DEFUN(AM_INIT_AUTOMAKE,
[AC_REQUIRE([AM_PROG_INSTALL])
PACKAGE=[$1]
AC_SUBST(PACKAGE)
VERSION=[$2]
AC_SUBST(VERSION)
dnl test to see if srcdir already configured
if test "`cd $srcdir && pwd`" != "`pwd`" && test -f $srcdir/config.status; then
  AC_MSG_ERROR([source directory already configured; run "make distclean" there first])
fi
ifelse([$3],,
AC_DEFINE_UNQUOTED(PACKAGE, "$PACKAGE")
AC_DEFINE_UNQUOTED(VERSION, "$VERSION"))
AC_REQUIRE([AM_SANITY_CHECK])
AC_REQUIRE([AC_ARG_PROGRAM])
dnl FIXME This is truly gross.
missing_dir=`cd $ac_aux_dir && pwd`
AM_MISSING_PROG(ACLOCAL, aclocal, $missing_dir)
AM_MISSING_PROG(AUTOCONF, autoconf, $missing_dir)
AM_MISSING_PROG(AUTOMAKE, automake, $missing_dir)
AM_MISSING_PROG(AUTOHEADER, autoheader, $missing_dir)
AM_MISSING_PROG(MAKEINFO, makeinfo, $missing_dir)
AC_REQUIRE([AC_PROG_MAKE_SET])])


# serial 1

AC_DEFUN(AM_PROG_INSTALL,
[AC_REQUIRE([AC_PROG_INSTALL])
test -z "$INSTALL_SCRIPT" && INSTALL_SCRIPT='${INSTALL_PROGRAM}'
AC_SUBST(INSTALL_SCRIPT)dnl
])

#
# Check to make sure that the build environment is sane.
#

AC_DEFUN(AM_SANITY_CHECK,
[AC_MSG_CHECKING([whether build environment is sane])
# Just in case
sleep 1
echo timestamp > conftestfile
# Do `set' in a subshell so we don't clobber the current shell's
# arguments.  Must try -L first in case configure is actually a
# symlink; some systems play weird games with the mod time of symlinks
# (eg FreeBSD returns the mod time of the symlink's containing
# directory).
if (
   set X `ls -Lt $srcdir/configure conftestfile 2> /dev/null`
   if test "[$]*" = "X"; then
      # -L didn't work.
      set X `ls -t $srcdir/configure conftestfile`
   fi
   if test "[$]*" != "X $srcdir/configure conftestfile" \
      && test "[$]*" != "X conftestfile $srcdir/configure"; then

      # If neither matched, then we have a broken ls.  This can happen
      # if, for instance, CONFIG_SHELL is bash and it inherits a
      # broken ls alias from the environment.  This has actually
      # happened.  Such a system could not be considered "sane".
      AC_MSG_ERROR([ls -t appears to fail.  Make sure there is not a broken
alias in your environment])
   fi

   test "[$]2" = conftestfile
   )
then
   # Ok.
   :
else
   AC_MSG_ERROR([newly created file is older than distributed files!
Check your system clock])
fi
rm -f conftest*
AC_MSG_RESULT(yes)])

dnl AM_MISSING_PROG(NAME, PROGRAM, DIRECTORY)
dnl The program must properly implement --version.
AC_DEFUN(AM_MISSING_PROG,
[AC_MSG_CHECKING(for working $2)
# Run test in a subshell; some versions of sh will print an error if
# an executable is not found, even if stderr is redirected.
# Redirect stdin to placate older versions of autoconf.  Sigh.
if ($2 --version) < /dev/null > /dev/null 2>&1; then
   $1=$2
   AC_MSG_RESULT(found)
else
   $1="$3/missing $2"
   AC_MSG_RESULT(missing)
fi
AC_SUBST($1)])

# Define a conditional.

AC_DEFUN(AM_CONDITIONAL,
[AC_SUBST($1_TRUE)
AC_SUBST($1_FALSE)
if $2; then
  $1_TRUE=
  $1_FALSE='#'
else
  $1_TRUE='#'
  $1_FALSE=
fi])

