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


#include <string.h>
#include <sndfile.h>
#include "jclient.h"


//-------------------------------------------------------------------------------------------------------


Jclient::Jclient (const char *jname) :
    A_thread ("Jclient"),
    _jack_client (0),
    _jname (jname),
    _state (S_IDLE)
{
    init_jack ();   
    init_proc ();
}


Jclient::~Jclient (void)
{
    if (_jack_client) close_jack ();
}


void Jclient::init_jack (void)
{
    int            i;
    jack_status_t  s;

    if ((_jack_client = jack_client_open (_jname, (jack_options_t) 0, &s)) == 0)
    {
        fprintf (stderr, "Can't connect to JACK\n");
        exit (1);
    }

    jack_set_process_callback (_jack_client, jack_static_process, (void *) this);
    jack_on_shutdown (_jack_client, jack_static_shutdown, (void *) this);

    for (i = 0; i <  8; i++)
    {
        _jack_ipports [i] = jack_port_register (_jack_client, ipp_names [i], JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
    }
    for (i = 0; i < 10; i++)
    {
        _jack_opports [i] = jack_port_register (_jack_client, opp_names [i], JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
    }

    if (jack_activate (_jack_client))
    {
        fprintf(stderr, "Can't activate JACK.\n");
        exit (1);
    }

    _jname = jack_get_client_name (_jack_client);
    _fsamp = jack_get_sample_rate (_jack_client);
    _fsize = jack_get_buffer_size (_jack_client);
    _psize = _fsize;

}


void Jclient::close_jack ()
{
    jack_deactivate (_jack_client);
    jack_client_close (_jack_client);
}


void Jclient::jack_static_shutdown (void *arg)
{
    return ((Jclient *) arg)->jack_shutdown ();
}


void Jclient::jack_shutdown (void)
{
    send_event (EV_EXIT, 1);
}


int Jclient::jack_static_process (jack_nframes_t nframes, void *arg)
{
    return ((Jclient *) arg)->jack_process (nframes);
}


void Jclient::init_proc ()
{
    _meter = METER_INP;
    _monit = MONIT_REC;
    _vgain = 0.0f;
    _voldb0 = -100.0f;
    _voldb1 = -100.0f;
    _dvoldb = 200.0f * _fsize / _fsamp;
    _abproc.init (_fsamp, _fsize, _psize);
    _virtmic.set_fsam (_fsamp);
    Kmeterdsp::init (_fsamp, _fsize, 0.5f, 15.0f);
}


int Jclient::jack_process (jack_nframes_t nframes)
{
    unsigned int  e, j;
    float         *p [4], *q [4];

    switch (e = get_event_nowait ())
    {
    case EV_GO_IDLE:
	_state = S_IDLE;
        send_event (e, 1);
	break;
    case EV_GO_PROC:
	_state = S_PROC;
        send_event (e, 1);
	break;
    }

    if (nframes != _fsize) return 0;

    for (j = 0; j < 4; j++)
    {
        p [j] = (float *) jack_port_get_buffer (_jack_ipports [j], nframes);
        q [j] = (float *) jack_port_get_buffer (_jack_opports [j], nframes);
    }

    if (_state == S_IDLE || _abproc.cerror ())
    {
        for (j = 0; j < 4; j++)
	{
	    if (_meter == METER_INP) _kmdsp [j].reset ();
            memset (q [j], 0, nframes * sizeof (float));
	}
    }
    else
    {
        if (_meter == METER_INP) measure (nframes, p);
        _abproc.process (nframes, p, q);
    }
    monitor (nframes, q);

    return 0;
}


void Jclient::monitor (unsigned int n, float *p [4])
{
    unsigned int  i, j;
    float         g, d, t, *q [4];

    // Select monitor source.
    for (j = 0; j < 4; j++) 
    {
	if (_monit == MONIT_EXT) p [j] = (float *) jack_port_get_buffer (_jack_ipports [4 + j], n);
	q [j] = (float *) jack_port_get_buffer (_jack_opports [4 + j], n);
    }

    // Update level meters.
    if (_meter == METER_MON) measure (n, p);

    // Check volume change.
    g = _vgain;
    d = _voldb1 - _voldb0;
    if (fabsf (d) > 0.1f)
    {
	if      (d >  _dvoldb) d =  _dvoldb;
        else if (d < -_dvoldb) d = -_dvoldb; 
	_voldb0 += d;
        t = (_voldb0 < -80.0f) ? 0 : powf (10.0f, 0.05f * _voldb0);
	d = (t - g) / _fsize + 1e-30f;
	_vgain = t;
    }	
    else d = 0;
   
    // Apply volume.
    for (i = 0; i < n; i++)
    {
	g += d;
	q [0][i] = p [0][i] * g;
	q [1][i] = p [1][i] * g;
	q [2][i] = p [2][i] * g;
	q [3][i] = p [3][i] * g;
    }

    // Create stereo output.
    _virtmic.process (n, q [0], q [1], q [2], q [3],
                      (float *) jack_port_get_buffer (_jack_opports [8], n),
                      (float *) jack_port_get_buffer (_jack_opports [9], n));
}


void Jclient::measure (unsigned int n, float *p [4])
{
    int i;

    for (i = 0; i < 4; i++)
    {
	_kmdsp [i].process (p [i], n);
    }
}


//-------------------------------------------------------------------------------------------------------


const char *Jclient::ipp_names [8] =
{
    "A-form.1(LF)",
    "A-form.2(RF)",
    "A-form.3(LB)",
    "A-form.4(RB)",
    "Mon-in.W",
    "Mon-in.X",
    "Mon-in.Y",
    "Mon-in.Z"
};


const char *Jclient::opp_names [10] =
{
    "B-form.W",
    "B-form.X",
    "B-form.Y",
    "B-form.Z",
    "Mon-out.W",
    "Mon-out.X",
    "Mon-out.Y",
    "Mon-out.Z",
    "Stereo.L",
    "Stereo.R"
};


//-------------------------------------------------------------------------------------------------------
