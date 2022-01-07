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

bin_PROGRAMS = stm32_rot_dec_3x1

stm32_rot_dec_3x1_SOURCES = \
    $(top_srcdir)/src/common/utils.c \
    $(top_srcdir)/src/common/strl.c \
    $(top_srcdir)/src/common/logger.c \
    $(top_srcdir)/src/common/demonize.c \
    instance.c \
    main.c \
    stm32_rot_dec_3x1.c

stm32_rot_dec_3x1_LDADD = \
    @LIBUSB_LIBS@ \
    @HIDAPI_LIBS@ \
    @LIBDEP@

CLEANFILES = stm32_rot_dec_3x1.git.h

instance.c: stm32_rot_dec_3x1.git.h

stm32_rot_dec_3x1.git.h:
	echo -e "#ifndef GITVERSION_H\n#define GITVERSION_H\n#define GITVERSION \""`git describe --always 2> /dev/null`"\"\n#endif\n" > stm32_rot_dec_3x1.git.h
