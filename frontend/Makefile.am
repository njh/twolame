
AM_CFLAGS = -I ../build/ -I../libtwolame/ @SNDFILE_CFLAGS@
AM_LDFLAGS = @SNDFILE_LIBS@


bin_PROGRAMS = @TWOLAME_BIN@
EXTRA_PROGRAMS = twolame

twolame_SOURCES = frontend.c
twolame_LDADD = $(top_srcdir)/libtwolame/libtwolame.la
