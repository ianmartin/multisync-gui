## Process this file with automake to produce Makefile.in

SUBDIRS = file-sync evolution2_sync evolution_sync

AM_CFLAGS = -Werror

INCLUDES = \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	@PACKAGE_CFLAGS@

bin_PROGRAMS = multisync0.90

multisync0_90_SOURCES = \
	main.c \
	gui.c \
	pair.c \
	plugins.c \
	support.c support.h \
	interface.c interface.h \
	callbacks.c callbacks.h

multisync0_90_LDADD = @PACKAGE_LIBS@ \
	file-sync/libfile_sync.la \
	evolution2_sync/libevo2_sync.la \
	evolution_sync/libevo_sync.la

EXTRA_DIST = \
	autogen.sh \
	multisync.glade2 \
	multisync.glade2p

install-data-local:
	@$(NORMAL_INSTALL)
	if test -d $(srcdir)/pixmaps; then \
	  $(mkinstalldirs) $(DESTDIR)$(datadir)/pixmaps/$(PACKAGE); \
	  for pixmap in $(srcdir)/pixmaps/*; do \
	    if test -f $$pixmap; then \
	      $(INSTALL_DATA) $$pixmap $(DESTDIR)$(datadir)/pixmaps/$(PACKAGE); \
	    fi \
	  done \
	fi

dist-hook:
	if test -d pixmaps; then \
	  mkdir $(distdir)/pixmaps; \
	  for pixmap in pixmaps/*; do \
	    if test -f $$pixmap; then \
	      cp -p $$pixmap $(distdir)/pixmaps; \
	    fi \
	  done \
	fi

