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


#ifndef __ABPROCESS_H
#define __ABPROCESS_H


#include "kmeterdsp.h"
#include "abconfig.h"
#include "convol44.h"
#include "hpfilt.h"
#include "pmfilt.h"



class ABprocess
{
public:

    ABprocess (void);
    ~ABprocess (void);

    void set_hpfil (float v) { _hpconf.setfreq (v / _fsamp); }
    void set_mute  (int m) { _mute = m; }
    void set_invb  (int m) { _invb = m; }
    void set_endf  (int m) { _endf = m; }
    void set_norm  (int k) { _norm = k; }
    int  set_lffilt (ABconfig *);
    int  set_matrix (ABconfig *);
    int  set_convol (ABconfig *);
    int  set_hffilt (ABconfig *);

    void init (unsigned int fsamp, unsigned int fsize, unsigned int psize)
    {
	_fsamp = fsamp;
	_fsize = fsize;
	_psize = psize;
    }
    void reset (void);
    void process (unsigned int n, float *inp [4], float *out [4]);
    int  cerror (void) const { return _cerror; }

private:

    void measure (unsigned int n, float *p [4]);
    void checkfp (float& f, float& b, float& g);

    unsigned int    _fsamp;
    unsigned int    _fsize;
    unsigned int    _psize;

    int             _mute;
    int             _invb;
    int             _endf;
    int             _norm;
    int             _enable;
    bool            _cerror;
    float           _matrix [4][4];
    HP_conf         _hpconf;
    HP_filt         _hpfilt [4];
    PM_filt1        _lffilt [4];
    Convol44       *_convol;
    PM_filt3        _hffilt [4];
};


#endif
