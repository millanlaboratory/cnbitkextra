AM_CXXFLAGS = $(CXXFLAGS) -I$(top_srcdir)/src -std=c++0x

lib_LTLIBRARIES = libfeslanguage.la

libfeslanguage_includedir = $(includedir)/feslanguage

libfeslanguage_include_HEADERS = fesencode.h\
								 fesdecode.h\
								 feslanguage.h\
								 serialport.h

libfeslanguage_la_SOURCES = fesbytestream.c\
							fesbytestream.h\
							fesencode.h\
							fesencode.c\
							fesdecode.h\
							fesdecode.c\
				  			feslanguage.h\
							serialport.c\
							serialport.h

libfeslanguage_la_LDFLAGS = -version-info $(CURRENT):$(REVISION):$(AGE)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = feslanguage.pc
