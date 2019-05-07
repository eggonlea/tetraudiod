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


#ifndef __VIRTMIC_H
#define __VIRTMIC_H


#include <math.h>



class Virtmic 
{
public:

    Virtmic (void);
    ~Virtmic (void);

    void process (int n, float *W, float *X, float *Y, float *Z, float *L, float *R);
    void set_fsam  (int fsam);

    void set_norm  (int   v)  { _norm1  = v; }
    void set_azim  (float v)  { _azim1  = v; }
    void set_elev  (float v)  { _elev1  = v; }
    void set_angle (float v)  { _angle1 = v; }
    void set_direc (float v)  { _direc1 = v; }
    void set_xtalk (bool  b)  { _xtalk  = b; }
    void set_mono  (bool  b)  { _mono   = b; }

private:
 
    int lincheck (float& v, float& v1)
    {
	float d;
        d = v1 - v;
        if (fabsf (d) < 1e-3f) return 0;
        else if (d >  0.05f) v += 0.05f;
        else if (d < -0.05f) v -= 0.05f;
        else v = v1;
	return 1;
    }

    int circheck (float& v, float& v1)
    {
	float d;
        d = v1 - v;
        d -= floorf (d + 0.5f);
        if (fabsf (d) < 1e-3f) return 0;
        else if (d >  0.02f) v += 0.02f;
        else if (d < -0.02f) v -= 0.02f;
        else v = v1;
        v -= floorf (v);
	return 1;
    }

    float diff (int n, float v, float v1)
    {
	float d;
        d = (v1 - v) / n;
	return (fabsf (d) < 1e-6f) ? 0 : d;
    }

    int        _norm;     // B-form norm
    float      _azim;     // azimuth
    float      _elev;     // elevation
    float      _angle;    // mic angle
    float      _direc;    // mic directivity
    bool       _xtalk;    // enable LF crosstalk
    bool       _mono;     // mono output
    int        _norm1;    // target format
    float      _azim1;    // target azimuth  
    float      _elev1;    // target elevation     
    float      _angle1;   // target mic angle
    float      _direc1;   // target mic directivity
    float      _csw;      // matrix elements
    float      _csx;
    float      _csy;
    float      _csz;
    float      _cdx;
    float      _cdy;
    float      _ww;       // crosstalk filter omega
    float      _zs;       // sum channel filter state
    float      _zd;       // diff channel filter state
};


#endif
