AM_LDFLAGS = -lm
AM_CFLAGS = -fms-extensions \
    -D_GNU_SOURCE \
    @STD_CFLAGS@ \
    -I$(top_srcdir)/src

AM_CPPFLAGS = -fms-extensions \
    -D_GNU_SOURCE \
    -I$(top_srcdir)/src

bin_PROGRAMS = mojo_rot_dec

mojo_rot_dec_SOURCES = \
    $(top_srcdir)/src/common/utils.c \
    $(top_srcdir)/src/common/strl.c \
    $(top_srcdir)/src/common/logger.c \
    $(top_srcdir)/src/common/demonize.c \
    instance.c \
    main.c \
    mojo_rot_dec.c

mojo_rot_dec_LDADD = \
    @LIBDEP@

CLEANFILES = mojo_rot_dec.git.h

instance.c: mojo_rot_dec.git.h

mojo_rot_dec.git.h:
	echo -e "#ifndef GITVERSION_H\n#define GITVERSION_H\n#define GITVERSION \""`git describe --always 2> /dev/null`"\"\n#endif\n" > mojo_rot_dec.git.h
