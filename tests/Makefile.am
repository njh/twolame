dist_check_SCRIPTS = test.pl
dist_check_DATA = testcase.wav

TESTS = $(dist_check_SCRIPTS)
TESTS_ENVIRONMENT = \
	TWOLAME_CMD="$(top_builddir)/frontend/twolame" \
	perl -w -Mstrict -MTest::Harness -e "runtests(@ARGV)"

CLEANFILES = *.mp2
