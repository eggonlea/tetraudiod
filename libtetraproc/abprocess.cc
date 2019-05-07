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


#include <string.h>
#include <math.h>
#include "bformat.h"
#include "abprocess.h"


ABprocess::ABprocess (void) :
    _mute (0),
    _invb (0),
    _endf (0),
    _form (Bformat::FM_FUMA),
    _enable (0),
    _cerror (0)
{
}


ABprocess::~ABprocess (void)
{
}


void ABprocess::reset (void)
{
    int i;

    for (i = 0; i < 4; i++)
    {
	_hpfilt [i].reset ();
	_lffilt [i].reset ();
	_hffilt [i].reset ();
    }
}


void ABprocess::process (unsigned int n, float *p [4], float *q [4])
{
    unsigned int  i, j, k;
    float         c0, c1, c2, c3;
    float         *p0, *p1, *p2, *p3, t;
    float         W [64], X [64], Y [64], Z [64];

    _hpconf.process ();
    _hpfilt [0].process (&_hpconf, n, p [0], q [0]);
    _hpfilt [1].process (&_hpconf, n, p [1], q [1]);
    _hpfilt [2].process (&_hpconf, n, p [2], q [2]);
    _hpfilt [3].process (&_hpconf, n, p [3], q [3]);

    if (_mute & 1) memset (q [0], 0, n * sizeof (float));
    if (_mute & 2) memset (q [1], 0, n * sizeof (float));
    if (_mute & 4) memset (q [2], 0, n * sizeof (float));
    if (_mute & 8) memset (q [3], 0, n * sizeof (float));

    if (_enable & ABconfig::EN_MATR)
    {
 	p0 = q [0];
 	p1 = q [1];
 	p2 = q [2];
 	p3 = q [3];
	for (i = 0; i < n; i += 64)
	{
	    k = n - i;
            if (k > 64) k = 64;
	    c0 = _matrix [0][0];
	    c1 = _matrix [0][1];
	    c2 = _matrix [0][2];
	    c3 = _matrix [0][3];
	    for (j = 0; j < k; j++) W [j] = c0 * p0 [j] + c1 * p1 [j] + c2 * p2 [j] + c3 * p3 [j];
	    c0 = _matrix [1][0];
	    c1 = _matrix [1][1];
	    c2 = _matrix [1][2];
	    c3 = _matrix [1][3];
	    for (j = 0; j < k; j++) X [j] = c0 * p0 [j] + c1 * p1 [j] + c2 * p2 [j] + c3 * p3 [j];
	    c0 = _matrix [2][0];
	    c1 = _matrix [2][1];
	    c2 = _matrix [2][2];
	    c3 = _matrix [2][3];
	    for (j = 0; j < k; j++) Y [j] = c0 * p0 [j] + c1 * p1 [j] + c2 * p2 [j] + c3 * p3 [j];
	    c0 = _matrix [3][0];
	    c1 = _matrix [3][1];
	    c2 = _matrix [3][2];
	    c3 = _matrix [3][3];
	    for (j = 0; j < k; j++) Z [j] = c0 * p0 [j] + c1 * p1 [j] + c2 * p2 [j] + c3 * p3 [j];
            memcpy (p0, W, k * sizeof (float));
            memcpy (p1, X, k * sizeof (float));
            memcpy (p2, Y, k * sizeof (float));
            memcpy (p3, Z, k * sizeof (float));
	    p0 += k;
	    p1 += k;
	    p2 += k;
	    p3 += k;
	}	    
    }

    if (_endf)
    {
        for (i = 0; i < n; i++)
	{
	    t = q [1][i];
	    q [1][i] = q [3][i];
	    q [3][i] = -t;
	}
    }

    c0 = 1;
    c1 = (_invb & 2) ? -1 : 1;
    c2 = (_invb & 4) ? -1 : 1;
    c3 = (_invb & 8) ? -1 : 1;
    if (_form == Bformat::FM_FUMA)
    {
        c0 *= 0.7071f;
    }
    else if (_form == Bformat::FM_N3D)
    {
	c1 *= 1.7320f;
	c2 *= 1.7320f;
	c3 *= 1.7320f;
    }
    if (c0 != 1.0f) for (i = 0; i < n; i++) q [0][i] *= c0;
    if (c1 != 1.0f) for (i = 0; i < n; i++) q [1][i] *= c1;
    if (c2 != 1.0f) for (i = 0; i < n; i++) q [2][i] *= c2;
    if (c3 != 1.0f) for (i = 0; i < n; i++) q [3][i] *= c3;
}


void ABprocess::checkfp (float& f, float& b, float& g)
{
    if (f < 1e1f) f = 1e1f;
    if (f > 2e4f) f = 2e4f;
    if (b < 0.1f) b = 0.1f;
    if (b > 10.f) b = 10.f;
    if (g < -30.f) g = -30.f;
    if (g >  30.f) g =  30.f;
    f /= _fsamp;
    g = powf (10.f, 0.05f * g);
}


int ABprocess::set_lffilt (ABconfig *C)
{
    float f, b, g;

    if (C->_enable & ABconfig::EN_LFEQ)
    {
        _enable |= ABconfig::EN_LFEQ;
        f = C->_lffilt [0];
        b = C->_lffilt [1];
        g = C->_lffilt [2];
        checkfp (f, b, g);
        _lffilt [0].init (f, b, g);
        _lffilt [1].init (_lffilt [0]);
        _lffilt [2].init (_lffilt [0]);
        _lffilt [3].init (_lffilt [0]);
    }
    else _enable &= ~ABconfig::EN_LFEQ;
    return 0;
}


int ABprocess::set_matrix (ABconfig *C)
{
    if (C->_enable & ABconfig::EN_MATR)
    {
	_enable |= ABconfig::EN_MATR;
        memcpy (_matrix, C->_matrix, 16 * sizeof (float));
    }
    else _enable &= ~ABconfig::EN_MATR;
    return 0;
}


int ABprocess::set_convol (ABconfig *C)
{
    int       i, j, k;

    _enable &= ~ABconfig::EN_CONV;
    _cerror = false;

    // we don't support convol yet
    (void)C;

    return 0;
}


int ABprocess::set_hffilt (ABconfig *C)
{
    int   i;
    float f1, b1, g1, f2, b2, g2, gc;

    if (C->_enable & ABconfig::EN_PMEQ)
    {
	_enable |= ABconfig::EN_PMEQ;
        for (i = 0; i < 4; i++)
        {
	    f1 = C->_hffilt [i][0];
	    b1 = C->_hffilt [i][1];
	    g1 = C->_hffilt [i][2];
	    f2 = C->_hffilt [i][3];
	    b2 = C->_hffilt [i][4];
	    g2 = C->_hffilt [i][5];
	    checkfp (f1, b1, g1);
	    checkfp (f2, b2, g2);
	    gc = powf (10.0f, 0.05f * C->_eqgain [i]);
	    _hffilt [i].init (f1, b1, g1, f2, b2, g2, gc);
	}
    }
    else _enable &= ~ABconfig::EN_PMEQ;
    return 0;
}

