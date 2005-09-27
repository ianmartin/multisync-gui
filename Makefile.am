## Process this file with automake to produce Makefile.in

SUBDIRS = file-sync $(EVO_SYNC_DIR) $(EVO2_SYNC_DIR) synce_sync

if EVO2_SYNC_DIR
EVO2LIB = evolution2_sync/libevo2_sync.la
else
EVO2LIB = 
endif

if EVO_SYNC_DIR
EVOLIB = evolution_sync/libevo_sync.la
else
EVOLIB = 
endif

AM_CFLAGS = -Wall -Werror -I$(top_srcdir)

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
	$(EVO2LIB) \
	$(EVOLIB) \
	synce_sync/libsynce_sync.la

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

