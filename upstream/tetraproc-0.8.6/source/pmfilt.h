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


class PM_filt3
{
public:

    PM_filt3 (void) { reset (); }
    ~PM_filt3 (void) {}

    void init (float w1, float b1, float g1,
               float w2, float b2, float g2,
               float w3, float b3, float g3,
               float gc);
    void init (PM_filt3& F);
    void reset (void) { _z1 = _z2 = _z3 = _z4 = _z5 = _z6 = 0; }
    void process (int n, float *ip, float *op);

private:

    float _g1, _g2, _g3, _gc;            // gains
    float _c1, _c2, _c3, _c4, _c5, _c6;  // filter coefficients
    float _z1, _z2, _z3, _z4, _z5, _z6;  // delay elements 
};


#endif
