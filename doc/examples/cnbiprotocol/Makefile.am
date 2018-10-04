AM_CPPFLAGS = -I$(top_srcdir)/src/cnbiprotocol/

check_PROGRAMS = cpaudio \
				 cpplayer \
				 cpsound \
				 cptrials \
				 cptrack \
				 cptriggerbeep \
				 cptriggerlpt \
				 cptriggerfactory \
				 cpprobability \
				 cpdynamicp2

cpaudio_SOURCES = cpaudio.cpp
cpplayer_SOURCES = cpplayer.cpp
cpsound_SOURCES = cpsound.cpp
cptrials_SOURCES = cptrials.cpp
cptrack_SOURCES = cptrack.cpp
cptriggerbeep_SOURCES = cptriggerbeep.cpp
cptriggerlpt_SOURCES = cptriggerlpt.cpp
cptriggerfactory_SOURCES = cptriggerfactory.cpp
cpprobability_SOURCES = cpprobability.cpp
cpdynamicp2_SOURCES = cpdynamicp2.cpp

LDADD = $(top_builddir)/src/cnbiprotocol/libcnbiprotocol.la \
		-lSDL_mixer -llpttrigger -lcnbicore -lcnbiconfig -ltobiic
