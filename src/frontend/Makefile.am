
AM_CFLAGS = -I../libtwolame/ @SNDFILE_CFLAGS@
AM_LDFLAGS = @SNDFILE_LIBS@


bin_PROGRAMS = @TWOLAME_BIN@
EXTRA_PROGRAMS = twolame

twolame_SOURCES = twolame_frontend.c
twolame_LDADD = $(top_srcdir)/src/libtwolame/libtwolame.la
