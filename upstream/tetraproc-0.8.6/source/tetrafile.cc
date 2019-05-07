// -----------------------------------------------------------------------------
//
//  Copyright (C) 2006-2018 Fons Adriaensen <fons@linuxaudio.org>
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// -----------------------------------------------------------------------------


#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdio.h>
#include <math.h>
#include "audiofile.h"
#include "abprocess.h"


enum
{
     HELP, STIN,
     HPF, INVX, INVY, INVZ, ENDF, FUMA, SN3D, N3D,
     CAF, WAV, AMB, AIFF, FLAC, BIT16, BIT24, FLOAT, REC, TRI, LIPS 
};
enum { BUFFSIZE = 256 };


static float hpfr = 0;
static int   norm = Bformat::FM_FUMA;
static int   invb = 0;
static int   endf = 0;
static int   stin = 0;
static int   type = Audiofile::TYPE_AMB;
static int   form = Audiofile::FORM_24BIT;
static int   dith = Audiofile::DITHER_NONE;


static void help (void)
{
    fprintf (stderr, "\ntetrafile %s\n", VERSION);
    fprintf (stderr, "(C) 2007-2014 Fons Adriaensen  <fons@linuxaudio.org>\n");
    fprintf (stderr, "Usage: tetrafile <options> <config file> <input file(s)> <output file>.\n");
    fprintf (stderr, "Options:\n");
    fprintf (stderr, "  Display this text:     --help\n");
    fprintf (stderr, "  Stereo input files:    --stin\n");
    fprintf (stderr, "  Highpass filter:       --hpf <frequency>\n");
    fprintf (stderr, "  B format standard:     --fuma, --sn3d, --n3d\n");
    fprintf (stderr, "  B format inversion:    --invx, --invy, --invz\n");
    fprintf (stderr, "  Endfire mode:          --endf\n");
    fprintf (stderr, "  Output file type:      --amb, --caf, --wav, -aiff, --flac\n");
    fprintf (stderr, "  Output sample format:  --16bit, --24bit, --float\n");
    fprintf (stderr, "  Dither type (16 bit):  --rec, --tri, --lips\n");
    fprintf (stderr, "The default options are --fuma, --amb, --24bit.\n");
    exit (1);
}


static struct option options [] = 
{
    { "help",  0, 0, HELP  },
    { "stin",  0, 0, STIN  },
    { "invx",  0, 0, INVX  },
    { "invy",  0, 0, INVY  },
    { "invz",  0, 0, INVZ  },
    { "endf",  0, 0, ENDF  },
    { "fuma",  0, 0, FUMA  },
    { "sn3d",  0, 0, SN3D  },
    { "n3d",   0, 0, N3D   },
    { "amb",   0, 0, AMB   },
    { "caf",   0, 0, CAF   },
    { "wav",   0, 0, WAV   },
    { "aiff",  0, 0, AIFF  },
    { "flac",  0, 0, FLAC  },
    { "hpf",   1, 0, HPF   },
    { "16bit", 0, 0, BIT16 },
    { "24bit", 0, 0, BIT24 },
    { "float", 0, 0, FLOAT },
    { "rec",   0, 0, REC   },
    { "tri",   0, 0, TRI   },
    { "lips",  0, 0, LIPS  },
    { 0, 0, 0, 0 }
};


static void procoptions (int ac, char *av [])
{
    int k;

    while ((k = getopt_long (ac, av, "", options, 0)) != -1)
    {
	switch (k)
	{
        case '?':
	case HELP:
	    help ();
	    break;
	case STIN:
	    stin = 1;
	    break;
	case HPF:
	    if ((sscanf (optarg, "%f", &hpfr) != 1) || (hpfr < 10) || (hpfr > 160))
	    {
		fprintf (stderr, "Illegal value for --hpf option: '%s'.\n", optarg);
		exit (1);
	    }
	    break;
	case INVX:
	    invb |= 2;
	    break;
	case INVY:
	    invb |= 4;
	    break;
	case INVZ:
	    invb |= 8;
	    break;
	case ENDF:
	    endf = 1;
	    break;
	case FUMA:
	    norm = Bformat::FM_FUMA;
	    break;
	case SN3D:
	    norm = Bformat::FM_SN3D;
	    break;
	case N3D:
	    norm = Bformat::FM_N3D;
	    break;
	case CAF:
	    type = Audiofile::TYPE_CAF;
	    break;
	case WAV:
	    type = Audiofile::TYPE_WAV;
	    break;
	case AMB:
	    type = Audiofile::TYPE_AMB;
	    break;
	case AIFF:
	    type = Audiofile::TYPE_AIFF;
	    break;
	case FLAC:
	    type = Audiofile::TYPE_FLAC;
	    break;
	case BIT16:
	    form = Audiofile::FORM_16BIT;
	    break;
	case BIT24:
	    form = Audiofile::FORM_24BIT;
	    break;
	case FLOAT:
	    form = Audiofile::FORM_FLOAT;
	    break;
	case REC:
	    dith = Audiofile::DITHER_RECT;
	    break;
	case TRI:
	    dith = Audiofile::DITHER_TRIA;
	    break;
	case LIPS:
	    dith = Audiofile::DITHER_LIPS;
	    break;
 	}
    }
}


int main (int ac, char *av [])
{
    Audiofile     Ainp1;
    Audiofile     Ainp2;
    Audiofile     Aout;
    ABprocess     abproc;
    ABconfig      config;
    unsigned int  i, k, n, rate;
    float         *buff, *inp [4], *out [4];
    float         *p;

    procoptions (ac, av);
    if (ac - optind < 3 + stin)
    {
        fprintf (stderr, "Missing arguments, try --help.\n");
	return 1;
    }
    if (ac - optind > 3 + stin )
    {
        fprintf (stderr, "Too many arguments, try --help.\n");
	return 1;
    }

    if (config.load (av [optind]))
    {
	fprintf (stderr, "Can't open config file '%s'.\n", av [optind]);
	return 1;
    }

    n = stin ? 2 : 4;
    optind++;
    if (Ainp1.open_read (av [optind]))
    {
	fprintf (stderr, "Can't open input file '%s'.\n", av [optind]);
	return 1;
    }
    if (Ainp1.chan () != (int) n)
    {
	fprintf (stderr, "Input file '%s' does not have %d channels.\n", av [optind], n);
	Ainp1.close ();
	return 1;
    }
    rate = Ainp1.rate ();

    if (stin)
    {
        optind++;
        if (Ainp2.open_read (av [optind]))
        {
	    fprintf (stderr, "Can't open input file '%s'.\n", av [optind]);
	    Ainp1.close ();
	    return 1;
        }
        if (Ainp2.chan () != (int) n)
        {
	    fprintf (stderr, "Input file '%s' does not have %d channels.\n", av [optind], n);
	    Ainp1.close ();
	    Ainp2.close ();
	    return 1;
        }
	if (Ainp1.size () != Ainp2.size ())
	{
	    fprintf (stderr, "Sizes of input files don't match.\n");
	    Ainp1.close ();
	    Ainp2.close ();
	    return 1;
        }
	if (Ainp1.rate () != Ainp2.rate ())
	{
	    fprintf (stderr, "Sample rates of input files don't match.\n");
	    Ainp1.close ();
	    Ainp2.close ();
	    return 1;
        }
    }

    optind++;
    if (Aout.open_write (av [optind], type, form, rate, 4))
    {
	fprintf (stderr, "Can't open output file '%s'.\n", av [optind]);
	Ainp1.close ();
	Ainp2.close ();
	return 1;
    }
    if (dith != Audiofile::DITHER_NONE) 
    {
	Aout.set_dither (dith);
    }

    abproc.init (rate, BUFFSIZE, BUFFSIZE);
    abproc.set_lffilt (&config);
    abproc.set_matrix (&config);
    abproc.set_convol (&config);
    abproc.set_hffilt (&config); 
    abproc.set_hpfil ((hpfr < 1) ? 1 : hpfr);
    abproc.set_invb (invb);
    abproc.set_endf (endf);
    abproc.set_norm (norm);

    buff = new float [4 * BUFFSIZE];
    for (i = 0; i < 4; i++)
    {
	inp [i] = new float [BUFFSIZE];
	out [i] = new float [BUFFSIZE];
    }

    while (1)
    {
        k = Ainp1.read (buff, BUFFSIZE);
        if (k)
        {
 	    if (stin)
	    {
                for (i = 0, p = buff; i < k; i++, p += 2)
                {
		    inp [0][i] = p [0];
		    inp [1][i] = p [1];
		}
   	        Ainp2.read (buff, BUFFSIZE);
                for (i = 0, p = buff; i < k; i++, p += 2)
                {
		    inp [2][i] = p [0];
		    inp [3][i] = p [1];
		}
	    }
            else
	    {
                for (i = 0, p = buff; i < k; i++, p += 4)
                {
		    inp [0][i] = p [0];
		    inp [1][i] = p [1];
		    inp [2][i] = p [2];
		    inp [3][i] = p [3];
		}
	    }

	    abproc.process (k, inp, out); 
            for (i = 0, p = buff; i < k; i++, p += 4)
	    {
		p [0] = out [0][i];
		p [1] = out [1][i];
		p [2] = out [2][i];
		p [3] = out [3][i];
	    }
            Aout.write (buff, k);
	}
        else break;
    }

    Aout.close ();
    Ainp1.close ();
    Ainp2.close ();
    delete[] buff;
    for (i = 0; i < 4; i++)
    {
	delete[] inp [i];
	delete[] out [i];
    }

    return 0;
}
