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


#include <stdio.h>
#include "hpfilt.h"


#define PIF 3.141592f
#define P2F 6.283185f


void HP_conf::process (void)
{
    float a, b, t;

    if (_w1 == _w0) return;
    if (_w0 == 0) _w0 = _w1;
    if      (_w1 > 1.25f * _w0) _w0 *= 1.25f;
    else if (_w1 < 0.80f * _w0) _w0 *= 0.80f;
    else _w1 = _w0;

    _g = 1;
    a = PIF * _w0;
    b = a * a;
    a *= 1.8f;
    t = 1 + a + b;
    _g /= t;   
    _c1 = 2 * a + 4 * b;
    _c2 = 4 * b / _c1;
    _c1 /= t;
    a = PIF * _w0;
    b = a * a;
    a *= 0.8f;
    t = 1 + a + b;
    _g /= t;   
    _c3 = 2 * a + 4 * b;
    _c4 = 4 * b / _c3;
    _c3 /= t;
}


void HP_filt::process (const HP_conf *C, int n, float *ip, float *op)
{
    float x, z1, z2, z3, z4;

    z1 = _z1;
    z2 = _z2;
    z3 = _z3;
    z4 = _z4;
    while (n--)
    {
	x = *ip++;
        x -= z1 + z2 + 1e-20f;
        z2 += C->_c2 * z1;
        z1 += C->_c1 * x;
        x -= z3 + z4 + 1e-20f;
        z4 += C->_c4 * z3;
        z3 += C->_c3 * x;
        *op++ = C->_g * x;
    }
    _z1 = z1;
    _z2 = z2;
    _z3 = z3;
    _z4 = z4;
}
