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
#include <string.h>
#include <math.h>
#include "virtmic.h"
#include "bformat.h"


#define PIF 3.141592f
#define P2F 6.283185f


Virtmic::Virtmic (void) :
    _norm (0),
    _azim (0),
    _elev (0),
    _angle (0),
    _direc (0),
    _xtalk (false),
    _mono  (false),
    _norm1 (0),
    _azim1 (0),
    _elev1 (0),
    _angle1 (0),
    _direc1 (0),
    _csw (0),
    _csx (0),
    _csy (0),
    _csz (0),
    _cdx (0),
    _cdy (0),
    _ww (0),
    _zs (0),
    _zd (0)
{
}   


Virtmic::~Virtmic (void)
{
}


void Virtmic::set_fsam (int fsam)
{
    _ww = P2F * 400.0f / fsam;
}


void Virtmic::process (int n, float *W, float *X,float *Y, float *Z, float *L, float *R)
{
    int    i, k;
    float  w, dw, x, dx, y, dy, z, dz;
    float  ca, sa, ce, se, cv, sv;
    float  s, d, zs, zd;
    float  S [64];
    float  D [64];

    while (n)
    {
	k = (n > 64) ? 64 : n;
        if (  (_norm != _norm1)
            | circheck (_azim, _azim1)
            | lincheck (_elev, _elev1)
            | lincheck (_angle, _angle1)
            | lincheck (_direc, _direc1))
	{                
	    _norm = _norm1;
            ca = cosf (P2F * _azim);
            sa = sinf (P2F * _azim);
            ce = cosf (P2F * _elev);
            se = sinf (P2F * _elev);
            cv = cosf (PIF * _angle);
            sv = sinf (PIF * _angle);

	    w = _csw;
	    x = _csx;
	    y = _csy;
	    z = _csz;
	    d = _direc;
	    if (_norm == Bformat::FM_N3D) d *= 0.57735f;
	    _csw = 1 - _direc;
	    _csx = d * ca * ce * cv;
            _csy = d * sa * ce * cv;
            _csz = d * cv * se;
	    if (_norm == Bformat::FM_FUMA) _csw *= 1.41421f;
	    dw = diff (k, w, _csw);
	    dx = diff (k, x, _csx);
            dy = diff (k, y, _csy);
            dz = diff (k, z, _csz);
    	    for (i = 0; i < k; i++)
    	    {
		w += dw;
                x += dx;
                y += dy;
                z += dz;
		S [i] = w * W [i] + x * X [i] + y * Y [i] + z * Z [i]; 
    	    }

	    x = _cdx;
	    y = _cdy;
            _cdx = -d * sa * sv;
	    _cdy =  d * ca * sv;
            dx = diff (k, x, _cdx);
	    dy = diff (k, y, _cdy);
   	    for (i = 0; i < k; i++)
   	    {
                x += dx;
                y += dy;
		D [i] = x * X [i] + y * Y [i];
            }
	}
        else
	{
   	    for (i = 0; i < k; i++)
	    {
                S [i] = _csw * W [i] + _csx * X [i] + _csy * Y [i] + _csz * Z [i]; 
                D [i] = _cdx * X [i] + _cdy * Y [i];
	    }
	}

	if (_mono) memset (D, 0, k * sizeof (float));
	if (_xtalk)
	{
	    zs = _zs;
	    zd = _zd;
            for (i = 0; i < k; i++)
	    {
	        s = S [i];
		zs += _ww * (s - zs) + 1e-20f;
		s += 0.4f * zs;
		d = D [i];
		zd += _ww * (d - zd) + 1e-20f;
		d -= zd;
		*L++ = s + d;
		*R++ = s - d;
	    }
	    _zs = zs;
	    _zd = zd;
	}
        else
	{
 	    for (i = 0; i < k; i++)
	    {
		*L++ = S [i] + D [i]; 
		*R++ = S [i] - D [i]; 
	    }		
	    _zs = _zd = 0;
	}

        W += k;
        X += k;
        Y += k;           
        Z += k;
        n -= k;
    }
}

