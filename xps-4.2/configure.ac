dnl $Id: configure.ac,v 1.8 2003/04/24 00:21:29 rockyb Exp $
dnl Process this file with autoconf to produce a configure script.

# Like AC_CHECK_HEADER, but it uses the already-computed -I directories.
#
AC_DEFUN(AC_CHECK_X_HEADER, [
  ac_save_CPPFLAGS="$CPPFLAGS"
  if test \! -z "$includedir" ; then 
    CPPFLAGS="$CPPFLAGS -I$includedir"
  fi
  CPPFLAGS="$CPPFLAGS $X_CFLAGS"
  AC_CHECK_HEADER([$1], [$2], [$3], [$4])
  CPPFLAGS="$ac_save_CPPFLAGS"])

dnl ========================================================================
dnl What file is guarenteed to be in the source directory where 
dnl the Makefile should be run? 
dnl ========================================================================
define(XPS_VERSION, 4.2)
AC_INIT(configure.ac)
AM_INIT_AUTOMAKE(xps,XPS_VERSION)

AM_CONFIG_HEADER(src/config.h)

AC_CANONICAL_HOST

PROGRAM=xps
AC_SUBST(PROGRAM, $PROGRAM)dnl

dnl ========================================================================
dnl Try to guess a decent prefix. Perhaps this program is already installed;
dnl first that. Then a locally added program, and then a 
dnl standard X11 program. DO NOT use . as a valid prefix.
dnl ========================================================================
old_prefix=$prefix
AC_PREFIX_PROGRAM($PROGRAM)
if test "x$prefix" = x.; then 
  prefix=$old_prefix
  unset ac_cv_path_ac_prefix_program
fi
if test "x$prefix" = xNONE; then 
  AC_PREFIX_PROGRAM(xscreensaver)
fi
if test "x$prefix" = xNONE; then 
  AC_PREFIX_PROGRAM(xterm)
fi

dnl ========================================================================
dnl Allow option specifying whether to enable/disable suid as root if needed.
dnl ========================================================================
AC_ARG_ENABLE(suid,
[  --enable suid           install suid root if needed to see ps of all procs],
	      ,enable_suid=yes)

dnl ========================================================================
dnl Allow option for specifying where the Motif libraries are located
dnl ========================================================================
AC_ARG_WITH(motif-libs,
[  --with-motif-libs=library  find motif library in library],
	      X_LIBS="$X_LIBS -L$with_motif_libs -R$with_motif_libs")

VERSION_CURRENT=XPS_VERSION
AC_SUBST(VERSION_CURRENT, $VERSION_CURRENT)dnl

AC_PREREQ(2.10)dnl               dnl Minimum Autoconf version required.

dnl =============================
dnl Checks for programs.
dnl =============================
AC_PROG_CC
AC_AIX
AC_PROG_INSTALL
AC_PATH_PROG(RPMBUILD, rpmbuild, /bin/true)
AC_PROG_LN_S
AC_PROG_MAKE_SET

dnl =============================
dnl Checks for headers.
dnl =============================
AC_HEADER_DIRENT
AC_HEADER_STDC

AC_CHECK_HEADERS([errno.h fcntl.h limits.h malloc.h nlist.h pwd.h signal.h \
		  stdlib.h string.h sys/param.h regex.h sys/resource.h \
		  sys/time.h sys/vfs.h unistd.h usersec.h])

AC_SUBST(XINCLUDES)
AC_CACHE_CHECK(for X11 includes,XINCLUDES,
[# Look for the header file in a standard set of common directories.
# Check X11 before X11Rn because it is often a symlink to the current release.
  for ac_dir in $x_includes     \
    /usr/dt/share/include       \
    /usr/openwin/share/include  \
    /usr/X11R6/include          \
    ; \
  do
    if test -d "$ac_dir"
    then
        XINCLUDES="$XINCLUDES -I$ac_dir"
    fi

  done])

CPPFLAGS="$CPPFLAGS $XINCLUDES"

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_UID_T

dnl ========================================================================
dnl Solaris before 2.6 needs to be installed as setuid root to 
dnl get information about other processes, where as at or after 2.6 we can
dnl use alternate code. We may need to have to define __STDC__ 0 if
dnl we have an old egcs or gcc compiler.
dnl ========================================================================
INSTALL_TARGET=install-nosuid
case "$host_os" in
  solaris2.6 | solaris2.7 | solaris2.8 ) 
    AC_DEFINE(SOLARIS_NEW_PROC, 1, 
    [Define if you are running Solaris not before 2.6 and 
    want to try the non-setuid code.])

    AC_CHECKING([whether you need __STDC__ set to 0. 
If so, you probably need a newer version of the gcc or egcs compiler.])
    AC_TRY_COMPILE([#include <procfs.h>],[],try_stdc=no,try_stdc=yes)
    if test $try_stdc = yes; then 
      AC_TRY_COMPILE(,[#define __STDC__ 0
#include <procfs.h>],AC_DEFINE(NEED_STDC_DEFINE, 1,
	[Define if you need __STDC__ defined to 0 in order to get certain
   includes to compile compile correctly. In particular Solaris 2.6
   with older gcc or egcs.]),
	AC_DEFINE(NEED_SUID_ROOT, 1, 
	[Defined if your OS, OS release and how we've coded the program
	need to have this program run as root. However if this is defined
	that doesn't mean it *will* installed suid as root. That's 
	decision is also subject to user discretion too.]) 
	INSTALL_TARGET=install-suid )
	AC_MSG_RESULT([Consider upgrading your compiler.])
    fi
    ;;
  solaris* | sunos5* ) 
    if test "$enable_suid" = yes; then
      AC_MSG_RESULT(This program needs to be run as suid to get information
about processes other than the one you own)
      INSTALL_TARGET=install-suid; AC_DEFINE(NEED_SUID_ROOT)
    fi
     ;;
  linux-gnu )
    AC_CHECK_HEADERS(linux/tasks.h)
    ;;
  netbsd* | bsdi* )
    INSTALL_TARGET=install-sgid;
    ;;
  * ) 
    ;;
esac
AC_SUBST(INSTALL_TARGET)
	
dnl
dnl Find the X11 libraries
dnl
AC_PATH_XTRA
AC_CHECK_X_HEADER(Xm/Xm.h,,
  AC_MSG_ERROR([Motif/Lestif header not found. If you have Motif/Lesstif
                installed perhaps you need to use the --x-includes option.])

)
AC_CHECK_X_HEADER(X11/Xmu/Editres.h,,,
[#include <X11/Intrinsic.h>
])

AC_CACHE_CHECK(for X11 app-defaults,x_applibdir,
[# Look for the header file in a standard set of common directories.
# Check X11 before X11Rn because it is often a symlink to the current release.
  for ac_dir in                        \
    $prefix/lib/app-defaults           \
    $prefix/lib/X11/app-defaults       \
    /usr/lpp/X11/lib/app-defaults      \
    /usr/local/lib/X11/app-defaults    \
    /usr/lib/X11/app-defaults          \
    /usr/openwin/lib/app-defaults      \
    /usr/X11R6/lib/X11/app-defaults    \
    ; \
  do
    if test -d "$ac_dir"
    then
        x_applibdir="$ac_dir"
	break
    fi

  done])
AC_SUBST(x_applibdir)

dnl =============================
dnl Checks for library functions.
dnl =============================
AC_CHECK_LIB(s,  IDtouser, AC_DEFINE(HAVE_IDTOUSER, 1, 
	     [Define if you have IDtouser() file.  
	     This is used to convert a uid to a named. getpwuid() extracting
	     the pw_name field does the same thing.]) LIBS="$LIBS -ls")

dnl Libraries kvm, elf and kstat are used by Solaris and other in
dnl getting the load average.

AC_CHECK_LIB(elf, elf_begin, LIBS="-lelf $LIBS")
AC_CHECK_LIB(kvm, kvm_open, LIBS="-lkvm $LIBS")

AC_CHECK_LIB(kstat, kstat_open, LIBS="$LIBS -lkstat")

dnl AC_FUNC_GETLOADAVG also sets KMEM_GROUP which we use in the Makefile.
AC_PROG_GCC_TRADITIONAL
AC_FUNC_STAT
AC_CHECK_FUNCS([gethostname getloadavg kill memset getpriority \
		regcomp regexec regerror setpriority strerror strrchr])


X_LIBS="$X_LIBS -lXm -lXt -lXext -lX11"
rocky_save_LIBS="$LIBS"
LIBS="$LIBS $x_libdir"
AC_CHECK_LIB(Xmu, _XEditResCheckMessages, X_LIBS="$X_LIBS -lXmu"; 
		  AC_DEFINE(HAVE_XEDITRESCHECKMESSAGES, 1, 
		  [Define if you have the XEditResCheckMessages() to be able 
		  to use the X editres program to view/set Widgets/Resources 
		  in this program.]),,$X_LIBS)
LIBS="$rocky_save_LIBS"

dnl This had been for makesig.pl, but it can use $PATH just as 
dnl easily as configure.
dnl AC_PATH_PROG(KILL, kill, kill, $PATH:/bin:/sbin:/usr/bin:/usr/sbin)
dnl AC_SUBST(KILL)

dnl ===================================================
dnl Info gathered. Create the output files, symlinks..
dnl First remove extra slashes in directory names.
dnl ===================================================
dnl bindir=`echo  "${bindir}"  | sed 's@/$@@;s@//*@/@g'`
dnl prefix=`echo  "${prefix}"  | sed 's@/$@@;s@//*@/@g'`
dnl exec_prefix=`echo  "${exec_prefix}"  | sed 's@/$@@;s@//*@/@g'`
dnl x_includes=`echo  "${x_includes}"  | sed 's@/$@@;s@//*@/@g'`
dnl x_libraries=`echo  "${x_libraries}"  | sed 's@/$@@;s@//*@/@g'`
dnl libdir=`echo  "${libdir}"  | sed 's@/$@@;s@//*@/@g'`
dnl includedir=`echo  "${includedir}"  | sed 's@/$@@;s@//*@/@g'`
dnl srcdir=`echo  "${srcdir}"  | sed 's@/$@@;s@//*@/@g'`
dnl mandir=`echo  "${mandir}"  | sed 's@/$@@;s@//*@/@g'`


dnl We use a path for perl so the #! line in autoscan will work.
AC_PATH_PROG(PERL, perl, no)
AC_SUBST(PERL)dnl

AC_OUTPUT([
	doc/Makefile
	intl/Makefile
	macros/Makefile
	src/AIX/Makefile
	src/BSDI/Makefile
	src/FreeBSD/Makefile
	src/Linux/Makefile
	src/NetBSD/Makefile
	src/SunOS/Makefile
	src/libgtop/Makefile
	src/Makefile
	Makefile	
	src/help/Makefile
	doc/xps.pod
        xps.spec
])

[
perl $srcdir/src/postconf.pl `(cd $srcdir/src; pwd)`
]
