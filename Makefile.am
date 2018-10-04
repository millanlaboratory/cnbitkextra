ACLOCAL_AMFLAGS=-I m4
SUBDIRS = src/feslanguage \
		  src/cnbiprotocol \
		  src/cnbismr \
		  src/cnbiprotocols/cp \
		  src/cnbiprotocols/errp \
		  src/cnbiprotocols/lc \
		  src/cnbiprotocols/mi \
		  doc \
		  extra/cnbiprotocols 
		  
EXTRA_DIST = debian/changelog \
			 debian/compat \
			 debian/control\
			 debian/copyright \
			 debian/rules \
			 debian/cnbiprotocols.install \
			 debian/libcnbiprotocol-dev.install \
			 debian/libcnbiprotocol.install \
			 debian/libcnbismr-dev.install \
			 debian/libcnbismr.install \
			 debian/libfeslanguage-dev.install \
			 debian/libfeslanguage.install

REMOVE_DISTDIR = \
   { test ! -d "$(distdir)" \
   || { find "$(distdir)" -type d ! -perm -200 -exec chmod u+w {} ';' \
   && rm -fr "$(distdir)"; }; }

dist-deb: distdir
	am__cwd=`pwd` \
	&& cd $(distdir) \
	&& CFLAGS="$(CFLAGS)" CXXFLAGS="$(CXXFLAGS)" FFLAGS="$(FFLAGS)" \
	CPPFLAGS="$(CPPFLAGS)" LDFLAGS="$(LD    FLAGS)" dpkg-buildpackage \
	&& cd "$$am__cwd" 
	$(REMOVE_DISTDIR)
