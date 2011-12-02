dnl
dnl $Id: acinclude.m4,v 1.22 2004/07/02 18:43:01 rwscott Exp $
dnl
dnl This file provides the following macros:
dnl
dnl AC_PATH_MOTIF_DIRECT
dnl AC_PATH_MOTIF
dnl LT_HAVE_GETSERVBYNAME
dnl LT_HAVE_GETHOSTBYNAME
dnl LT_WITH_DMALLOC
dnl AC_HAVE_EXCTAGS
dnl LT_HAVE_LIBXP
dnl LT_NEED_LIBM
dnl

dnl
dnl Search for Motif by using explicit paths
dnl
AC_DEFUN(AC_PATH_MOTIF_DIRECT,
[test -z "$motif_direct_test_library" && motif_direct_test_library=Xm
test -z "$motif_direct_test_function" && motif_direct_test_function=XmCreatePushButton
test -z "$motif_direct_test_include" && motif_direct_test_include=Xm/Xm.h
  for ac_dir in               \
    /usr/include/Motif2.1     \
    /usr/Motif2.1/include     \
                              \
    /usr/include/Motif2.0     \
    /usr/Motif2.0/include     \
                              \
    /usr/include/Motif1.2     \
    /usr/Motif1.2/include     \
                              \
    /usr/motif/include        \
    /usr/lesstif/include      \
                              \
    /usr/X11R6/include        \
    /usr/X11R5/include        \
                              \
    /usr/include/X11R6        \
    /usr/include/X11R5        \
                              \
    /usr/local/X11R6/include  \
    /usr/local/X11R5/include  \
                              \
    /usr/local/include/X11R6  \
    /usr/local/include/X11R5  \
                              \
    /usr/X11/include          \
    /usr/include/X11          \
    /usr/local/X11/include    \
    /usr/local/include/X11    \
                              \
    /usr/X386/include         \
    /usr/x386/include         \
    /usr/XFree86/include/X11  \
                              \
    /usr/dt/include           \
    /usr/openwin/include      \
    /opt/openwin/include      \
                              \
    /usr/include              \
    /usr/local/include        \
    /usr/unsupported/include  \
    /usr/athena/include       \
    /usr/local/x11r5/include  \
    /usr/lpp/Xamples/include  \
    "${prefix}"/include       \
    "${HOME}"/include         \
    ; \
  do
    if test -r "$ac_dir/$motif_direct_test_include"; then
      no_motif= ac_motif_includes=$ac_dir
      break
    fi
  done

# Check for the libraries.
# See if we find them without any special options.
# Don't add to $LIBS permanently.
ac_save_LIBS="$LIBS"
LIBS="-l$motif_direct_test_library $LIBS"
# First see if replacing the include by lib works.
for ac_dir in `echo "$ac_motif_includes" | sed s/include/lib/` \
    /usr/lib/Motif2.1     \
    /usr/Motif2.1/lib     \
                          \
    /usr/lib/Motif2.0     \
    /usr/Motif2.0/lib     \
                          \
    /usr/lib/Motif1.2     \
    /usr/Motif1.2/lib     \
                          \
    /usr/motif/lib        \
    /usr/lesstif/lib      \
                          \
    /usr/X11R6/lib        \
    /usr/X11R5/lib        \
                          \
    /usr/lib/X11R6        \
    /usr/lib/X11R5        \
                          \
    /usr/local/X11R6/lib  \
    /usr/local/X11R5/lib  \
                          \
    /usr/local/lib/X11R6  \
    /usr/local/lib/X11R5  \
                          \
    /usr/X11/lib          \
    /usr/lib/X11          \
    /usr/local/X11/lib    \
                          \
    /usr/X386/lib         \
    /usr/x386/lib         \
    /usr/XFree86/lib/X11  \
                          \
    /usr/dt/lib           \
    /usr/openwin/lib      \
    /opt/openwin/lib      \
                          \
    /usr/lib              \
    /usr/local/lib        \
    /usr/unsupported/lib  \
    /usr/athena/lib       \
    /usr/local/x11r5/lib  \
    /usr/lpp/Xamples/lib  \
    "${prefix}"/include   \
    "${HOME}"/include     \
    ; \
  do
  for ac_extension in "a" "so" "sl" "lib" ; do
    if test -r $ac_dir/lib${motif_direct_test_library}.$ac_extension; then
      no_motif= ac_motif_libraries=$ac_dir
      break 2
    fi
  done
done
LIBS=$ac_save_LIBS
]
) dnl AC_PATH_MOTIF_DIRECT()



dnl
dnl Search for Motif
dnl
AC_DEFUN(AC_PATH_MOTIF,
[
AC_REQUIRE_CPP()

AC_ARG_WITH(motif-includes, [  --with-motif-includes=DIR     Motif include files are in DIR])
if test -z "$with_motif_includes"; then
  motif_includes=NONE
else
  motif_includes=$with_motif_includes
fi
AC_ARG_WITH(motif-libraries, [  --with-motif-libraries=DIR    Motif library files are in DIR])
if test -z "$with_motif_libraries"; then
  motif_libraries=NONE
else
  motif_libraries=$with_motif_libraries
fi

AC_MSG_CHECKING(for Motif)
AC_ARG_WITH(motif, [  --with-motif            enable Motif tests])
if test "x$with_motif" = xno; then
  no_motif=yes
else
  if test "x$motif_includes" != xNONE && test "x$motif_libraries" != xNONE; then
    no_motif=
  else
    AC_CACHE_VAL(ac_cv_path_motif,
    [# One or both of these vars are not set, and there is no cached value.
        no_motif=yes

        #
        # Let's try a test link. If it works this will avoid putting the
        # default paths onto the compile and link lines.
        #
        ac_save_libs="$LIBS"
        ac_save_cflags="$CFLAGS"
        ac_save_cppflags="$CPPFLAGS"
        LIBS="-lXm $XPLIB $X_LIBS -lXt $X_PRE_LIBS -lX11 $X_EXTRA_LIBS $LIBS"
        CFLAGS="$X_CFLAGS $CFLAGS"
        CPPFLAGS="$X_CFLAGS $CPPFLAGS"

        AC_TRY_LINK(
	[#include <Xm/Label.h>],[Widget w; XmCreateLabel(w, "", NULL, 0);],
        [
        #
        # link passed, do nothing
        #
        no_motif="no"
        motif_includes=""
        motif_libraries=""
        ],
        [
        #
        # link failed, go search for it
        #
        AC_PATH_MOTIF_DIRECT
        dnl The macro above is a pure file search.
        dnl We need to verify that we can actually link that stuff
        AC_TRY_LINK([#include <Xm/Label.h>],[Widget w; XmCreateLabel(w, "", NULL, 0);],
  	   [
	   #
	   # link passed, do nothing
	   ],
	   [
	   #
	   # link failed, bail out
	   AC_MSG_ERROR(Motif not properly installed)
	   ]
	   ) dnl AC_TRY_LINK
	   ]
    ) dnl AC_TRY_LINK

    LIBS="$ac_save_libs"
    CFLAGS="$ac_save_cflags"
    CPPFLAGS="$ac_save_cppflags"

    if test "$no_motif" = yes; then
      ac_cv_path_motif="no_motif=yes"
    else
      ac_cv_path_motif="no_motif= ac_motif_includes=$ac_motif_includes ac_motif_libraries=$ac_motif_libraries"
    fi]
    )  dnl AC_CACHE_VAL()

  fi
  eval "$ac_cv_path_motif"
fi # with_motif != no

if test "$no_motif" = yes; then
  AC_MSG_RESULT(no)
else
  test "x$motif_includes" = xNONE && motif_includes=$ac_motif_includes
  test "x$motif_libraries" = xNONE && motif_libraries=$ac_motif_libraries
  ac_cv_path_motif="no_motif= ac_motif_includes=$motif_includes ac_motif_libraries=$motif_libraries"
  AC_MSG_RESULT([libraries $motif_libraries, headers $motif_includes])
fi
]
) dnl AC_PATH_MOTIF()



dnl
dnl Extended check for getservbyname()
dnl
AC_DEFUN(LT_HAVE_GETSERVBYNAME,
[AC_REQUIRE([AC_PATH_X])
AC_CACHE_CHECK(whether getservbyname() is available, lt_cv_getservbyname,
[lt_save_CFLAGS="$CFLAGS"
lt_save_CPPFLAGS="$CPPFLAGS"
lt_save_LIBS="$LIBS"
LIBS="$X_EXTRA_LIBS"
CFLAGS="$X_CFLAGS $CFLAGS"
CPPFLAGS="$X_CFLAGS $CPPFLAGS"
AC_TRY_LINK(
[
#include <netdb.h>
],
[
struct servent *se;
const char *service="";
const char *protocol="tcp";
se=getservbyname(service, protocol);
],
lt_cv_getservbyname=yes,
lt_cv_getservbyname=no)
CFLAGS="$lt_save_CFLAGS"
CPPFLAGS="$lt_save_CPPFLAGS"
LIBS="$lt_save_LIBS"
])
if test $lt_cv_getservbyname = yes; then
  AC_DEFINE(HAVE_GETSERVBYNAME, [], [Define if you have getservbyname])
fi
])



dnl
dnl Extended check for gethostbyname()
dnl
AC_DEFUN(LT_HAVE_GETHOSTBYNAME,
[AC_REQUIRE([AC_PATH_X])
AC_CACHE_CHECK(whether gethostbyname() is available, lt_cv_gethostbyname,
[lt_save_CFLAGS="$CFLAGS"
lt_save_CPPFLAGS="$CPPFLAGS"
lt_save_LIBS="$LIBS"
LIBS="$X_EXTRA_LIBS"
CFLAGS="$X_CFLAGS $CFLAGS"
CPPFLAGS="$X_CFLAGS $CPPFLAGS"
AC_TRY_LINK(
[
#include <netdb.h>
],
[
struct hostent *she;
const char *name="";
she=gethostbyname(name);
],
lt_cv_gethostbyname=yes,
lt_cv_gethostbyname=no)
CFLAGS="$lt_save_CFLAGS"
CPPFLAGS="$lt_save_CPPFLAGS"
LIBS="$lt_save_LIBS"
])
if test $lt_cv_gethostbyname = yes; then
  AC_DEFINE(HAVE_GETHOSTBYNAME, [], [Define if you have gethostbyname])
fi
])


dnl
dnl Enable malloc checker for debugging purposes
dnl See http://dmalloc.com
dnl
AC_DEFUN(LT_WITH_DMALLOC,
[AC_MSG_CHECKING(if malloc debugging is wanted)
AC_ARG_WITH(dmalloc,
[  --with-dmalloc[=path]   use dmalloc, see INSTALL(.html) for reference],
[if test "$withval" = no; then
  AC_MSG_RESULT(no)
else
dnl We overwrite the variables since we won't continue in
dnl case of an error!
dnl We modify CFLAGS, and also link libs (LDFLAGS) and programs (LIBS)
  if test "$withval" != yes; then
dnl  a path was given
     CFLAGS="$CFLAGS -I$withval/include -DDMALLOC_FUNC_CHECK"

     LDFLAGS="$LDFLAGS -L$withval/lib -ldmalloc"
     LIBS="$LIBS -L$withval/lib -ldmalloc"
  else
dnl  no path was given
     CFLAGS="$CFLAGS -DDMALLOC_FUNC_CHECK"
     LDFLAGS="$LDFLAGS -ldmalloc"
     LIBS="$LIBS -ldmalloc"
  fi
  AC_TRY_LINK(
  [#include <dmalloc.h>],
  [char *ptr;
  ptr=malloc(1);
  free(ptr);]
  ,
  [AC_DEFINE(WITH_DMALLOC,1,
            [Define if using the dmalloc debugging malloc package])
  AC_MSG_RESULT(Using dmalloc)],
  AC_MSG_ERROR(dmalloc not found)
  )
fi],
[AC_MSG_RESULT(no)])
])


dnl
dnl Checks whether Exuberant ctags is present
dnl (see http://darren.hiebert.com/ctags/index.html)
dnl
AC_DEFUN(AC_HAVE_EXCTAGS,
[
ac_have_exctags=no
AC_CHECK_PROG(ac_have_ctags, ctags, yes, no)
if test "$ac_have_ctags" = "yes"; then
  AC_MSG_CHECKING(if ctags is actually Exuberant ctags)
  if test -z "`ctags --version 2>/dev/null | grep Exuberant`" ; then
    ac_have_exctags=no
  else
    ac_have_exctags=yes
  fi
  AC_MSG_RESULT($ac_have_exctags)
fi 
AM_CONDITIONAL(Have_Exctags, test "$ac_have_exctags" = "yes")
])


dnl
dnl Check for libXp
dnl In fact this check ensures that
dnl  - <X11/extensions/Print.h> and
dnl  - both libXp libXext
dnl are in place
dnl Note that a simpler check only for the libraries would not
dnl be sufficient perhaps.
dnl If the test succeeds it defines Have_Libxp within our 
dnl Makefiles. Perhaps one should immediately add those libs
dnl to link commands which include libXm version2.1?!
dnl
AC_DEFUN(LT_HAVE_LIBXP,
[AC_REQUIRE([AC_PATH_X])
AC_CACHE_CHECK(whether libXp is available, lt_cv_libxp,
[lt_save_CFLAGS="$CFLAGS"
lt_save_CPPFLAGS="$CPPFLAGS"
lt_save_LIBS="$LIBS"
LIBS="$X_LIBS -lXp -lXext -lXt $X_PRE_LIBS -lX11 $X_EXTRA_LIBS $LIBS"
CFLAGS="$X_CFLAGS $CFLAGS"
CPPFLAGS="$X_CFLAGS $CPPFLAGS"
AC_TRY_LINK([
#include <X11/Intrinsic.h>
#include <X11/extensions/Print.h>
],[
Display *display=NULL;
short   major_version, minor_version;
Status rc;
rc=XpQueryVersion(display, &major_version, &minor_version);
exit(0);
],
lt_cv_libxp=yes,
lt_cv_libxp=no)
])
if test "$lt_cv_libxp" = "yes"; then
  AC_DEFINE(HAVE_LIB_XP, [], [Define if you have libXp])
  LT_HAVELIBXP=1
else
  LT_HAVELIBXP=0 
fi
AM_CONDITIONAL(Have_Libxp, test "$lt_cv_libxp" = "yes")
AC_SUBST(LT_HAVELIBXP)
CFLAGS="$lt_save_CFLAGS"
CPPFLAGS="$lt_save_CPPFLAGS"
LIBS="$lt_save_LIBS"
])


dnl
dnl Check whether -lm provides us with e.g. sin()
dnl
AC_DEFUN(LT_NEED_LIBM,
[
AC_MSG_CHECKING([for sin within libm])
lt_save_LIBS="$LIBS"
LIBS="$LIBS -lm"
AC_TRY_LINK([
#include <stdlib.h>
#include <math.h>
],
[
double x;
x=sin(0);
exit(0);
],
lt_cv_libm=yes,
lt_cv_libm=no
LIBS="$lt_save_LIBS -lm"
)
AC_MSG_RESULT($lt_cv_libm)
])
