AM_LDFLAGS = -lm
AM_CFLAGS = -fms-extensions \
    -D_GNU_SOURCE \
    @HIDAPI_CFLAGS@ \
    @STD_CFLAGS@ \
    -I$(top_srcdir)/src

AM_CPPFLAGS = -fms-extensions \
    -D_GNU_SOURCE \
    @HIDAPI_CFLAGS@ \
    -I$(top_srcdir)/src

bin_PROGRAMS = ez_usb_rot_dec

ez_usb_rot_dec_SOURCES = \
    ez_usb_rot_dec.c

ez_usb_rot_dec_LDADD = \
    @HIDAPI_LIBS@ \
    @LIBDEP@

CLEANFILES = ez_usb_rot_dec.git.h
