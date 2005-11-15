
## Man Page
man_MANS = twolame.1

twolame.1.xml: twolame.1.txt
	asciidoc -d manpage -b docbook -o twolame.1.xml twolame.1.txt

twolame.1: twolame.1.xml
	xmlto man twolame.1.xml




## Building HTML documentation Requires:
##    asciidoc:	http://www.methods.co.nz/asciidoc/
##    doxygen:	http://www.doxygen.org
##

htmldir=$(srcdir)/html
asciidoc=asciidoc -b xhtml11 \
	-a revision="@PACKAGE_VERSION@" \
	-a theme=twolame \
	-a linkcss \
	-a stylesdir=. 
doxygen=doxygen doxygen.conf

build-html:
	rm -f $(htmldir)/*.html
	$(doxygen)
	rm -f $(htmldir)/index.html $(htmldir)/files.html $(htmldir)/globals*.html
	$(asciidoc) -o $(htmldir)/index.html $(srcdir)/index.txt
	$(asciidoc) -o $(htmldir)/readme.html $(top_srcdir)/README
	$(asciidoc) -o $(htmldir)/authors.html $(top_srcdir)/AUTHORS
	$(asciidoc) -o $(htmldir)/changelog.html $(top_srcdir)/ChangeLog
	$(asciidoc) -o $(htmldir)/todo.html $(top_srcdir)/TODO
	$(asciidoc) -o $(htmldir)/api.html $(srcdir)/api.txt
	$(asciidoc) -o $(htmldir)/psycho.html $(srcdir)/psycho.txt
	$(asciidoc) -o $(htmldir)/vbr.html $(srcdir)/vbr.txt
	$(asciidoc) -d manpage -o $(htmldir)/twolame.1.html $(srcdir)/twolame.1.txt


## Install HTML Documention
docdir = $(datadir)/doc
pkgdocdir = $(docdir)/$(PACKAGE)
pkghtmldir = $(pkgdocdir)/html

pkghtml_DATA = \
    $(htmldir)/api.html \
    $(htmldir)/authors.html \
    $(htmldir)/doxygen.css \
    $(htmldir)/doxygen.png \
    $(htmldir)/index.html \
    $(htmldir)/news.html \
    $(htmldir)/psycho.html \
    $(htmldir)/readme.html \
    $(htmldir)/todo.html \
    $(htmldir)/twolame_8h-source.html \
    $(htmldir)/twolame_8h.html \
    $(htmldir)/twolame.1.html \
    $(htmldir)/vbr.html


EXTRA_DIST = $(man_MANS) $(pkghtml_DATA) \
	dist10-text \
	api.txt \
	asciidoc.conf \
	doxygen.conf \
	index.txt \
	psycho.txt \
	twolame.1.txt \
	twolame.1.xml \
	vbr.txt
