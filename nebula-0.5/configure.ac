dnl                                               -*- Autoconf -*-
dnl Process this file with autoconf to produce a configure script.
dnl
dnl
AC_PREREQ(2.59)
AC_INIT(nebula, 0.5, rmottola@users.sourceforge.net)
AM_INIT_AUTOMAKE([1.8.5])
AC_CONFIG_SRCDIR([interpreter.c])
AC_CONFIG_HEADER([config.h])
dnl
dnl --------------------------------------------------------------
dnl
dnl set Architecure string
AC_CANONICAL_BUILD
NEBULA_ARCHOS="($build_os running on $build_cpu)"
AC_DEFINE_UNQUOTED(ARCHOS, "$NEBULA_ARCHOS", [the platform we are building
for])
AC_SUBST(NEBULA_ARCHOS)
dnl
dnl
dnl Checks for programs.
AC_PROG_CC
dnl
dnl with a c++ compiler we should check for -isystem PATH instead of -I
dnl
ISYSTEM="-I"
AC_SUBST(ISYSTEM)
dnl
dnl Checks for libraries.
dnl
if test "$with_x" = no; then
AC_MSG_ERROR([Nebula requires the X window system to compile and run.
                  Please do not use the configure option '--without-x'.])
fi
if test "$with_motif" = no; then
AC_MSG_ERROR([Nebula requires Motif to compile and run.
		  Please do not use the configure option '--without-motif'.])
fi
dnl
AC_PATH_XTRA
dnl
dnl
dnl Check for Motif widget libraries
dnl
dnl
dnl Motif includes and libraries...
dnl
motif_includes=
motif_libraries=
AC_ARG_WITH(motif,
[  --without-motif                  do not use Motif widgets])
dnl Treat --without-motif like
dnl --without-motif-includes --without-motif-libraries.
if test "$with_motif" = "no"
then
motif_includes=no
motif_libraries=no
fi
AC_ARG_WITH(motif-includes,
[  --with-motif-includes=DIR        Motif include files are in DIR],
motif_includes="$withval")
AC_ARG_WITH(motif-libraries,
[  --with-motif-libraries=DIR       Motif libraries are in DIR],
motif_libraries="$withval")
AC_MSG_CHECKING(for Motif)
dnl
dnl Search for the include files
dnl
dnl XXXX SEARCH CODE
dnl
dnl
dnl Add Motif definitions to X flags
dnl
if test "$motif_includes" != "" && test "$motif_includes" != "$x_includes" && test "$motif_includes" != "no"
then
X_CFLAGS="$ISYSTEM$motif_includes $X_CFLAGS"
fi
if test "$motif_libraries" != "" && test "$motif_libraries" != "$x_libraries" && test "$motif_libraries" != "no"
then
case "$X_LIBS" in
  *-R\ *) X_LIBS="-L$motif_libraries -R $motif_libraries $X_LIBS";;
  *-R*)   X_LIBS="-L$motif_libraries -R$motif_libraries $X_LIBS";;
  *)      X_LIBS="-L$motif_libraries $X_LIBS";;
esac
fi
dnl
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
dnl
LIBS="$LIBS $X_EXTRA_LIBS"
CFLAGS="$CFLAGS $X_CFLAGS"
CPPFLAGS="$CPPFLAGS $X_CFLAGS"
LDFLAGS="$LDFLAGS $X_LIBS"
dnl
dnl Check for X library
dnl
X11_LIBS=""
AC_CHECK_LIB([X11],[XOpenDisplay],[X11_LIBS="-lX11"],[],
	[${X_PRE_LIBS} ${X_EXTRA_LIBS}])
if test "$X11_LIBS" = ""; then
dnl Not having X is fatal.  Let the user fix this.
AC_MSG_ERROR([The X11 library '-lX11' could not be found.
		  Please use the configure options '--x-includes=DIR'
		  and '--x-libraries=DIR' to specify the X location.
		  See the files 'config.log' for further diagnostics.])
fi
AC_SUBST(X_LIBS)
AC_SUBST(X11_LIBS)
AC_SUBST(X_PRE_LIBS)
dnl
dnl
dnl Check for X toolkit libraries
dnl
XT_LIBS=""
AC_CHECK_LIB([Xt],[XtToolkitInitialize],[XT_LIBS="-lXt"],[],
	[${X_PRE_LIBS} ${X11_LIBS} ${X_EXTRA_LIBS}])
if test "$XT_LIBS" = ""; then
dnl Not having Xt is fatal.  Let the user fix this.
AC_MSG_ERROR([The X toolkit library '-lXt' could not be found.
 = 		  Please use the configure options '--x-includes=DIR'
		  and '--x-libraries=DIR' to specify the X location.
		  See the files 'config.log' for further diagnostics.])
										fi
AC_SUBST(XT_LIBS)
if test "$motif_includes" = "no"
then
dnl Without Xm/Xm.h, we won't get far.  Let the user fix this.
AC_MSG_ERROR([The Motif include file 'Xm/Xm.h' could not be found.
                  Please use the configure options '--with-motif-includes=DIR'
                  and '--with-motif-libraries=DIR' to specify the Xm location.
                  See the files 'config.log' and 'ddd/config.log'
                  for further diagnostics.])
fi
XM_LIBS=""
XP_LIBS=""
if test "$motif_includes" != "no" && test "$motif_libraries" != "no"
then
AC_CHECK_LIB([Xm],[XmCreateOptionMenu],
        [XM_LIBS="-lXm"
        AC_DEFINE([HAVE_MOTIF],1,[Define if you have Motif widgets.])],
        [],
        [${XMU_LIBS} ${XT_LIBS} ${XP_LIBS} ${XEXT_LIBS} ${X_PRE_LIBS} ${X11_LIBS} ${X_EXTRA_LIBS} ${LIBGEN}])
dnl
fi
if test "$XM_LIBS" = ""; then
dnl Without Motif, we won't get far.  Let the user fix this.
AC_MSG_ERROR([The Motif library '-lXm' could not be found.
                  Please use the configure options '--with-motif-includes=DIR'
                  and '--with-motif-libraries=DIR' to specify the Xm location.
                  See the files 'config.log' and for further diagnostics.])
fi
AC_SUBST(XM_LIBS)

dnl 
dnl ceck for the PW library neeeded to link on some older motif systems
dnl
AC_CHECK_LIB([PW], [regcmp], [XM_LIBS="$XM_LIBS -lPW"], [], [])

dnl
dnl Now check for some specific Motif functions available in different version
dnl
OLD_LIBS=$LIBS
LIBS="$LIBS $XM_LIBS $XT_LIBS $X11_LIBS"
AC_CHECK_FUNCS(XmStringCreateLocalized)
AC_CHECK_FUNCS(XmStringGenerate)
LIBS=$OLD_LIBS

dnl
dnl Now Check for silicon graphics specific motif extensions
dnl
SGM_LIBS=""
AC_CHECK_LIB([Sgm], [SgCreateList], [XM_LIBS="-lSgm $XM_LIBS"], [], [])
AC_SUBST(SGM_LIBS)

dnl
dnl Checks for header files.
dnl
AC_CHECK_HEADERS([arpa/inet.h fcntl.h netdb.h netinet/in.h strings.h sys/socket.h sys/time.h unistd.h])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_TIME

dnl
dnl Checks for library functions.
dnl
AC_HEADER_STDC
AC_TYPE_SIGNAL
AC_CHECK_FUNCS([gethostbyname inet_ntoa socket strstr])

dnl a small hack
LIBS="$LIBS $XM_LIBS $XT_LIBS $X11_LIBS"

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

dnl
dnl Give an indication of what to do next.
dnl
AC_MSG_NOTICE([
configuration is done.  Type `make' to make Nebula. In case of trouble, please
check the settings in the files generated by `configure' - that is, `config.h' and `Makefile'.])
