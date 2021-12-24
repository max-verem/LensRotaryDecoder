## Process this file with automake to produce Makefile.in

SUBDIRS = src po

INTLTOOL_FILES = intltool-extract.in \
                 intltool-merge.in \
                 intltool-update.in

DISTCLEANFILES = intltool-extract \
                 intltool-merge \
                 intltool-update \
                 po/.intltool-merge-cache

EXTRA_DIST = \
	$(INTLTOOL_FILES) \
	autogen.sh

install-data-local:
	@$(NORMAL_INSTALL)

dist:
	make -f makedist all
