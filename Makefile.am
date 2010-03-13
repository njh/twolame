
AUTOMAKE_OPTIONS = foreign

SUBDIRS = libtwolame frontend simplefrontend doc

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = twolame.pc

INDENT = indent -npro -kr -nsob -fca -blf -ip1 -hnl -l100 -lc100 -nut -ts4 -v
indent:
	find $(top_srcdir) -type f -name "*.[ch]" -print -exec $(INDENT) {} ';'

EXTRA_DIST = \
	autogen.sh \
	twolame.pc.in \
	win32/configwin.h \
	win32/libtwolame_dll.sln \
	win32/libtwolame_dll.vcproj \
	win32/libtwolame_static.sln \
	win32/libtwolame_static.vcproj \
	win32/winutil.h
