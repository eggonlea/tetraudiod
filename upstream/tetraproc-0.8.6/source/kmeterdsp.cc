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
#include "kmeterdsp.h"


int    Kmeterdsp::_hold; 
float  Kmeterdsp::_fall; 
float  Kmeterdsp::_omega; 



Kmeterdsp::Kmeterdsp (void)
{
    reset ();
}


Kmeterdsp::~Kmeterdsp (void)
{
}


void Kmeterdsp::reset (void)
{
    _z1 = _z2 = 0;
    _rms = 0;
    _dpk = 0;
    _cnt = 0;
    _flag = false;
}


void Kmeterdsp::process (float *p, int n)
{
    // Called by JACK's process callback.
    //
    // p : pointer to sample buffer
    // n : number of samples to process

    float  s, t, z1, z2;

    if (_flag) // Display thread has read the rms value.
    {
	_rms  = 0;
	_flag = 0;
    }

    // Get filter state.
    z1 = _z1;
    z2 = _z2;

    // Process n samples. Find digital peak value for this
    // period and perform filtering. The second filter is
    // evaluated only every 4th sample - this is just an
    // optimisation.
    t = 0;
    n /= 4;  // Loop is unrolled by 4.
    while (n--)
    {
	s = *p++;
	s *= s;
	if (t < s) t = s;             // Update digital peak.
	z1 += _omega * (s - z1);      // Update first filter.
	s = *p++;
	s *= s;
	if (t < s) t = s;             // Update digital peak.
	z1 += _omega * (s - z1);      // Update first filter.
	s = *p++;
	s *= s;
	if (t < s) t = s;             // Update digital peak.
	z1 += _omega * (s - z1);      // Update first filter.
	s = *p++;
	s *= s;
	if (t < s) t = s;             // Update digital peak.
	z1 += _omega * (s - z1);      // Update first filter.
        z2 += 4 * _omega * (z1 - z2); // Update second filter.
    }
    t = sqrtf (t);

    // Save filter state. The added constants avoid denormals.
    _z1 = z1 + 1e-20f;
    _z2 = z2 + 1e-20f;

    // Adjust RMS value and update maximum since last read().
    s = sqrtf (2 * z2);
    if (s > _rms) _rms = s;

    // Digital peak hold and fallback.
    if (t > _dpk)
    {
	// If higher than current value, update and set hold counter.
	_dpk = t;
	_cnt = _hold;
    }
    else if (_cnt) _cnt--; // else decrement counter if not zero,
    else
    {
        _dpk *= _fall;     // else let the peak value fall back,
	_dpk += 1e-10f;    // and avoid denormals.
    }
}


void Kmeterdsp::read (float *rms, float *dpk)
{
    // Called by display process approx. 30 times per second.
    //
    // Returns highest _rms value since last call, 
    // and current _dpk value.

    *rms = _rms;
    *dpk = _dpk;
    _flag = true; // Resets _rms in next process().
}


void Kmeterdsp::init (int fsamp, int fsize, float hold, float fall)
{
    // Called by initialisation code.
    //
    // fsamp = sample frequency
    // fsize = period size
    // hold  = peak hold time, seconds
    // fall  = peak fallback rate, dB/s

    float t;

    _omega = 9.72f / fsamp;                    // ballistic filter coefficient
    t = (float) fsize / fsamp;                 // period time in seconds
    _hold = (int)(hold / t + 0.5f);            // number of periods to hold peak
    _fall = powf (10.0f, -0.05f * fall * t);   // per period fallback multiplier
}
