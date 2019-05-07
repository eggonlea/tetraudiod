// ----------------------------------------------------------------------------
//
//  Copyright (C) 2007-2010 Fons Adriaensen <fons@linuxaudio.org>
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
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
// ----------------------------------------------------------------------------


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/stat.h>
#include "abconfig.h"
#include "oscform.h"


ABconfig::ABconfig (void)
{
    reset ();
}


void ABconfig::reset (void)
{
    int i;

    strcpy (_copyright, "(C) 2009 Fons Adriaensen");
    strcpy (_descript, "Default configuration");
    strcpy (_micident, "Theoretical microphone");
    _enable = EN_MATR;
    _update = 0;
    _lffilt [0] = 20.0f;
    _lffilt [1] = 2.0f;
    _lffilt [2] = 0;
    _matrix [0][0] = 0.5f;
    _matrix [0][1] = 0.5f;
    _matrix [0][2] = 0.5f;
    _matrix [0][3] = 0.5f;
    _matrix [1][0] =  0.866;
    _matrix [1][1] =  0.866f; 
    _matrix [1][2] = -0.866f; 
    _matrix [1][3] = -0.866f; 
    _matrix [2][0] =  0.866f; 
    _matrix [2][1] = -0.866f; 
    _matrix [2][2] =  0.866f; 
    _matrix [2][3] = -0.866f; 
    _matrix [3][0] =  0.866f; 
    _matrix [3][1] = -0.866f; 
    _matrix [3][2] = -0.866f; 
    _matrix [3][3] =  0.866f; 
    *_convfile = 0;
    for (i = 0; i < 4; i++)
    {
	_hffilt [i][0] = 8e3f;
	_hffilt [i][1] = 1.0f;
	_hffilt [i][2] = 0.0f;
	_hffilt [i][3] = 8e3f;
	_hffilt [i][4] = 1.0f;
	_hffilt [i][5] = 0.0f;
	_eqgain [i] = 0.0f;
    }
}


int ABconfig::save (const char *file)
{
    int           i, j;
    char          *p;
    FILE          *F;
    char          s [1024];
    Oscform       Z (4096);

    strcpy (s, file);
    p = strrchr (s, '.');
    if (!p) strcat (s, ".tetra");
    if (! (F = fopen (s, "w"))) 
    {
	fprintf (stderr, "Can't open '%s' for writing\n", s);
        return 1;
    } 
    
    Z.put_command ("/tetraproc");
    Z.put_format (",i");
    Z.put_int (2);
    Z.write (F);

    Z.put_command ("/copyright");
    Z.put_format (",s");
    Z.put_string (_copyright);
    Z.write (F);

    Z.put_command ("/descript");
    Z.put_format (",s");
    Z.put_string (_descript);
    Z.write (F);

    Z.put_command ("/micident");
    Z.put_format (",s");
    Z.put_string (_micident);
    Z.write (F);

    Z.put_command ("/enable");
    Z.put_format (",i");
    Z.put_int (_enable);
    Z.write (F);

    Z.put_command ("/lffilt/par");
    Z.put_format (",fff");
    for (i = 0; i < 3; i++) Z.put_float (_lffilt [i]);
    Z.write (F);
    for (i = 0; i < 4; i++)
    {
        Z.put_command ("/matrix/row");
        Z.put_format (",iffff");
	Z.put_int (i);
        for (j = 0; j < 4; j++) Z.put_float (_matrix [i][j]);
	Z.write (F);
    }
    Z.put_command ("/convol/file");
    Z.put_format (",s");
    Z.put_string (_convfile);
    Z.write (F);
    for (i = 0; i < 4; i++)
    {
        Z.put_command ("/pmfilt/par");
        Z.put_format (",ifffffff");
	Z.put_int (i);
        for (j = 0; j < 6; j++) Z.put_float (_hffilt [i][j]);
	Z.put_float (_eqgain [i]);
	Z.write (F);
    }
	
    fclose (F);
    return 0;
}


int ABconfig::load (const char *file)
{
    int           i, j, n, err, ver;
    const char    *p, *q;
    FILE          *F;
    char          s [1024];
    Oscform       Z (4096);

    strcpy (s, file);
    p = strrchr (s, '.');
    if (!p) strcat (s, ".tetra");
    if (! (F = fopen (s, "r"))) 
    {
	fprintf (stderr, "Can't open file '%s' for reading\n", s);
        return 1;
    } 

    reset ();
    if (Z.read (F) || ((p = Z.get_command ()) == 0) || strcmp (p, "/tetraproc"))
    {
	fprintf (stderr, "File '%s' is not a valid configuration file.\n", s);
        fclose (F);
	return -1;
    }
    if (Z.get_int (&ver) || (ver <  1) || (ver > 2))
    {
	fprintf (stderr, "File '%s' has wrong version number.\n", s);
        fclose (F);
	return -1;
    }

    err = 0;
    while (!(err || Z.read (F)))
    {
	p = Z.get_command ();
	if (!p) err = 1;
	else if (!strcmp (p, "/copyright"))
	{
	    err = Z.get_string (&q, &n);
	    if (!err) strcpy (_copyright, q);
	}
	else if (!strcmp (p, "/descript"))
	{
	    err = Z.get_string (&q, &n);
	    if (!err) strcpy (_descript, q);
	}
	else if (!strcmp (p, "/micident"))
	{
	    err = Z.get_string (&q, &n);
	    if (!err) strcpy (_micident, q);
	}
	else if (!strcmp (p, "/enable"))
	{
	    err = Z.get_int (&_enable);
	}
	else if (!strcmp (p, "/lffilt/par"))
        {
	    for (i = 0; i < 3; i++) err |= Z.get_float (&_lffilt [i]);
	}
	else if (!strcmp (p, "/matrix/row"))
	{
	    err = Z.get_int (&i);
	    if (!err) for (j = 0; j < 4; j++) err |= Z.get_float (&_matrix [i][j]);
	}	    
	else if (!strcmp (p, "/convol/file"))
	{
	    err = Z.get_string (&q, &n);
	    if (!err) strcpy (_convfile, q);
	}
	else if (!strcmp (p, "/pmfilt/par"))
	{
	    err = Z.get_int (&i);
	    if (!err) for (j = 0; j < 6; j++) err |= Z.get_float (&_hffilt [i][j]);
	    if (ver == 2) err |= Z.get_float (&_eqgain [i]);
	}
	else
	{
	    fprintf (stderr, "Command '%s' not understood - ignored'\n",  p);
	}
	if (err)
	{
	    fprintf (stderr, "Format error in command '%s'\n",  p);
	    fclose (F);
	    return -1;
	}
    }

    fclose (F);
    return 0;
}
