AM_CXXFLAGS = $(CXXFLAGS) -I$(top_srcdir)/src -std=c++0x

bin_PROGRAMS = mi_online \
	       mi_offline \
	       mi_control \
	       mi_tactile_online

noinst_PROGRAMS=mi_online_original

cnbiprotocolsincludedir = $(includedir)/cnbiprotocols
cnbiprotocolsinclude_HEADERS = mi_utilities.hpp \
				  artifact_utilities.hpp 

mi_online_SOURCES = mi_online.cpp mi_utilities.hpp
mi_online_LDADD = $(builddir)/../../cnbismr/libcnbismr.la \
				  $(builddir)/../../cnbiprotocol/libcnbiprotocol.la \
				  -lcnbicore -lcnbiloop -ltobicore -ltobiid -ltobiic \
				  -ldrawtk -llpttrigger\
				  -lSDL_mixer -lcnbiconfig

mi_offline_SOURCES = mi_offline.cpp mi_utilities.hpp
mi_offline_LDADD = $(builddir)/../../cnbismr/libcnbismr.la \
				   $(builddir)/../../cnbiprotocol/libcnbiprotocol.la \
				  -lcnbicore -lcnbiloop -ltobicore -ltobiid -ltobiic \
				  -ldrawtk -llpttrigger\
				  -lSDL_mixer -lcnbiconfig

mi_control_SOURCES = mi_control.cpp mi_utilities.hpp artifact_utilities.hpp
mi_control_LDADD = $(builddir)/../../cnbismr/libcnbismr.la \
				  $(builddir)/../../cnbiprotocol/libcnbiprotocol.la \
				  -lcnbicore -lcnbiloop -ltobicore -ltobiid -ltobiic \
				  -ldrawtk -llpttrigger\
				  -lSDL_mixer -lcnbiconfig

mi_tactile_online_SOURCES = mi_tactile_online.cpp tactile_utilities.hpp
mi_tactile_online_LDADD = $(builddir)/../../cnbismr/libcnbismr.la \
				  $(builddir)/../../cnbiprotocol/libcnbiprotocol.la \
				  -lcnbicore -lcnbiloop -ltobicore -ltobiid -ltobiic \
				  -ldrawtk -llpttrigger\
				  -lSDL_mixer -lcnbiconfig

# Created by L.Tonin  <luca.tonin@epfl.ch> on 13/09/16 14:56:49
# Deprecated protocol. To be removed.
mi_online_original_SOURCES = mi_online_original.cpp mi_utilities.hpp
mi_online_original_LDADD = $(builddir)/../../cnbismr/libcnbismr.la \
				  $(builddir)/../../cnbiprotocol/libcnbiprotocol.la \
				  -lcnbicore -lcnbiloop -ltobicore -ltobiid -ltobiic \
				  -ldrawtk -llpttrigger\
				  -lSDL_mixer -lcnbiconfig
