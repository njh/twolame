
SUBDIRS = src


pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = twolame.pc


# Man Page
man_MANS = doc/twolame.1


# HTML Documention
docdir = $(datadir)/doc
pkgdocdir = $(docdir)/$(PACKAGE)
htmldir = $(docdir)/html
pkghtmldir = $(pkgdocdir)/html

pkghtml_DATA = \
    doc/html/api.html \
    doc/html/authors.html \
    doc/html/changes.txt \
    doc/html/doxygen.css \
    doc/html/doxygen.png \
    doc/html/index.html \
    doc/html/news.html \
    doc/html/psycho.html \
    doc/html/readme.html \
    doc/html/todo.html \
    doc/html/twolame_8h-source.html \
    doc/html/twolame_8h.html \
    doc/html/vbr.html


EXTRA_DIST = bootstrap.sh debian doc twolame.pc.in
