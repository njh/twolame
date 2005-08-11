#!/bin/sh
#

run_cmd() {
    echo running $* ...
    if ! $*; then
			echo failed!
			exit 1
    fi
}

# Create the (empty) build directory if it doesn't exist
if ! [ -d build ]; then
	echo "creating build directory"
	mkdir build
fi

run_cmd aclocal
run_cmd autoheader
run_cmd libtoolize --force --copy
run_cmd automake --add-missing --copy
run_cmd autoconf

echo
echo "Now type './configure' to configure TwoLAME"
echo
