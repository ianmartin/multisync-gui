## Process this file with automake to produce Makefile.in

AM_CFLAGS = -Werror

INCLUDES = @PACKAGE_CFLAGS@ -I$(top_srcdir)

noinst_LTLIBRARIES = libfile_sync.la

libfile_sync_la_SOURCES = \
	file_sync.c \
	interface.c interface.h \
	callbacks.c callbacks.h

libfile_sync_la_LIBADD = @PACKAGE_LIBS@
