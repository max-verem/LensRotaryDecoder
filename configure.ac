dnl
dnl autoreconf --force --install ; ./configure ; make
dnl

dnl Process this file with autoconf to produce a configure script.
dnl subdir-objects
AC_INIT([LensRotaryDecoder], [1.0])
dnl AM_INIT_AUTOMAKE([no-dependencies foreign])
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_HEADERS(config.h)
dnl AM_CONDITIONAL([AMDEP],true)

AM_MAINTAINER_MODE
dnl AC_PROG_RANLIB

AC_ISC_POSIX
: ${CFLAGS="-g3 -ggdb -O0 -Wall"}
AC_PROG_CC
AC_GNU_SOURCE
AM_PROG_CC_STDC
AM_PROG_CC_C_O
AC_HEADER_STDC
AC_PROG_CXX
AM_PROG_AS
dnl AC_PROG_RANLIB

dnl LT_INIT([disable-static])

AC_SUBST([LIBTOOL_DEPS])

AC_CHECK_FUNCS(localtime_r)

DATADIRNAME="share"

LIBDEP="-lpthread"
case "${host}" in
    *-*-mingw32*)
        LIBDEP="$LIBDEP -lws2_32 -lwsock32 -lwinmm"
        win32=true
        AC_CHECK_DECLS([localtime_r], [], [], [#include <time.h>])
        ;;
esac
AC_SUBST(LIBDEP)
AM_CONDITIONAL([WIN32], [test x$win32 = xtrue])

AC_CHECK_LIB([pthread], [pthread_create])
AC_CHECK_LIB([dl], [dlopen])

AC_ARG_WITH([vrpn],
        [AS_HELP_STRING([--with-vrpn=path], [path to VRPN framework])],
        [VRPN_PATH=$withval],
        [VRPN_PATH=/usr/local/vrpn/usr/local])
AC_SUBST(VRPN_PATH)

VRPN_CFLAGS="-I$VRPN_PATH/include"
AC_SUBST(VRPN_CFLAGS)

VRPN_LIBS="-L$VRPN_PATH/lib -lvrpnserver -lvrpn -lquat"
AC_SUBST(VRPN_LIBS)

STD_CFLAGS="-std=gnu99"
AC_SUBST(STD_CFLAGS)

PKG_CHECK_MODULES(WEBSOCKETS, [libwebsockets])
AC_SUBST(WEBSOCKETS_CFLAGS)
AC_SUBST(WEBSOCKETS_LIBS)

PKG_CHECK_MODULES(OPENCV, [opencv4])
AC_SUBST(OPENCV_CFLAGS)
AC_SUBST(OPENCV_LIBS)

PKG_CHECK_MODULES(HIDAPI, [hidapi-libusb])
AC_SUBST(HIDAPI_CFLAGS)
AC_SUBST(HIDAPI_LIBS)

PKG_CHECK_MODULES(LIBUSB, [libusb-1.0])
AC_SUBST(LIBUSB_CFLAGS)
AC_SUBST(LIBUSB_LIBS)

AM_PATH_XML2(2.4.0)

IT_PROG_INTLTOOL([0.35.0])
GETTEXT_PACKAGE=TheCoreElements
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE","")
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.17])
PROGRAMNAME_LOCALEDIR=[${datadir}/locale]
AC_SUBST(PROGRAMNAME_LOCALEDIR)

dnl Add the languages which your application supports here.
dnl #ALL_LINGUAS=""
dnl #AM_GLIB_GNU_GETTEXT

dnl Use -Wall if we have gcc.
changequote(,)dnl
if test "x$GCC" = "xyes"; then
  case " $CFLAGS " in
  *[\ \	]-Wall[\ \	]*) ;;
  *) CFLAGS="$CFLAGS -Wall" ;;
  esac
fi
changequote([,])dnl

AC_OUTPUT([
Makefile
po/Makefile.in
src/Makefile
src/ez-usb/Makefile
src/stm32_rot_dec/Makefile
src/mojo_rot_dec/Makefile
])
