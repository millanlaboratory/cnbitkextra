#AM_CPPFLAGS = $(CPPFLAGS) -I$(top_srcdir)/src
AM_CXXFLAGS = $(CXXFLAGS) -I$(top_srcdir)/src -std=c++0x

lib_LTLIBRARIES = libcnbismr.la

#bin_PROGRAMS = csmr_triggercheck

libcnbismr_includedir = $(includedir)/cnbismr
libcnbismr_include_HEADERS = CSmrEngine.hpp \
							 CSmrEngineExt.hpp \
							 CSmrEvents.hpp \
							 CSmrTrigger.hpp \
							 CSmrBars.hpp \
							 CSmrCBars.hpp \
							 CSmrCopilot.hpp \
							 stimula.h \
							 generator.h

libcnbismr_la_SOURCES = generator.c \
						generator.h \
						stimula.c \
						stimula.h \
						CSmrEngine.cpp \
						CSmrEngine.hpp \
						CSmrEngineExt.cpp \
						CSmrEngineExt.hpp \
						CSmrEvents.cpp \
						CSmrEvents.hpp \
						CSmrTrigger.cpp \
						CSmrTrigger.hpp \
						CSmrBars.cpp \
						CSmrBars.hpp \
						CSmrCopilot.cpp \
						CSmrCopilot.hpp \
						CSmrCBars.cpp \
						CSmrCBars.hpp 

#libcnbismr_la_LDFLAGS = $(AM_CFLAGS) -ldrawtk -lSDL\
#						-version-info $(CURRENT):$(REVISION):$(AGE)
#libcnbismr_la_LIBADD = -ldrawtk -lSDL

libcnbismr_la_LIBADD = -ldrawtk -lSDL -lcnbicore -lcnbiconfig $(top_srcdir)/src/cnbiprotocol/libcnbiprotocol.la
libcnbismr_la_LDFLAGS = $(AM_CFLAGS) \
						-version-info $(CURRENT):$(REVISION):$(AGE)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = cnbismr.pc
