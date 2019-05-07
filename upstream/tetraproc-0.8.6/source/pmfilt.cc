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


#include <math.h>
#include "pmfilt.h"


#define PIF 3.141592f
#define P2F 6.283185f


void PM_filt1::init (float w, float b, float g)
{
    b *= PIF * w;         
//    if (g < 1.0f) b /= g;  // Uncomment for +/- gain symmetry - NOT for TetraProc
    _g = 0.5f * (g - 1);
    _c1 = -cosf (P2F * w);
    _c2 = (1 - b) / (1 + b);
}


void PM_filt1::init (PM_filt1& F)
{
    _g  = F._g;
    _c1 = F._c1;
    _c2 = F._c2;
}


void PM_filt1::process (int n, float *ip, float *op)
{
    float x, y, z1, z2;

    z1 = _z1;
    z2 = _z2;
    while (n--)
    {
	x = *ip++;
	y = x - _c2 * z2;
	x -= _g * (z2 + _c2 * y - x);                           
	y -= _c1 * z1;
	z2 = z1 + _c1 * y;
	z1 = y + 1e-10f;
	*op++ = x;
    }
    _z1 = z1;
    _z2 = z2;
}



void PM_filt3::init (float w1, float b1, float g1,
                     float w2, float b2, float g2,
                     float w3, float b3, float g3,
                     float gc)
{
    b1 *= PIF * w1;         
    _g1 = (g1 - 1) / 2;
    _c1 = -cosf (P2F * w1);
    _c2 = (1 - b1) / (1 + b1);
    b2 *= PIF * w2;         
    _g2 = (g2 - 1) / 2;
    _c3 = -cosf (P2F * w2);
    _c4 = (1 - b2) / (1 + b2);
    b3 *= PIF * w3;         
    _g3 = (g3 - 1) / 2;
    _c5 = -cosf (P2F * w3);
    _c6 = (1 - b3) / (1 + b3);
    _gc = gc;
}


void PM_filt3::init (PM_filt3& F)
{
    _g1 = F._g1;
    _g2 = F._g2;
    _g3 = F._g3;
    _gc = F._gc;
    _c1 = F._c1;
    _c2 = F._c2;
    _c3 = F._c3;
    _c4 = F._c4;
    _c5 = F._c5;
    _c6 = F._c6;
}


void PM_filt3::process (int n, float *ip, float *op)
{
    float x, y, z1, z2, z3, z4, z5, z6;

    z1 = _z1;
    z2 = _z2;
    z3 = _z3;
    z4 = _z4;
    z5 = _z5;
    z6 = _z6;

    while (n--)
    {
	x = *ip++;

	y = x - _c2 * z2;
	x -= _g1 * (z2 + _c2 * y - x);                           
	y -= _c1 * z1;
	z2 = z1 + _c1 * y;
	z1 = y + 1e-10f;

	y = x - _c4 * z4;
	x -= _g2 * (z4 + _c4 * y - x);                           
	y -= _c3 * z3;
	z4 = z3 + _c3 * y;
	z3 = y + 1e-10f;

	y = x - _c6 * z6;
	x -= _g3 * (z6 + _c6 * y - x);                           
	y -= _c5 * z5;
	z6 = z5 + _c5 * y;
	z5 = y + 1e-10f;

	*op++ = x * _gc;
    }

    _z1 = z1;
    _z2 = z2;
    _z3 = z3;
    _z4 = z4;
    _z5 = z5;
    _z6 = z6;
}
