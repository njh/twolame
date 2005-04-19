
# Man Page
man_MANS = twolame.1



## Building HTML Requires:
##    asciidoc:	http://www.methods.co.nz/asciidoc/
##    doxygen:	http://www.doxygen.org
##

htmldir=$(srcdir)/html
asciidoc=asciidoc -f asciidoc.conf -b html -d article
doxygen=doxygen doxygen.conf

build-html:
	$(doxygen)
	rm -f $(htmldir)/index.html $(htmldir)/files.html $(htmldir)/globals*.html
	$(asciidoc) -o $(htmldir)/index.html $(srcdir)/index.txt
	$(asciidoc) -o $(htmldir)/readme.html $(top_srcdir)/README
	$(asciidoc) -o $(htmldir)/authors.html $(top_srcdir)/AUTHORS
	$(asciidoc) -o $(htmldir)/news.html $(top_srcdir)/NEWS
	$(asciidoc) -o $(htmldir)/todo.html $(top_srcdir)/TODO
	$(asciidoc) -o $(htmldir)/api.html $(srcdir)/api.txt
	$(asciidoc) -o $(htmldir)/psycho.html $(srcdir)/psycho.txt
	$(asciidoc) -o $(htmldir)/vbr.html $(srcdir)/vbr.txt


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
    $(htmldir)/vbr.html


EXTRA_DIST = $(man_MANS) $(pkghtml_DATA) \
	api.txt \
	asciidoc.conf \
	doxygen.conf \
	index.txt \
	psycho.txt \
	vbr.txt
