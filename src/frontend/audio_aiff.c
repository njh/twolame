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
#include <math.h>

#include "audio.h"
#include "../config.h"

typedef float Single;



/* AIFF Definitions */

#define IFF_ID_FORM 0x464f524d	/* "FORM" */
#define IFF_ID_AIFF 0x41494646	/* "AIFF" */
#define IFF_ID_COMM 0x434f4d4d	/* "COMM" */
#define IFF_ID_SSND 0x53534e44	/* "SSND" */
#define IFF_ID_MPEG 0x4d504547	/* "MPEG" */

#define AIFF_FORM_HEADER_SIZE 12
#define AIFF_SSND_HEADER_SIZE 16

#define	kFloatLength	4
#define	kDoubleLength	8
#define	kExtendedLength	10

# define FloatToUnsigned(f)	((unsigned long)(((long)((f) - 2147483648.0)) + 2147483647L + 1))
# define UnsignedToFloat(u)	(((double)((long)((u) - 2147483647L - 1))) + 2147483648.0)

static double ConvertFromIeeeExtended (char *bytes)
{
  double f;
  long expon;
  unsigned long hiMant, loMant;

  expon = ((bytes[0] & 0x7F) << 8) | (bytes[1] & 0xFF);
  hiMant = ((unsigned long) (bytes[2] & 0xFF) << 24)
    | ((unsigned long) (bytes[3] & 0xFF) << 16) |
    ((unsigned long) (bytes[4] & 0xFF) << 8) |
    ((unsigned long) (bytes[5] & 0xFF));
  loMant = ((unsigned long) (bytes[6] & 0xFF) << 24)
    | ((unsigned long) (bytes[7] & 0xFF) << 16) |
    ((unsigned long) (bytes[8] & 0xFF) << 8) |
    ((unsigned long) (bytes[9] & 0xFF));

  if (expon == 0 && hiMant == 0 && loMant == 0) {
    f = 0;
  } else {
    if (expon == 0x7FFF) {	/* Infinity or NaN */
      f = HUGE_VAL;
    } else {
      expon -= 16383;
      f = ldexp (UnsignedToFloat (hiMant), expon -= 31);
      f += ldexp (UnsignedToFloat (loMant), expon -= 32);
    }
  }

  if (bytes[0] & 0x80)
    return -f;
  else
    return f;
}

static void ReadBytes (FILE *fp, char *p, int n)
{
  while (!feof (fp) & (n-- > 0))
    *p++ = getc (fp);
}

static int Read16BitsHighLow (FILE *fp)
{
  int first, second, result;

  first = 0xff & getc (fp);
  second = 0xff & getc (fp);

  result = (first << 8) + second;
  return (result);
}

static double ReadIeeeExtendedHighLow (FILE *fp)
{
  char bits[kExtendedLength];

  ReadBytes (fp, bits, kExtendedLength);
  return ConvertFromIeeeExtended (bits);
}

static int Read32BitsHighLow (FILE *fp)
{
  int first, second, result;

  first = 0xffff & Read16BitsHighLow (fp);
  second = 0xffff & Read16BitsHighLow (fp);

  result = (first << 16) + second;
  return (result);
}




/*****************************************************************************
 *
 *  Read Audio Interchange File Format (AIFF) headers.
 *
 *****************************************************************************/

static int aiff_read_headers (FILE * file_ptr, audio_info_t * aiff_ptr)
{
  int chunkSize, subSize, sound_position;
  unsigned long offset;

  if (fseek (file_ptr, 0, SEEK_SET) != 0)
    return -1;

  if (Read32BitsHighLow (file_ptr) != IFF_ID_FORM)
    return -1;

  chunkSize = Read32BitsHighLow (file_ptr);

  if (Read32BitsHighLow (file_ptr) != IFF_ID_AIFF)
    return -1;

  sound_position = 0;
  while (chunkSize > 0) {
    chunkSize -= 4;
    switch (Read32BitsHighLow (file_ptr)) {

    case IFF_ID_COMM:
      chunkSize -= subSize = Read32BitsHighLow (file_ptr);
      aiff_ptr->channels = Read16BitsHighLow (file_ptr);
      subSize -= 2;
      aiff_ptr->samples = Read32BitsHighLow (file_ptr);
      subSize -= 4;
      aiff_ptr->sample_size = Read16BitsHighLow (file_ptr);
      subSize -= 2;
      aiff_ptr->samplerate = (int)ReadIeeeExtendedHighLow (file_ptr);
      subSize -= 10;
      while (subSize > 0) {
	getc (file_ptr);
	subSize -= 1;
      }
      break;

    case IFF_ID_SSND:      
      chunkSize -= subSize = Read32BitsHighLow (file_ptr);
      //aiff_ptr->blkAlgn.offset = Read32BitsHighLow (file_ptr);
      offset = Read32BitsHighLow (file_ptr);
      subSize -= 4;
      //aiff_ptr->blkAlgn.blockSize = Read32BitsHighLow (file_ptr);
      Read32BitsHighLow (file_ptr);
      subSize -= 4;
      sound_position = ftell (file_ptr) + offset; //aiff_ptr->blkAlgn.offset;
      if (fseek (file_ptr, (long) subSize, SEEK_CUR) != 0)
	return -1;
      break;
      
    default:
      chunkSize -= subSize = Read32BitsHighLow (file_ptr);
      while (subSize > 0) {
	getc (file_ptr);
	subSize -= 1;
      }
      break;
    }
  }


  return sound_position;
}



audio_info_t *audio_init_aiff(char *inPath)
{
  audio_info_t *file_info;
  long soundPosition;
  FILE *infile;

  if ( (infile=fopen(inPath, "r")) == NULL) {
    fprintf(stdout,"Error init'ing aiff file\n");
    return(NULL);
  }

  file_info = (audio_info_t *)calloc(1, sizeof(audio_info_t));
  file_info->file = infile;

  if ((soundPosition = aiff_read_headers (infile, file_info)) != -1) {
    fprintf (stderr, ">>> Using Audio IFF sound file headers\n");
    if (fseek (infile, soundPosition, SEEK_SET) != 0) {
      fprintf (stderr, "Could not seek to PCM sound data in \"%s\".\n",
	       inPath);
      fclose(infile);
      free(file_info);
      return(NULL);
    }
    fprintf (stderr, "Parsing AIFF audio file \n");
    fprintf (stderr, ">>> %i Hz sampling frequency selected\n", file_info->samplerate);

    /* Determine number of samples in sound file */
    //    glopts->num_samples = file_info.numChannels * file_info.numSampleFrames;


    return(file_info);
  }

  fclose(infile);
  free(file_info);
  return(NULL);

}

