AM_CXXFLAGS = $(CXXFLAGS) -std=c++0x

lib_LTLIBRARIES = libcnbiprotocol.la

libcnbiprotocol_includedir=$(includedir)/cnbiprotocol

libcnbiprotocol_include_HEADERS = CpAudio.hpp \
								  CpPlayer.hpp \
								  CpRandInt.hpp \
								  CpSound.hpp \
								  CpTrials.hpp \
								  CpTrack.hpp \
								  CpTriggerBeep.hpp \
								  CpTrigger.hpp \
								  CpTriggerLPT.hpp \
								  CpTriggerSerial.hpp \
								  CpTriggerFactory.hpp \
								  CpProbability.hpp \
								  CpDynamicP2.hpp \
								  cp_utilities.hpp

libcnbiprotocol_la_SOURCES = CpAudio.cpp \
							 CpAudio.hpp \
							 CpPlayer.cpp \
							 CpPlayer.hpp \
							 CpRandInt.cpp \
							 CpRandInt.hpp \
							 CpSound.cpp \
							 CpSound.hpp \
							 CpTrials.cpp \
							 CpTrials.hpp \
							 CpTrack.cpp \
							 CpTrack.hpp \
							 CpTriggerBeep.cpp \
							 CpTriggerBeep.hpp \
							 CpTrigger.cpp \
							 CpTrigger.hpp \
							 CpTriggerLPT.cpp \
							 CpTriggerLPT.hpp \
							 CpTriggerSerial.cpp \
							 CpTriggerSerial.hpp \
							 CpTriggerFactory.cpp \
							 CpTriggerFactory.hpp \
							 CpProbability.cpp \
							 CpProbability.hpp \
							 CpDynamicP2.cpp \
							 CpDynamicP2.hpp

#libcnbiprotocol_la_LIBADD = -lSDL_mixer -llpttrigger -lcnbicore
#libcnbiprotocol_la_LDFLAGS = $(AM_CFLAGS) -lSDL_mixer -llpttrigger -lcnbicore \
#							 -version-info $(CURRENT):$(REVISION):$(AGE)
libcnbiprotocol_la_LIBADD = -lSDL -lSDL_mixer -llpttrigger -lcnbicore -lcnbiconfig -ltobiic
libcnbiprotocol_la_LDFLAGS = $(AM_CFLAGS) \
							 -version-info $(CURRENT):$(REVISION):$(AGE)

bin_PROGRAMS = cp_triggersend
cp_triggersend_SOURCES = cp_triggersend.cpp
cp_triggersend_LDADD = libcnbiprotocol.la \
				-lcnbicore \
			   $(LDADD)
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = cnbiprotocol.pc
