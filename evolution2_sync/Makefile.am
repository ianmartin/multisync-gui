## Process this file with automake to produce Makefile.in

AM_CFLAGS = @XMLCFLAGS@ -Werror -Wall

INCLUDES = @PACKAGE_CFLAGS@ -I$(top_srcdir)

noinst_LTLIBRARIES = libevo2_sync.la

libevo2_sync_la_SOURCES = \
	evolution2_sync.c \
	interface.c interface.h \
	callbacks.c callbacks.h

libevo2_sync_la_LDFLAGS = @XMLLDFLAGS@
libevo2_sync_la_LIBADD = @PACKAGE_LIBS@
