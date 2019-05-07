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


#ifndef __JCLIENT_H
#define __JCLIENT_H


#include <clthreads.h>
#include <jack/jack.h>
#include "abprocess.h"
#include "kmeterdsp.h"
#include "virtmic.h"
#include "global.h"


class Jclient : public A_thread
{
public:

    Jclient (const char *jname);
    ~Jclient (void);

    enum { METER_INP, METER_MON }; 
    enum { MONIT_REC, MONIT_EXT }; 

    const char *jname (void) const { return _jname; }

    void set_hpfil (float v) { _abproc.set_hpfil (v); }
    void set_mute (int m) { _abproc.set_mute (m); }
    void set_invb (int m) { _abproc.set_invb (m); }
    void set_endf (int m) { _abproc.set_endf (m); }
    void set_form (int k)
    {
        _abproc.set_norm (k);
	_virtmic.set_norm (k);
    }
    int  set_lffilt (ABconfig *C) { return _abproc.set_lffilt (C); }
    int  set_matrix (ABconfig *C) { return _abproc.set_matrix (C); }
    int  set_convol (ABconfig *C) { return _abproc.set_convol (C); }
    int  set_hffilt (ABconfig *C) { return _abproc.set_hffilt (C); }

    void set_meter (int k) { _meter = k; }
    void set_monit (int k) { _monit = k; }
    void set_azim  (float v) { _virtmic.set_azim (v); }
    void set_elev  (float v) { _virtmic.set_elev (v); }
    void set_angle (float v) { _virtmic.set_angle (v); }
    void set_direc (float v) { _virtmic.set_direc (v); }
    void set_xtalk (bool b)  { _virtmic.set_xtalk (b); }
    void set_mono  (bool b)  { _virtmic.set_mono (b); }
    void set_volum (float v) { _voldb1 = v; }

    void get_level (int i, float *rms, float *dpk) { _kmdsp [i].read (rms, dpk); }

private:

    enum { S_IDLE, S_PROC };

    virtual void thr_main (void) {}

    void init_jack (void);
    void init_proc (void);
    void close_jack (void);
    void jack_shutdown (void);
    int  jack_process (jack_nframes_t nframes);
    void monitor (unsigned int n, float *p [4]);
    void measure (unsigned int n, float *p [4]);

    jack_client_t  *_jack_client;
    jack_port_t    *_jack_ipports [8];
    jack_port_t    *_jack_opports [10];
    const char     *_jname;
    unsigned int    _state;
    unsigned int    _fsamp;
    unsigned int    _fsize;
    unsigned int    _psize;

    float           _vgain;
    float           _voldb0;
    float           _voldb1;
    float           _dvoldb;
    ABprocess       _abproc;
    Virtmic         _virtmic;

    int             _meter;
    int             _monit;
    Kmeterdsp       _kmdsp [4];

    static void jack_static_shutdown (void *arg);
    static int  jack_static_process (jack_nframes_t nframes, void *arg);
    static const char *ipp_names [8];
    static const char *opp_names [10];
};


#endif
