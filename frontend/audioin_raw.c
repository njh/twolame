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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "frontend.h"



/* Read in some audio samples into buffer */
static int read_raw( audioin_t* audioin, short *buffer, int samples) 
{
	FILE* file = audioin->file;
	
	if (audioin->samplesize==16) {
		return fread( buffer, 2, samples, file );
	} else {
		fprintf(stderr, "Error: sorry %d-bit samples are not supported at the moment.\n", audioin->samplesize);
	}
	
	// Error
	return -1;
}


/* Return error string (or NULL) */
static const char* error_str_raw( audioin_t* audioin )
{
	FILE* file = audioin->file;
	int error = ferror( file );
	if (error==0) return NULL;
	return strerror(error);
}

static int close_raw( audioin_t* audioin )
{
	FILE* file = audioin->file;
	return fclose( file );
}

audioin_t* open_audioin_raw( char* filename, int samplesize )
{
	audioin_t* audioin = NULL;

	fprintf(stderr, "Debug: Going to open %s using raw, samplesize=%d.\n", filename, samplesize);
	
	// Allocate memory for structure
	audioin = malloc( sizeof( audioin_t ) );
	if (audioin==NULL) {
		fprintf(stderr, "Failed to allocate memory for audioin_t structure.\n");
		exit(ERR_MEM_ALLOC);
	}
	
	// Open the input file by filename
	if (strcmp(filename, "-") == 0) {
		// Use STDIN
		audioin->file = stdin;
	} else {
		audioin->file = fopen(filename, "rb");
	}

	// Check for errors
	if (audioin->file == NULL) {
		fprintf(stderr, "Failed to open input file (%s):\n", filename);
		fprintf(stderr, "  %s\n", strerror(errno));
		exit(ERR_OPENING_INPUT);
	}

	// Fill-in data structure
	audioin->samplesize = samplesize;
	audioin->read = read_raw;
	audioin->error_str = error_str_raw;
	audioin->close = close_raw;


	return audioin;
}
