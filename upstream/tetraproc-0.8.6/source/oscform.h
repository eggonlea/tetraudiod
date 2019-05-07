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


#ifndef __OSCFORM_H
#define __OSCFORM_H


#include <stdlib.h>
#include <stdio.h>


class Oscform
{
public:

    Oscform (int size);
    ~Oscform (void);

    int put_command (const char *v);
    int put_format (const char *v);
    int put_int (int v);
    int put_float (float v);
    int put_string (const char *v);
    int write (FILE *F);

    int read (FILE *F);
    const char *get_command (void) { return _pcomm; }
    const char *get_format (void) { return _pform; }
    int get_datalen (void) { return _isize; }
    int get_int (int *v);
    int get_float (float *v);
    int get_float1 (float *v);
    int get_string (const char **p, int *n);

private:

    char   *_data;
    int     _size;
    int     _iform; 
    int     _idata;
    int     _inext;
    int     _isize;
    char   *_pcomm;
    char   *_pform;
};


#endif
