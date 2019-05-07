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


#ifndef __HPFILT_H
#define __HPFILT_H


class HP_conf
{
public:

    HP_conf (void) : _w0 (0), _w1 (0), _g (1), _c1 (0), _c2 (0), _c3 (0), _c4 (0) {};
    ~HP_conf (void) {}

    void setfreq (float w) { _w1 = w; }
    void process (void);

private:

    friend class HP_filt;

    float _w0;                 // current frequency
    float _w1;                 // target frequency
    float _g;                  // gain
    float _c1, _c2, _c3, _c4;  // filter coefficients
};


class HP_filt
{
public:

    HP_filt (void) { reset (); }
    ~HP_filt (void) {}

    void reset (void) { _z1 = _z2 = _z3 = _z4 = 0; }
    void process (const HP_conf *C, int n, float *ip, float *op);

private:

    float _z1, _z2, _z3, _z4;  // delay elements 
};


#endif
