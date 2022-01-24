AM_LDFLAGS = -lm
AM_CFLAGS = -fms-extensions \
    -D_GNU_SOURCE \
    @LIBUSB_CFLAGS@ \
    @HIDAPI_CFLAGS@ \
    @STD_CFLAGS@ \
    -I$(top_srcdir)/src

AM_CPPFLAGS = -fms-extensions \
    -D_GNU_SOURCE \
    @LIBUSB_CFLAGS@ \
    @HIDAPI_CFLAGS@ \
    -I$(top_srcdir)/src

bin_PROGRAMS = ez_usb_rot_dec

ez_usb_rot_dec_SOURCES = \
    $(top_srcdir)/src/common/FreeD.c \
    $(top_srcdir)/src/common/utils.c \
    $(top_srcdir)/src/common/strl.c \
    $(top_srcdir)/src/common/logger.c \
    $(top_srcdir)/src/common/demonize.c \
    freed_sender.c \
    instance.c \
    main.c \
    ez_usb_rot_dec.c

ez_usb_rot_dec_LDADD = \
    @LIBUSB_LIBS@ \
    @HIDAPI_LIBS@ \
    @LIBDEP@

CLEANFILES = ez_usb_rot_dec.git.h

instance.c: ez_usb_rot_dec.git.h

ez_usb_rot_dec.git.h:
	echo -e "#ifndef GITVERSION_H\n#define GITVERSION_H\n#define GITVERSION \""`git describe --always 2> /dev/null`"\"\n#endif\n" > ez_usb_rot_dec.git.h
