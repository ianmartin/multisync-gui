## Process this file with automake to produce Makefile.in

AM_CFLAGS = @XMLCFLAGS@ -Werror

INCLUDES = @PACKAGE_CFLAGS@ -I$(top_srcdir)

noinst_LTLIBRARIES = libevo_sync.la

libevo_sync_la_SOURCES = \
	evolution_sync.c \
	interface.c interface.h \
	callbacks.c callbacks.h

libevo_sync_la_LDFLAGS = @XMLLDFLAGS@
libevo_sync_la_LIBADD = @PACKAGE_LIBS@
