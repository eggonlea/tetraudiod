// ----------------------------------------------------------------------------
//
//  Copyright (C) 2006-2008 Fons Adriaensen <fons@linuxaudio.org>
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


#ifndef __PMFILT_H
#define __PMFILT_H


class PM_filt1
{
public:

    PM_filt1 (void) { reset (); }
    ~PM_filt1 (void) {}

    void init (float w, float b, float g);
    void init (PM_filt1& F);
    void reset (void) { _z1 = _z2 = 0; }
    void process (int n, float *ip, float *op);

private:

    float _g;        // gain
    float _c1, _c2;  // filter coefficients
    float _z1, _z2;  // delay elements 
};


class PM_filt2
{
public:

    PM_filt2 (void) { reset (); }
    ~PM_filt2 (void) {}

    void init (float w1, float b1, float g1, float w2, float b2, float g2, float gc);
    void init (PM_filt2& F);
    void reset (void) { _z1 = _z2 = _z3 = _z4 = 0; }
    void process (int n, float *ip, float *op);

private:

    float _g1, _g2, _gc;       // gain
    float _c1, _c2, _c3, _c4;  // filter coefficients
    float _z1, _z2, _z3, _z4;  // delay elements 
};


#endif
