/*
 *	TwoLAME: an optimized MPEG Audio Layer Two encoder
 *
 *	Copyright (C) 2004-2007 The TwoLAME Project
 *
 *	This library is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU Lesser General Public
 *	License as published by the Free Software Foundation; either
 *	version 2.1 of the License, or (at your option) any later version.
 *
 *	This library is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *	Lesser General Public License for more details.
 *
 *	You should have received a copy of the GNU Lesser General Public
 *	License along with this library; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  $Id: frontend.h 156 2007-03-20 23:57:35Z nhumfrey $
 *
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sndfile.h>
#include "frontend.h"




/* Read in some audio samples into buffer */
static int read_sndfile( audioin_t* audioin, short *buffer, int samples) 
{
	SNDFILE* file = audioin->file;
	return sf_read_short( file, buffer, samples );
}


/* Return error string (or NULL) */
static const char* error_str_sndfile( audioin_t* audioin )
{
	SNDFILE* file = audioin->file;
	
	if (sf_error(file) == SF_ERR_NO_ERROR) {
		// No error
		return NULL;
	} else {
		// Return error string
		return sf_strerror( file );
	}
}

static int close_sndfile( audioin_t* audioin )
{
	SNDFILE* file = audioin->file;

	return sf_close( file );
}

audioin_t* open_audioin_sndfile( char* filename, SF_INFO *sfinfo )
{
	audioin_t* audioin = NULL;

	fprintf(stderr, "Debug: Going to open %s using libsndfile.\n", filename);
	
	// Allocate memory for structure
	audioin = malloc( sizeof( audioin_t ) );
	if (audioin==NULL) {
		fprintf(stderr, "Failed to allocate memory for audioin_t structure.\n");
		exit(ERR_MEM_ALLOC);
	}
	
	// Open the input file by filename
	audioin->file = sf_open(filename, SFM_READ, sfinfo);

	// Check for errors
	if (audioin->file == NULL) {
		fprintf(stderr, "Failed to open input file (%s):\n", filename);
		fprintf(stderr, "  %s\n", sf_strerror(NULL));
		exit(ERR_OPENING_INPUT);
	}

	// Fill-in data structure
	audioin->samplesize = 0;
	audioin->read = read_sndfile;
	audioin->error_str = error_str_sndfile;
	audioin->close = close_sndfile;


	return audioin;
}
