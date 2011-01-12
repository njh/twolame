dist_check_SCRIPTS = test.pl
dist_check_DATA = testcase-44100.wav testcase-22050.wav

TESTS = $(dist_check_SCRIPTS)
TESTS_ENVIRONMENT = \
	perl -w -Mstrict -MTest::Harness -e "runtests(@ARGV)"

CLEANFILES = *.mp2 *.raw
