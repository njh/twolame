#!/bin/sh
#
# Make HTML documentation pages from asciidoc 
# and header file sources
#
# Requires:
#	asciidoc:	http://www.methods.co.nz/asciidoc/
#	doxygen:	http://www.doxygen.org
#
# Nicholas Humfrey
# September 2004
#

html=./html
asciidoc="asciidoc -f ./asciidoc.conf -b html -d article"
doxygen="doxygen ./doxygen.conf"


# Create HTML documentation from twolame.h
echo "Running doxygen..."
$doxygen

# We don't use these files (single header file)
rm -f $html/index.html
rm -f $html/files.html
rm -f $html/globals*.html


# Create other HTML documentation
echo "Running asciidoc..."
$asciidoc -o $html/index.html ./index.txt
$asciidoc -o $html/readme.html ../README
$asciidoc -o $html/authors.html ../AUTHORS
$asciidoc -o $html/news.html ../NEWS
$asciidoc -o $html/todo.html ../TODO
$asciidoc -o $html/api.html ./api.txt
$asciidoc -o $html/psycho.html ./psycho.txt
$asciidoc -o $html/vbr.html ./vbr.txt


# Create HTML from the ChangeLog... ?
cp ../ChangeLog $html/changes.txt


