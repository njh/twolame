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
#include <unistd.h>
#include <getopt.h>

#include <twolame.h>
#include <sndfile.h>


/*
  Constants
*/
#define MP2BUFSIZE		(16384)
#define AUDIOBUFSIZE	(9210)
#define MAX_NAME_SIZE	(256)


/*
  Result codes
*/
#define ERR_NO_ERROR		(0)		// No Error (encoded ok)
#define ERR_NO_ENCODE		(1)		// No Error (no encoding performed)
#define ERR_OPENING_INPUT	(2)		// Error opening input file
#define ERR_OPENING_OUTPUT	(4)		// Error opening output file
#define ERR_MEM_ALLOC		(6)		// Error allocating memory
#define ERR_INVALID_PARAM	(8)		// Error in chosen parameters
#define ERR_READING_INPUT	(10)	// Error reading input
#define ERR_ENCODING		(12)	// Error occured during encoding
#define ERR_WRITING_OUTPUT	(14)	// Error occured writing to output file


/* 
  Global Variables
*/
int single_frame_mode = FALSE;		// only encode a single frame of MPEG audio ?
int downmix = FALSE;				// downmix from stereo to mono ?
int byteswap = FALSE;				// swap endian on input audio ?
int channelswap = FALSE;			// swap left and right channels ?
SF_INFO	sfinfo = {0,0,0,0,0,0};		// contains information about input file format

char inputfilename[MAX_NAME_SIZE] = "go_away.aiff";
char outputfilename[MAX_NAME_SIZE] = "go_away.mp2";





/* 
  new_extension()
  Puts a new extension name on a file name <filename>.
  Removes the last extension name, if any.
*/
static void
new_extension(char *filename, char *extname, char *newname)
{
	int             found, dotpos;

	/* First, strip the extension */
	dotpos = strlen(filename);
	found = 0;
	do {
		switch (filename[dotpos]) {
		case '.':
			found = 1;
			break;
		case '\\':
		case '/':
		case ':':
			found = -1;
			break;
		default:
			dotpos--;
			if (dotpos < 0)
				found = -1;
			break;
		}
	} while (found == 0);

	if (found == -1) {
		strcpy(newname, filename);
	}
	if (found == 1) {
		strncpy(newname, filename, dotpos);
		newname[dotpos] = '\0';
	}
	strcat(newname, extname);
}



/* 
  print_file_config() 
  Display information about input and output files
*/
static void
print_file_config( SNDFILE *inputfile )
{
	SF_FORMAT_INFO format_info;

	format_info.format = sfinfo.format;
	sf_command (inputfile, SFC_GET_FORMAT_INFO, &format_info, sizeof (format_info)) ;

	fprintf(stderr, "---------------------------------------------------------\n");
	fprintf(stderr, "Input File Name: %s\n", inputfilename );
	fprintf(stderr, "Input File Format: %s\n", format_info.name );
	fprintf(stderr, "Output File Name: %s\n", outputfilename );
	fprintf(stderr, "---------------------------------------------------------\n");

}


/* 
  usage() 
  Display the extended usage information
*/
static void
usage_long()
{
	fprintf(stdout, "\nTwoLAME version %s (%s)\n", get_twolame_version(), get_twolame_url());
	fprintf(stdout, "MPEG Audio Layer II encoder\n\n");
	fprintf(stdout, "usage: \n");
	fprintf(stdout, "\ttwolame [options] <input> <output>\n\n");

	fprintf(stdout, "Options:\n");
	fprintf(stdout, "Input\n");
	fprintf(stdout, "\t-r, --raw-input          input is raw pcm\n");
	fprintf(stdout, "\t-x, --byte-swap          force byte-swapping of input\n");
	fprintf(stdout, "\t-s, --samplerate sfreq   sampling frequency of raw input (kHz)\n");
	fprintf(stdout, "\t-N, --channels nch       number of channels in raw input\n");
	fprintf(stdout, "\t-a, --downmix            downmix input from stereo to mono\n");
	fprintf(stdout, "\t-g, --swap-channels      swap channels of input file\n");

    fprintf(stdout, "\t    --scale value        scale input (multiply PCM data)\n");
    fprintf(stdout, "\t    --scale-l value      scale channel 0 (left) input (multiply PCM data)\n");
    fprintf(stdout, "\t    --scale-r value      scale channel 1 (right) input (multiply PCM data)\n");

	
	fprintf(stdout, "Output\n");
	fprintf(stdout, "\t-m, --mode mode          (s)tereo, (j)oint, (m)ono or (a)uto\n");
	fprintf(stdout, "\t-p, --psy-mode psy       psychoacoustic model 0/1/2/3 (default 3)\n");
	fprintf(stdout, "\t-b, --bitrate br         total bitrate in kbps (dflt 192)\n");
	fprintf(stdout, "\t-v, --vbr lev            vbr mode\n");
	fprintf(stdout, "\t-l, --eth lev            ATH level (dflt 0)\n");
	fprintf(stdout, "\t-q, --quick num          quick mode. only calculate psy model every num frames\n");
		
	fprintf(stdout, "Misc\n");
	fprintf(stdout, "\t-e, --deemphasis emp     de-emphasis n/5/c (default: (n)one)\n");
	fprintf(stdout, "\t-c, --copyright          mark as copyright\n");
	fprintf(stdout, "\t-o, --original           mark as original\n");
	fprintf(stdout, "\t-p, --error-protect      enable error protection\n");
	fprintf(stdout, "\t-r, --padding            force padding bit/frame on\n");
	fprintf(stdout, "\t-B, --max-bitrate rate   set the upper bitrate when in VBR mode\n");
	fprintf(stdout, "\t-R, --reserve-bits num   set the number of reserved bits at the end of frame\n");
	fprintf(stdout, "\t-E, --energy             turn on energy level extensions\n");
	
	
	fprintf(stdout, "Verbosity\n");
	fprintf(stdout, "\t-t, --talkativity num    talkativity 0-10 (default is 1)\n");
	fprintf(stdout, "\t    --quiet    			same as --talkativity=0\n");
	fprintf(stdout, "\t    --brief       		same as --talkativity=2\n");
	fprintf(stdout, "\t    --verbose    		same as --talkativity=4\n");
	


	fprintf(stdout, "Files\n");
	fprintf(stdout, "\tinput       input sound file (any format supported by libsndfile)\n");
	fprintf(stdout, "\toutput      output bit stream of encoded audio\n");
	
	fprintf(stdout, "\n\tAllowable bitrates for 16, 22.05 and 24kHz sample input\n");
	fprintf(stdout, "\t   8,  16,  24,  32,  40,  48,  56,  64,  80,  96, 112, 128, 144, 160\n");
	fprintf(stdout, "\n\tAllowable bitrates for 32, 44.1 and 48kHz sample input\n");
	fprintf(stdout, "\t  32,  48,  56,  64,  80,  96, 112, 128, 160, 192, 224, 256, 320, 384\n");
	fprintf(stdout, "brate indx 1    2    3    4    5    6    7    8    9   10   11   12   13   14\n");

	fprintf(stdout, "\n");
	exit(ERR_NO_ENCODE);
}



/* 
  usage_short() 
  Display the short usage information
*/
void
usage_short()
{
	/* print a bit of info about the program */
	fprintf(stdout, "TwoLAME version %s (%s)\n", get_twolame_version(), get_twolame_url());
	fprintf(stderr, "MPEG Audio Layer II encoder\n\n");
	fprintf(stderr, "USAGE: twolame [options] <infile> [outfile]\n\n");
	fprintf(stderr, "Try \"twolame --help\" for more information.\n");
	exit(ERR_NO_ENCODE);
}




/* 
  parse_args() 
  Parse the command line arguments
*/
void
parse_args(int argc, char **argv, twolame_options * encopts )
{

	// process args
	static struct option longopts[] = {
		{ "raw-input",		no_argument,			NULL,		'r' },
		{ "byte-swap",		no_argument,			NULL,		'x' },
		{ "samplerate",		required_argument,		NULL,		's' },
		{ "channels",		required_argument,		NULL,		'N' },
		{ "swap-channels",	no_argument,			NULL,		'g' },
		{ "downmix",		no_argument,			NULL,		'a' },
		
		//{ "scale",		required_argument,		NULL,			'' },
		//{ "scale-l",	required_argument,		NULL,			'' },
		//{ "scale-r",	required_argument,		NULL,			'' },
		
		
		
		{ NULL,         0,                      NULL,			0 }
	};
	
	
	//http://www.gnu.org/software/libc/manual/html_node/Getopt-Long-Options.html
	//http://www.gnu.org/software/libc/manual/html_node/Getopt-Long-Option-Example.html
	
	if (0) {
	usage();
	new_extension( NULL, NULL, NULL);
	}
}


SNDFILE*
open_input_file( char* filename )
{
	SNDFILE* file = NULL;


	// Do they want STDIN ?
	if (strncmp( filename, "-", 1 )==0) {
		/// *** do stuff here ***
		// sf_open_fd()
		fprintf(stderr, "reading from stdin does not work yet.\n");
		exit(ERR_OPENING_INPUT);
	}

	
	// Open the input file
	file = sf_open(filename, SFM_READ, &sfinfo);
	
	// Check for errors
	if (file == NULL) {
		fprintf(stderr,"failed to open input file\n");
		exit(ERR_OPENING_INPUT);
	}
	
	return file;
}


FILE*
open_output_file( char* filename )
{
	FILE* file;

	
	// Do they want STDOUT ?
	if (strncmp( filename, "-", 1 )==0) {
		file = stdout;
	} else {
		file = fopen(filename, "w");
	}
	
	// Check for errors
	if (file == NULL) {
		perror("failed to open output file");
		exit(ERR_OPENING_OUTPUT);
	}

	return file;
}


int
main(int argc, char **argv)
{
	twolame_options	*encopts = NULL;
	SNDFILE			*inputfile = NULL;
	FILE			*outputfile = NULL;
	short int		*pcmaudio = NULL;
	int				samples_read = 0;
	unsigned int	frame_count = 0;
	unsigned char	*mp2buffer = NULL;
	int    			mp2fill_size = 0;
	int				audioReadSize = 0;

	usage_long();

	if (argc == 1)
		short_usage();

	// Allocate memory for the PCM audio data
	if ((pcmaudio = (short int *) calloc(AUDIOBUFSIZE, sizeof(short int))) == NULL) {
		fprintf(stderr, "Error: pcmaudio memory allocation failed\n");
		exit(ERR_MEM_ALLOC);
	}
	
	// Allocate memory for the encoded MP2 audio data
	if ((mp2buffer = (unsigned char *) calloc(MP2BUFSIZE, sizeof(unsigned char))) == NULL) {
		fprintf(stderr, "Error: mp2buffer memory allocation failed\n");
		exit(ERR_MEM_ALLOC);
	}
	
	// Initialise Encoder Options Structure 
	encopts = twolame_init();
	if (encopts == NULL) {
		fprintf(stderr, "Error: initializing libtwolame encoder failed.\n");
		exit(ERR_MEM_ALLOC);
	}

	
	// Get options and parameters from the command line
	parse_args(argc, argv, encopts);


	// Open the input file
	inputfile = open_input_file( inputfilename );
	twolame_set_num_channels( encopts, sfinfo.channels );
	twolame_set_in_samplerate( encopts, sfinfo.samplerate );
		
	// Open the output file
	outputfile = open_output_file( outputfilename );
	
	// display file settings
	print_file_config( inputfile );


	//
	// If energy information is required, see if we're in MONO mode in
	// which case, we only need 16 bits of ancillary data
	//
	//if (twolame_get_energy_levels(encodeOptions))
	//	if (twolame_get_mode(encodeOptions) == TWOLAME_MONO)
	//		twolame_set_num_ancillary_bits(encodeOptions, 16);
	// only 2 bytes needed for energy level for mono channel


	// initialise twolame with this set of options
	if (twolame_init_params( encopts ) != 0) {
		fprintf(stderr, "Error: configuring libtwolame encoder failed.\n");
		exit(ERR_INVALID_PARAM);
	}

	// display encoder settings
	twolame_print_config( encopts );


	if (single_frame_mode) {
		audioReadSize = 1152;
	} else {
		audioReadSize = AUDIOBUFSIZE;
	}


	// Now do the reading/encoding/writing
	while ((samples_read = sf_read_short( inputfile, pcmaudio, audioReadSize )) > 0) {
		int bytes_out = 0;
		
		// Calculate the number of samples we have (per channel)
		samples_read /= sfinfo.channels;

		// Encode the audio to MP2
		mp2fill_size = twolame_encode_buffer_interleaved( encopts, pcmaudio, samples_read, mp2buffer, MP2BUFSIZE);
		
		// Stop if we don't have any bytes (probably don't have enough audio for a full frame of mpeg audio)
		if (mp2fill_size==0) break;
		if (mp2fill_size<0) {
			fprintf(stderr,"error while encoding audio: %d\n", mp2fill_size);
			exit(ERR_WRITING_OUTPUT);
		}

		// Write the encoded audio out
		bytes_out = fwrite(mp2buffer, sizeof(unsigned char), mp2fill_size, outputfile);
		if (bytes_out<=0) {
			perror("error while writing to output file");
			exit(ERR_WRITING_OUTPUT);
		}
		
		
		// Display Progress
		frame_count += (samples_read / 1152);
		fprintf(stderr, "[%04i/%04i]\r", frame_count, (int)(sfinfo.frames / sfinfo.channels  / 1152));
		fflush(stderr);
	}

	//
	// flush any remaining audio. (don't send any new audio data) There
	// should only ever be a max of 1 frame on a flush. There may be zero
	// frames if the audio data was an exact multiple of 1152
	//
	mp2fill_size = twolame_encode_flush( encopts, mp2buffer, MP2BUFSIZE);
	if (mp2fill_size>0) {
		int bytes_out = fwrite(mp2buffer, sizeof(unsigned char), mp2fill_size, outputfile);
		if (bytes_out<=0) {
			perror("error while writing to output file");
			exit(ERR_WRITING_OUTPUT);
		}
	}
	
	// Close input and output files
	sf_close( inputfile );
	fclose( outputfile );

	// Close the libtwolame encoder
	twolame_close(&encopts);
	

	fprintf(stderr, "\nEncoding Finished.\n");
	return (ERR_NO_ERROR);
}

