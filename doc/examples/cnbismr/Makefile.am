AM_CPPFLAGS = -I$(top_srcdir)/src/cnbismr/

check_PROGRAMS = csmrbars \
				 csmrcbars \
				 csmrevents

csmrbars_SOURCES = csmrbars.cpp
csmrcbars_SOURCES = csmrcbars.cpp
csmrevents_SOURCES = csmrevents.cpp

LDADD = $(top_builddir)/src/cnbismr/libcnbismr.la \
		$(top_builddir)/src/cnbiprotocol/libcnbiprotocol.la \
		 -ldrawtk -lSDL -lcnbicore -lcnbiconfig -lSDL_mixer  -llpttrigger
