
AM_CFLAGS = -I../libtwolame/ @SNDFILE_CFLAGS@
AM_LDFLAGS = @SNDFILE_LIBS@


bin_PROGRAMS = twolame

twolame_SOURCES = \
	frontend.c \
	frontend.h \
	audio.c \
	audio.h \
	audio_wave.c \
	audio_aiff.c

twolame_LDADD = $(top_srcdir)/src/libtwolame/libtwolame.la
