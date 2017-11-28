ACLOCAL_AMFLAGS = -I build-scripts

SUBDIRS = libtwolame frontend simplefrontend doc tests

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = twolame.pc

INDENT = indent -npro -kr -nsob -fca -blf -ip1 -hnl -l100 -lc100 -nut -ts4 -v
indent:
	find $(top_srcdir) -type f -name "*.[ch]" -print -exec $(INDENT) {} ';'

# Copy README.md to README and NEWS.md to NEWS when building distribution
README:
	[ -f README.md ] && fgrep -v "Build Status]" $(top_srcdir)/README.md > $@ || true

NEWS:
	[ -f NEWS.md ] && cat $(top_srcdir)/NEWS.md > $@ || true

ChangeLog:
	[ -d .git ] && git log > ChangeLog || true

EXTRA_DIST = \
	autogen.sh \
	twolame.pc.in \
	twolame.spec \
	win32/configwin.h \
	win32/libtwolame_dll.sln \
	win32/libtwolame_dll.vcproj \
	win32/libtwolame_static.sln \
	win32/libtwolame_static.vcproj \
	win32/winutil.h

test: check
