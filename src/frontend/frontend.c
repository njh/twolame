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

#include "config.h"


#define MP2BUFSIZE		(16384)
#define AUDIOBUFSIZE	(9210)
#define MAX_NAME_SIZE	(256)


typedef struct frontend_options_struct {
	int             single_frame_mode;
	int             downmix;
	int             byteswap;
	int             channelswap;
	int             samplerate;
	int             channels;
}
frontend_options;


frontend_options *
frontend_init(void)
{
	frontend_options *frontOptions;

	frontOptions = (frontend_options *) calloc(1, sizeof(frontend_options));
	frontOptions->single_frame_mode = FALSE;
	frontOptions->downmix = FALSE;
	frontOptions->byteswap = FALSE;
	frontOptions->channelswap = FALSE;
	frontOptions->samplerate = 44100;
	frontOptions->channels = 2;
	return (frontOptions);
}



/* 
  new_ext()
  Puts a new extension name on a file name <filename>.
  Removes the last extension name, if any.
*/
static void
new_ext(char *filename, char *extname, char *newname)
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


void
usage(void)
{
	fprintf(stdout, "\nTwoLAME version %s (http://twolame.sourceforge.net)\n", PACKAGE_VERSION);
	fprintf(stdout, "MPEG Audio Layer II encoder\n\n");
	fprintf(stdout, "usage: \n");
	fprintf(stdout, "\ttwolame [options] <input> <output>\n\n");

	fprintf(stdout, "Options:\n");
	fprintf(stdout, "Input\n");
	fprintf(stdout, "\t-r          input is raw pcm\n");
	fprintf(stdout, "\t-x          force byte-swapping of input\n");
	fprintf(stdout, "\t-s sfreq    sampling frequency of raw input (kHz) (default 44.1)\n");
	fprintf(stdout, "\t-N nch      number of channels for raw input (default 2)\n");
	fprintf(stdout, "\t-g          swap channels of input file\n");
	fprintf(stdout, "\t-a          downmix from stereo to mono\n");

    //--scale <arg>   scale input (multiply PCM data) by <arg>
    //--scale-l <arg> scale channel 0 (left) input (multiply PCM data) by <arg>
    //--scale-r <arg> scale channel 1 (right) input (multiply PCM data) by <arg>

	
	fprintf(stdout, "Output\n");
	fprintf(stdout, "\t-m mode     (s)tereo, (j)oint, (m)ono or (a)uto\n");
	fprintf(stdout, "\t-p psy      psychoacoustic model 0/1/2/3 (dflt 3)\n");
	fprintf(stdout, "\t-b br       total bitrate in kbps    (dflt 192)\n");
	fprintf(stdout, "\t-v lev      vbr mode\n");
	fprintf(stdout, "\t-l lev      ATH level (dflt 0)\n");
	
	fprintf(stdout, "Operation\n");
	fprintf(stdout, "\t-q num      quick mode. only calculate psy model every num frames\n");
		
	fprintf(stdout, "Misc\n");
	fprintf(stdout, "\t-d emp      de-emphasis n/5/c        (dflt: (n)one)\n");
	fprintf(stdout, "\t-c          mark as copyright\n");
	fprintf(stdout, "\t-o          mark as original\n");
	fprintf(stdout, "\t-e          add error protection\n");
	fprintf(stdout, "\t-r          force padding bit/frame on\n");
	fprintf(stdout, "\t-D len      add DAB extensions of length [len]\n");
	fprintf(stdout, "\t-t          talkativity 0=no messages (dflt 2)\n");
	fprintf(stdout, "\t-u ind      Set the upper bitrate when in VBR mode\n");
	fprintf(stdout, "\t-R num      Set the number of reserved bits at the end of frame\n");
	fprintf(stdout, "\t-E          turn on energy level extensions\n");

	fprintf(stdout, "Files\n");
	fprintf(stdout, "\tinput       input sound file. (WAV,AIFF,PCM or use '/dev/stdin')\n");
	fprintf(stdout, "\toutput      output bit stream of encoded audio\n");
	
	fprintf(stdout, "\n\tAllowable bitrates for 16, 22.05 and 24kHz sample input\n");
	fprintf(stdout, "\t   8,  16,  24,  32,  40,  48,  56,  64,  80,  96, 112, 128, 144, 160\n");
	fprintf(stdout, "\n\tAllowable bitrates for 32, 44.1 and 48kHz sample input\n");
	fprintf(stdout, "\t  32,  48,  56,  64,  80,  96, 112, 128, 160, 192, 224, 256, 320, 384\n");
	fprintf(stdout, "brate indx 1    2    3    4    5    6    7    8    9   10   11   12   13   14\n");

	fprintf(stdout, "\n");
	exit(1);
}




void
parse_args(int argc, char **argv, twolame_options * glopts, frontend_options * frontOptions,
	   char inPath[MAX_NAME_SIZE], char outPath[MAX_NAME_SIZE])
{
	int	ch = -1;

	/* preset defaults */
	inPath[0] = '\0';
	outPath[0] = '\0';

	/* process args */
	static struct option longopts[] = {
		{ "scale",		required_argument,		NULL,			'b' },
		{ "scale-l",	required_argument,		NULL,			'f' },
		{ "scale-r",	required_argument,		&daggerset,		1 },
		{ NULL,         0,                      NULL,			0 }
	};
	
	while ((ch = getopt_long(argc, argv, "bf:", longopts, NULL)) != -1)
	
	
	while ((ch = getopt(argc, argv, "hs:t:if:lp:y:d:z:")) != -1)
		switch (ch) {
			case 'm':
				if (*optarg == 's') {
					twolame_set_mode(glopts, TWOLAME_STEREO);
				} else if (*optarg == 'd') {
					twolame_set_mode(glopts, TWOLAME_DUAL_CHANNEL);
				} else if (*optarg == 'j') {
					twolame_set_mode(glopts, TWOLAME_JOINT_STEREO);
				} else if (*optarg == 'm') {
					twolame_set_mode(glopts, TWOLAME_MONO);
				} else {
					fprintf(stderr, "Error: -m mode must be s/d/j/m not '%s'\n\n", optarg);
					usage();
				}
				break;
			case 'p':
				twolame_set_psymodel(glopts, atoi(optarg));
				break;

			case 's':
				twolame_set_out_samplerate(glopts, atoi(optarg));
				frontOptions->samplerate = atoi(optarg);
				break;
				
			case 'N':
				frontOptions->channels = atoi(optarg);
				break;
				
			case 'b':
				twolame_set_bitrate(glopts, atoi(optarg));
				break;
				
			case 'd':
				if (*optarg == 'n')
					twolame_set_emphasis(glopts, TWOLAME_EMPHASIS_N);
				else if (*optarg == '5')
					twolame_set_emphasis(glopts, TWOLAME_EMPHASIS_5);
				else if (*optarg == 'c')
					twolame_set_emphasis(glopts, TWOLAME_EMPHASIS_C);
				else {
					fprintf(stderr, "Error: -d emp must be n/5/c not '%s'\n\n", optarg);
					usage();
				}
				break;
			case 'D':
				twolame_set_error_protection(glopts, TRUE);
				twolame_set_DAB(glopts, TRUE);
				twolame_set_DAB_crc_length(glopts, 2);
				twolame_set_DAB_xpad_length(glopts, atoi(optarg));
				// 2 bytes for default DAB
				//	+2 bytes for CRC
				//	+Xpad bytes
				twolame_set_num_ancillary_bits(glopts, (2 + 2 + atoi(optarg)) * 8);

				frontOptions->single_frame_mode = TRUE;
				break;
			case 'c':
				twolame_set_copyright(glopts, TRUE);
				break;
			case 'o':
				twolame_set_original(glopts, TRUE);
				break;
			case 'e':
				twolame_set_error_protection(glopts, TRUE);
				break;
			case 'r':
				twolame_set_padding(glopts, TWOLAME_PAD_ALL);
				break;
			case 'q':
				twolame_set_quick_mode(glopts, TRUE);
				twolame_set_quick_count(glopts, atoi(optarg));
				break;
			case 'a':
				frontOptions->downmix = TRUE;
				twolame_set_mode(glopts, TWOLAME_MONO);
				break;
			case 'x':
				frontOptions->byteswap = TRUE;
				break;
			case 'v':
				twolame_set_VBR(glopts, TRUE);
				twolame_set_VBR_q(glopts, atof(optarg));
				twolame_set_padding(glopts, FALSE);
				/* don't use padding for VBR */
				
				/*
				 * MFC Feb 2003: in VBR mode, joint
				 * stereo doesn't make any sense at
				 * the moment, as there are no noisy
				 * subbands according to
				 * bits_for_nonoise in vbr mode
				 */
				twolame_set_mode(glopts, TWOLAME_STEREO);	/* force stereo mode */
				break;
			case 'l':
				twolame_set_ATH_level(glopts, atof(optarg));
				break;
			case 'h':
				usage();
				break;
			case 'g':
				frontOptions->channelswap = TRUE;
				break;
			case 't':
				twolame_set_verbosity(glopts, atoi(optarg));
				break;
			case 'u':
				twolame_set_VBR_max_bitrate_kbps(glopts, atoi(optarg));
				break;
			case 'R':
				twolame_set_num_ancillary_bits(glopts, atoi(optarg));
				//MFC FIX:	Need to cross validate this option
				// with the energylevel(-E) setting.
				break;
			case 'E':
				twolame_set_energy_levels(glopts, TRUE);
				twolame_set_num_ancillary_bits(glopts, 40);
				// 5 bytes for the stereo energy info
				// NOTE: Assume it 's stereo, and then change to 2 bytes
				// if it turns out to be mono
				// MFC FIX:		This option must be mutually exclusive with the
				// reservebits(-R) option *UNLESS * the number
				// of explicitly reserved bits > 5 bytes.
				break;
			default:
				fprintf(stderr, "Error: Unrecognised option '%c'\n", ch);
				usage();
				break;
			}
	}
	
	
	argc -= optind;
	argv += optind;
     
     
/*
		} else {
			if (inPath[0] == '\0')
				strcpy(inPath, argv[i]);
			else if (outPath[0] == '\0')
				strcpy(outPath, argv[i]);
			else {
				fprintf(stderr, "excess arg %s\n", argv[i]);
				err = 1;
			}
		}
	}
*/

/*
	if (twolame_get_DAB(glopts)) {
		// in 48 kHz 
		//
		// if the bit rate per channel is less then 56 kbit/s, we
		// have 2 scf-crc
		//
		// else we have 4 scf-crc 
		// in 24 kHz, we have 4 scf-crc, see main loop
		if (brate / (twolame_get_mode(glopts) == TWOLAME_MONO ? 1 : 2) >= 56)
			twolame_set_DAB_crc_length(glopts, 4);
	}
	if (err || inPath[0] == '\0')
		usage();	// If no infile defined, or err has occured,
				 // then call usage() 

	if (outPath[0] == '\0') {
		// replace old extension with new one
		new_ext(inPath, ".mp2", outPath);
	}
*/

}




void
print_config(twolame_options * glopts, frontend_options * frontOptions, char *inPath, char *outPath)
{

	if (!twolame_get_verbosity(glopts))
		return;

	fprintf(stderr, "---------------------------------------------------------\n");
	fprintf(stderr, "Input File : '%s'   %.1f kHz\n",
		(strcmp(inPath, "-") ? inPath : "stdin"),
		twolame_get_in_samplerate(glopts) / 1000.0);
	fprintf(stderr, "Output File: '%s'   %.1f kHz\n",
		(strcmp(outPath, "-") ? outPath : "stdout"),
		twolame_get_out_samplerate(glopts) / 1000.0);
	fprintf(stderr, "%d kbps ", twolame_get_bitrate(glopts));
	fprintf(stderr, "%s ", twolame_get_version_name(glopts));
	if (twolame_get_mode(glopts) != TWOLAME_JOINT_STEREO)
		fprintf(stderr, "Layer II %s Psycho model=%d \n",
		twolame_get_mode_name(glopts), twolame_get_psymodel(glopts));
	else
		fprintf(stderr, "Layer II %s Psy model %d \n", twolame_get_mode_name(glopts),
			twolame_get_psymodel(glopts));

	fprintf(stderr, "[De-emph:%s\tCopyright:%s\tOriginal:%s\tCRC:%s]\n",
		((twolame_get_emphasis(glopts)) ? "On" : "Off"),
		((twolame_get_copyright(glopts)) ? "Yes" : "No"),
		((twolame_get_original(glopts)) ? "Yes" : "No"),
		((twolame_get_error_protection(glopts)) ? "On" : "Off"));

	fprintf(stderr, "[Padding:%s\tByte-swap:%s\tChanswap:%s\tDAB:%s]\n",
		((twolame_get_padding(glopts)) ? "Normal" : "Off"),
		((frontOptions->byteswap) ? "On" : "Off"),
		((frontOptions->channelswap) ? "On" : "Off"),
		((twolame_get_DAB(glopts)) ? "On" : "Off"));

	if (twolame_get_VBR(glopts))
		fprintf(stderr, "VBR Enabled. Using MNR boost of %f\n", twolame_get_VBR_q(glopts));
	fprintf(stderr, "ATH adjustment %f\n", twolame_get_ATH_level(glopts));
	fprintf(stderr, "Reserving %i Ancillary bits\n", twolame_get_num_ancillary_bits(glopts));

	fprintf(stderr, "---------------------------------------------------------\n");
}


/*********************************************
 * void short_usage(void)
 ********************************************/
void
short_usage(void)
{
	/* print a bit of info about the program */
	fprintf(stderr, "TwoLAME (http://twolame.sourceforge.net)\n");
	fprintf(stderr, "MPEG Audio Layer II encoder\n\n");
	fprintf(stderr, "USAGE: twolame [options] <infile> [outfile]\n\n");
	fprintf(stderr, "Try \"twolame -h\" for more information.\n");
	exit(0);
}


int
main(int argc, char **argv)
{
	twolame_options *encodeOptions;
	frontend_options *frontOptions;
	char            inputfilename[256], outputfilename[256];
	FILE           *outfile;
	short int      *pcmaudio;
	int             num_samples;
	int             frames = 0;
	unsigned char  *mp2buffer;
	//mp2buffer will never be bigger than PCM input
	int             audioReadSize = AUDIOBUFSIZE;
	int             mp2fill_size;

	if (argc == 1)
		short_usage();

	/* Allocate some space for the PCM audio data */
	if ((pcmaudio = (short int *) calloc(audioReadSize, sizeof(short int))) == NULL) {
		fprintf(stderr, "pcmaudio alloc failed\n");
		exit(99);
	}
	/* Allocate some space for the encoded MP2 audio data */
	if ((mp2buffer = (char *) calloc(MP2BUFSIZE, sizeof(char))) == NULL) {
		fprintf(stderr, "Error: mp2buffer alloc failed\n");
		exit(99);
	}
	/* grab a set of default options */
	encodeOptions = twolame_init();
	frontOptions = frontend_init();

	/* tweak them or get them from the command line */
	parse_args(argc, argv, encodeOptions, frontOptions, inputfilename, outputfilename);

	/* Frontend is responsible for opening sound file */
	audio_info = audio_open(inputfilename, frontOptions->channels, frontOptions->samplerate);
	if (audio_info == NULL) {
		fprintf(stderr, "No input file opened.\n");
		exit(99);
	} else {
		//Use sound file to over - ride preferences for
			//mono
			/stereo and sampling - frequency
				if (audio_info->channels == 1)
				twolame_set_mode(encodeOptions, TWOLAME_MONO);

		twolame_set_num_channels(encodeOptions, audio_info->channels);
		twolame_set_in_samplerate(encodeOptions, audio_info->samplerate);
		twolame_set_out_samplerate(encodeOptions, audio_info->samplerate);
		audio_info->byteswap = frontOptions->byteswap;
	}

	/*
	 * If energy information is required, see if we're in MONO mode in
	 * which case, we only need 16 bits of ancillary data
	 */
	if (twolame_get_energy_levels(encodeOptions))
		if (twolame_get_mode(encodeOptions) == TWOLAME_MONO)
			twolame_set_num_ancillary_bits(encodeOptions, 16);
	// only 2 bytes needed for energy level for mono channel

	/* initialise twolame with this set of options */
	twolame_init_params(encodeOptions);

	/* dump the config for a look at what you're about to do */
	print_config(encodeOptions, frontOptions, inputfilename, outputfilename);

	/* Open the output file for the encoded MP2 data */
	if ((outfile = fopen(outputfilename, "w")) == 0) {
		fprintf(stderr, "output file error opening %s\n", outputfilename);
		exit(99);
	}
	if (frontOptions->single_frame_mode)
		audioReadSize = 1152;

	/* Now do the buffering/encoding/writing */
	while ((num_samples = audio_info->get_samples(audio_info, pcmaudio, audioReadSize)) != 0) {
		//Read num_samples of audio data * per channel * from the input file
			mp2fill_size = twolame_encode_buffer_interleaved(encodeOptions, pcmaudio, num_samples, mp2buffer, MP2BUFSIZE);
		fwrite(mp2buffer, sizeof(unsigned char), mp2fill_size, outfile);
		frames += (num_samples / 1152);
		fprintf(stderr, "[%04i]\r", frames);
		fflush(stderr);
	}

	/*
	 * flush any remaining audio. (don't send any new audio data) There
	 * should only ever be a max of 1 frame on a flush. There may be zero
	 * frames if the audio data was an exact multiple of 1152
	 */
	mp2fill_size = twolame_encode_flush(encodeOptions, mp2buffer, MP2BUFSIZE);
	fwrite(mp2buffer, sizeof(unsigned char), mp2fill_size, outfile);

	twolame_close(&encodeOptions);
	audio_info->close(&audio_info);
	fprintf(stderr, "\nFinished nicely\n");
	return (0);
}
