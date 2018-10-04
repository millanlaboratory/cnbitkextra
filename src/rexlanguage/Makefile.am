AM_CXXFLAGS = $(CXXFLAGS) -I$(top_srcdir)/src -std=c++0x

lib_LTLIBRARIES = librexlanguage.la

librexlanguage_includedir = $(includedir)/rexlanguage

librexlanguage_include_HEADERS = rex.hpp
								

librexlanguage_la_SOURCES = rex.cpp

librexlanguage_la_LDFLAGS = -version-info $(CURRENT):$(REVISION):$(AGE)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = rexlanguage.pc
