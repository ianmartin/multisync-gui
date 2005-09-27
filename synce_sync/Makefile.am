## Process this synce with automake to produce Makesynce.in

AM_CFLAGS = @XMLCFLAGS@ -Werror -Wall

INCLUDES = @PACKAGE_CFLAGS@ -I$(top_srcdir)

noinst_LTLIBRARIES = libsynce_sync.la

libsynce_sync_la_SOURCES = \
	synce_sync.c \
	interface.c interface.h \
	callbacks.c callbacks.h

libsynce_sync_la_LDFLAGS = @XMLLDFLAGS@
libsynce_sync_la_LIBADD = @PACKAGE_LIBS@
