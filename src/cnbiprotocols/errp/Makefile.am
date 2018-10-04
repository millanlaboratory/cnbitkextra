#AM_CPPFLAGS = $(CPPFLAGS) -I$(top_srcdir)/src
AM_CXXFLAGS = $(CXXFLAGS) -I$(top_srcdir)/src -std=c++0x

dist_bin_SCRIPTS = errp_online \
				   errpm_online

bin_PROGRAMS = errp_offline \
			   errpm_offline


errp_offline_LDADD = $(builddir)/../../cnbismr/libcnbismr.la \
					 $(builddir)/../../cnbiprotocol/libcnbiprotocol.la \
					 -ldrawtk -lSDL -lSDL_mixer -llpttrigger \
					 -lcnbicore -lcnbiconfig -lcnbiloop -ltobiid
errpm_offline_LDADD = $(builddir)/../../cnbismr/libcnbismr.la \
					 $(builddir)/../../cnbiprotocol/libcnbiprotocol.la \
					 -ldrawtk -lSDL -lSDL_mixer -llpttrigger \
					 -lcnbicore -lcnbiconfig -lcnbiloop -ltobiid


errp_offline_SOURCES = 	errp_offline.cpp \
						ErrpSquares.hpp \
						ErrpSquares.cpp \
						errp_utilities.hpp
errpm_offline_SOURCES = errpm_offline.cpp \
						errpm_squares.hpp \
						errpm_squares.cpp \
						errpm_utilities.hpp
