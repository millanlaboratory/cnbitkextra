AM_CFLAGS = -I$(top_srcdir)/src/feslanguage/

check_PROGRAMS = jointcontroller \
				 channelsloop \
				 fldecode \
				 flencode \
				 jointreader

jointcontroller_SOURCES = jointcontroller.c
channelsloop_SOURCES = channelsloop.c
fldecode_SOURCES = fldecode.c
flencode_SOURCES = flencode.c
jointreader_SOURCES = jointreader.c

LDADD = $(top_builddir)/src/feslanguage/libfeslanguage.la


