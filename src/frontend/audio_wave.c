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

/*
  parse the wave header.
  returns NULL if not open failed
  returns a audio_info_t * if wave header successfully parsed.
  needs to fill in : samplerate + numchannels
  
  POST: audio_info->soundfile is set to be at the start of the
        PCM audio data 
*/

audio_info_t *audio_init_wave (char *inPath)
{
  unsigned char wave_header_buffer[40];	//HH fixed
  int wave_header_read = 0;
  int wave_header_stereo = -1;
  int wave_header_16bit = -1;
  unsigned long samplerate;


  audio_info_t *audio_info = NULL;
  FILE *musicin;

  if ( (musicin = fopen(inPath, "r"))==NULL) {
    fprintf(stdout,"WAV: cannot open input file: %s\n", inPath);
    return(NULL);
  }


  /**************************** WAVE *********************************/
  /*   Nick Burch <The_Leveller@newmail.net> */
  /*********************************/
  /* Wave File Headers:   (Dec)    */
  /* 8-11 = "WAVE"                 */
  /* 22 = Stereo / Mono            */
  /*       01 = mono, 02 = stereo  */
  /* 24 = Sampling Frequency       */
  /* 32 = Data Rate                */
  /*       01 = x1 (8bit Mono)     */
  /*       02 = x2 (8bit Stereo or */
  /*                16bit Mono)    */
  /*       04 = x4 (16bit Stereo)  */
  /*********************************/

  fseek (musicin, 0, SEEK_SET);
  fread (wave_header_buffer, 1, 40, musicin);

  if (wave_header_buffer[8] == 'W' && wave_header_buffer[9] == 'A'
      && wave_header_buffer[10] == 'V' && wave_header_buffer[11] == 'E') {
    fprintf (stderr, "Parsing Wave File Header\n");

#ifdef WORDS_BIGENDIAN
	samplerate = wave_header_buffer[27] +
		(wave_header_buffer[26] << 8) +
		(wave_header_buffer[25] << 16) +
		(wave_header_buffer[24] << 24);
#else
      samplerate = *(unsigned long *) (&wave_header_buffer[24]);
#endif


    /* Wave File */
    wave_header_read = 1;
    switch (samplerate) {
    case 44100:
    case 48000:
    case 32000:
    case 24000:
    case 22050:
    case 16000:
      fprintf (stderr, ">>> %ld Hz sampling freq selected\n", samplerate);
      break;
    default:
      /* Unknown Unsupported Frequency */
      fprintf (stderr, ">>> Unknown samp freq %ld Hz in Wave Header\n",
	       samplerate);
      fprintf (stderr, ">>> Default 44.1 kHz samp freq selected\n");
      samplerate = 44100;
    }

    if ((long) wave_header_buffer[22] == 1) {
      fprintf (stderr, ">>> Input Wave File is Mono\n");
      wave_header_stereo = 0;
    }
    if ((long) wave_header_buffer[22] == 2) {
      fprintf (stderr, ">>> Input Wave File is Stereo\n");
      wave_header_stereo = 1;
    }
    if ((long) wave_header_buffer[32] == 1) {
      fprintf (stderr, ">>> Input Wave File is 8 Bit\n");
      wave_header_16bit = 0;
      fprintf (stderr, "Input File must be 16 Bit! Please Re-sample");
      fclose(musicin); return(NULL);
    }
    if ((long) wave_header_buffer[32] == 2) {
      if (wave_header_stereo == 1) {
	fprintf (stderr, ">>> Input Wave File is 8 Bit\n");
	wave_header_16bit = 0;
	fprintf (stderr, "Input File must be 16 Bit! Please Re-sample");
	fclose(musicin); return(NULL);
      } else {
	/* fprintf(stderr,  ">>> Input Wave File is 16 Bit\n" ); */
	wave_header_16bit = 1;
      }
    }
    if ((long) wave_header_buffer[32] == 4) {
      /* fprintf(stderr,  ">>> Input Wave File is 16 Bit\n" ); */
      wave_header_16bit = 1;
    }
    /* should probably use the wave header to determine size here FIXME MFC Feb 2003 */
    if (fseek (musicin, 44, SEEK_SET) != 0) {	/* there's a way of calculating the size of the
						   wave header. i'll just jump 44 to start with */
      fprintf (stderr, "Could not seek to PCM sound data in \"%s\".\n",
	       inPath);
      fclose(musicin); return(NULL);
    }

    // Successfully processed the wave header
    audio_info = (audio_info_t *)calloc(1, sizeof(audio_info_t));
    audio_info->file = musicin;
    if (wave_header_stereo == 1)
      audio_info->channels = 2;
    else
      audio_info->channels = 1;
    audio_info->samplerate = samplerate;
    audio_info->samples = -1; // UNKNOWN. But we really should be able to work 
                                 // it out. FIX THIS. MFC May03.
    return(audio_info);
  }

  // not a wave file
  fclose(musicin);  return(NULL);
}



