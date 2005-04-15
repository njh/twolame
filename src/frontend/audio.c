/*
 *  TwoLAME: an optimized MPEG Audio Layer Two encoder
 *
 *  Copyright (C) 2001-2004 Michael Cheng
 *  Copyright (C) 2004-2005 The TwoLAME Project
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *  
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "audio.h"
#include "../config.h"

#ifdef HAVE_SNDFILE
#include <sndfile.h>
#endif


/*****************************************************************************
*
*  Routine to swap bytes in a 16bit word
*
*****************************************************************************/

void audio_byte_swap(short *loc, int words)
{
  int i;
  short thisval;
  char *dst, *src;
  src = (char *) &thisval;
  for (i = 0; i < words; i++) {
    thisval = *loc;
    dst = (char *) loc++;
    dst[0] = src[1];
    dst[1] = src[0];
  }
}


int audio_get_samples (audio_info_t *audio_info, short int *pcmaudio, int numSamples)
{
	FILE *file = audio_info->file;
	int samples_read=0;

	samples_read = fread (pcmaudio, sizeof(short int), numSamples, file);

	if (audio_info->byteswap) {
		audio_byte_swap (pcmaudio, samples_read);
	}

  	return (samples_read/audio_info->channels);
}


audio_info_t *audio_open( char* filename, int numChannels, int sampleRate )
{
	audio_info_t * audio_info=NULL;


#ifdef HAVE_SNDFILE

	/* Use libsndfile if we have it */
	if ( (audio_info = audio_init_sndfile(filename, numChannels, sampleRate)) != NULL ) {
		// Successfully opened with libsndfile
		audio_info->get_samples = audio_get_samples_sndfile;
		audio_info->close = audio_close_sndfile;
	} else {
		// Failed to open
	}

#else

	/* Alternatively use the built-in audio file handlers */
	if ( (audio_info = audio_init_aiff(filename)) != NULL ) {
		// This is an aiff input file
		audio_info->get_samples = audio_get_samples;
		audio_info->close = audio_close;
	} else if ( (audio_info = audio_init_wave(filename)) != NULL) {
		// This is a wave input file 
		audio_info->get_samples = audio_get_samples;
		audio_info->close = audio_close;
	} else if ( (audio_info = audio_init_pcm(filename, numChannels, sampleRate)) != NULL) {
		// This is a raw pcm file
		audio_info->get_samples = audio_get_samples;
		audio_info->close = audio_close;
	} else {
		// failed to open anything
    }
    
#endif

	return audio_info;
}


audio_info_t *audio_init_pcm(char *filename, int numChannels, int sampleRate)
{
  
	audio_info_t *audio_info;
	FILE *soundfile;

	if ( (soundfile = fopen(filename, "r")) == NULL) {
		fprintf(stderr, "PCM: Can't open file\n");
		return(NULL);
	}

	audio_info = (audio_info_t *)calloc(1, sizeof(audio_info_t));
	audio_info->file = soundfile;
	audio_info->channels = numChannels;
	audio_info->samplerate = sampleRate;
	audio_info->samples = -1; // UNKNOWN. i suppose you could
								// stat the file and have a guess. MFC May 03.

	return(audio_info);
}

/* Free the memory associated with the input buffer */
void audio_close( audio_info_t** audio_info ) {

    if ((*audio_info)->file) {
    	fclose( (FILE*)(*audio_info)->file );
    }
    
	free( *audio_info );
	*audio_info = NULL;
}




/* Function to handle libsndfile - if we have it */

#ifdef HAVE_SNDFILE


/* Display information about the input file */
static void display_sndfile_info( SNDFILE *sndfile, SF_INFO *info )
{
	SF_FORMAT_INFO format_info;

	format_info.format = info->format;
	sf_command (sndfile, SFC_GET_FORMAT_INFO, &format_info, sizeof (format_info)) ;

	fprintf(stderr, ">>> Input File Format: %s\n", format_info.name );
	fprintf(stderr, ">>> Sample Rate: %d Hz\n", info->samplerate );
	fprintf(stderr, ">>> Channels: %d\n", info->channels );
}

audio_info_t *audio_init_sndfile( char *filename, int numChannels, int sampleRate  )
{
	audio_info_t * audio_info = NULL;
	SNDFILE *sndfile = NULL;
	SF_INFO	sf_info;
	
	/* Setup Info */
	memset( &sf_info, 0, sizeof( sf_info ));
	sf_info.samplerate = sampleRate;
	sf_info.channels = numChannels;

	
	/* Open the file */
	sndfile = sf_open( filename, SFM_READ, &sf_info);
	if (!sndfile) {
		fprintf(stderr, "Failed to open file %s.\n", filename);
		fprintf(stderr, "%s\n", sf_strerror(sndfile));
		return NULL;
	}
	
	display_sndfile_info( sndfile, &sf_info );
	
	
	/* FIXME - ensure the file format is supported by toolame */
	
	audio_info = (audio_info_t *)calloc(1, sizeof(audio_info_t));
	audio_info->file = sndfile;
	audio_info->channels = sf_info.channels;
	audio_info->samplerate = sf_info.samplerate;
	audio_info->samples = -1;
	
	return audio_info;
}

int audio_get_samples_sndfile(audio_info_t *audio_info, short int *pcmaudio, int numSamples)
{
	SNDFILE *sndfile = (SNDFILE *)audio_info->file;
	int samples_read = 0;

	samples_read = sf_read_short( sndfile, pcmaudio, numSamples );

  	return (samples_read/audio_info->channels);
}


void
audio_close_sndfile( audio_info_t** audio_info  )
{
	SNDFILE *sndfile = (SNDFILE *)(*audio_info)->file;

	sf_close( sndfile );
	(*audio_info)->file = NULL;
	
	audio_close( audio_info );
}


#endif


