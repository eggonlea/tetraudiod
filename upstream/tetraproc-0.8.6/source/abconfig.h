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


#ifndef __ABCONFIG_H
#define __ABCONFIG_H


#include "bformat.h"
#include "impdata.h"


class ABconfig
{
public:

    ABconfig (void);

    void reset (void);
    int save (const char *name);
    int load (const char *name);
    int openconvfile (void);
    int closeconvfile (void);

    enum { EN_LFEQ = 1, EN_MATR = 2, EN_CONV = 4, EN_PMEQ = 8, EN_FORM = 16, EN_ALL = 31 };

    char            _copyright [64];
    char            _descript [64];
    char            _micident [64];
    int             _update;
    int             _enable;
    float           _lffilt [3];
    float           _matrix [4][4];
    float           _hffilt [4][9];
    float           _eqgain [4];
    char            _convfile [1024];
    Impdata         _convdata;
};


#endif
