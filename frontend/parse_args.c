/*
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
/*				twolame_set_mode(glopts, TWOLAME_STEREO);	// force stereo mode 
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


