#AM_CPPFLAGS = $(CPPFLAGS) -I$(top_srcdir)/src
AM_CXXFLAGS = $(CXXFLAGS) -I$(top_srcdir)/src -std=c++0x

bin_PROGRAMS = lc_triggercheck \
			   lc_eventcheck

lc_triggercheck_SOURCES = lc_triggercheck.cpp
lc_triggercheck_LDADD = $(builddir)/../../cnbismr/libcnbismr.la \
						$(builddir)/../../cnbiprotocol/libcnbiprotocol.la \
						-lcnbicore -lcnbiloop -ltobiid -ldrawtk \
						-lSDL_mixer -llpttrigger -lndf

lc_eventcheck_SOURCES = lc_eventcheck.cpp
lc_eventcheck_LDADD = -lcnbicore -lcnbiconfig -lcnbiloop -lndf -ltobiid
