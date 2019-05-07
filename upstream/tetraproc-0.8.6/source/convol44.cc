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


#define _XOPEN_SOURCE 600 // for posix_memalign()


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "convol44.h"



extern "C" void mac_sse (fftwf_complex *A, fftwf_complex *B, fftwf_complex *S, int N);
extern "C" void mac_3dn (fftwf_complex *A, fftwf_complex *B, fftwf_complex *S, int N);



Convdata::Convdata (unsigned int part, unsigned int size, unsigned int opts) :
    _refc (0), _part (part), _opts (opts)
{
    unsigned int i;

    if (   (part < 0x000040)
        || (part > 0x010000)
        || (part & (part - 1))
        || (size > 0x400000)
        || (size > 4096 * part)) abort ();

    _npar = (size + part - 1) / part; 
    _norm = 0.5f / part;
    _fftd = new fftwf_complex * [_npar]; 
    for (i = 0; i < _npar; i++) _fftd [i] = 0;
}


Convdata::~Convdata (void)
{
    unsigned int i;

    for (i = 0; i < _npar; i++) free (_fftd [i]);
    delete[] _fftd;
}


void Convdata::init (float *data, unsigned int size, unsigned int step, float gain)
{
    unsigned int   i, j, k;
    void           *v;	
    float          *p;
    fftwf_complex  *q;
    fftwf_plan     plan;

    i = 0;
    if (data)
    {
#ifdef __APPLE__
        p = (float *) malloc (2 * _part * sizeof (float));
        q = (fftwf_complex *) malloc (sizeof (fftwf_complex));
#else
	v = 0;
        posix_memalign (&v, 16, 2 * _part * sizeof (float));
	p = (float *) v;
        posix_memalign (&v, 16, sizeof (fftwf_complex));
	q = (fftwf_complex *) v;
#endif
    	plan = fftwf_plan_dft_r2c_1d (2 * _part, p, q, FFTW_ESTIMATE);
        gain *= _norm;
	if (size > _npar * _part) size = _npar * _part;
	while (size)
	{
	    k = (size < _part) ? size : _part;
            for (j = 0; j < k; j++) p [j] = data [j * step] * gain;
	    memset (p + k, 0, (2 * _part - k) * sizeof (float));
            if (! _fftd [i])
            {
#ifdef __APPLE__
	        _fftd [i] = (fftwf_complex *) malloc ((_part + 1) * sizeof (fftwf_complex));
#else
                posix_memalign (&v, 16, (_part + 1) * sizeof (fftwf_complex));
	        _fftd [i] = (fftwf_complex *) v;
#endif
            }
            fftwf_execute_dft_r2c (plan, p, _fftd [i]);
//            if (_opts) swap (_fftd [i], _part);
            size -= k;
	    data += k * step;
            i++;
	}
        fftwf_free (p);
        fftwf_free (q);
	fftwf_destroy_plan (plan);
    }
    while (i < _npar)
    {
	free (_fftd [i]);
        _fftd [i] = 0;
        i++;
    }
}


void Convdata::swap (fftwf_complex *p, unsigned int n)
{
    float a, b;

    while (n)
    {
	a = p [2][0];
	b = p [3][0];
        p [2][0] = p [0][1];
        p [3][0] = p [1][1];
        p [0][1] = a;
        p [1][1] = b;
	p += 4;
        n -= 4;
    }
}




Convol44::Convol44 (unsigned int part, unsigned int size, unsigned int frag, unsigned int opts) :
    _part (part), _frag (frag), _opts (opts)
{
    unsigned int   i, j;
    void           *v;

    if (   (part < 0x000040)
        || (part > 0x010000)
        || (part & (part - 1))
        || ((frag != part) && (2 * frag != part) && (4 * frag != part))  
        || (size > 0x400000)
        || (size > 4096 * part)) abort ();
   
    _npar = (size + part - 1) / part;
    _mode = part / frag;
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
	{
	    _conv [i][j] = 0;
	}
    }
#ifdef __APPLE__
    _inp_buff = (float *) malloc (5 * _part * sizeof (float));
    _op1_buff = (float *) malloc (8 * _part * sizeof (float));
    _op2_buff = (float *) malloc (8 * _part * sizeof (float));
    _md1_buff = (fftwf_complex *) malloc ((_part + 1) * sizeof (fftwf_complex));
    _md2_buff = (fftwf_complex *) malloc ((_part + 1) * sizeof (fftwf_complex));
#else
    v = 0;
    posix_memalign (&v, 16, 5 * _part * sizeof (float));
    _inp_buff = (float *) v;
    posix_memalign (&v, 16, 8 * _part * sizeof (float));
    _op1_buff = (float *) v;
    posix_memalign (&v, 16, 8 * _part * sizeof (float));
    _op2_buff = (float *) v;
    posix_memalign (&v, 16, (_part + 1) * sizeof (fftwf_complex));
    _md1_buff = (fftwf_complex *) v;
    posix_memalign (&v, 16, (_part + 1) * sizeof (fftwf_complex));
    _md2_buff = (fftwf_complex *) v;
#endif
    _fwd_buff = new  fftwf_complex * [4 * _npar]; 
    for (i = 0; i < 4 * _npar; i++)
    {
#ifdef __APPLE__
	_fwd_buff [i] = (fftwf_complex *) malloc ((_part + 1) * sizeof (fftwf_complex));
#else
	posix_memalign (&v, 16, (_part + 1) * sizeof (fftwf_complex));
	_fwd_buff [i] = (fftwf_complex *) v;
#endif
    }
    _fwd_plan = fftwf_plan_dft_r2c_1d (2 * part, _inp_buff, _md1_buff, FFTW_ESTIMATE);
    _rev_plan = fftwf_plan_dft_c2r_1d (2 * part, _md1_buff, _op1_buff, FFTW_ESTIMATE);
    reset ();
}



Convol44::~Convol44 (void)
{
    unsigned int i, j;

    fftwf_destroy_plan (_fwd_plan);
    fftwf_destroy_plan (_rev_plan);
    free (_inp_buff);
    free (_op1_buff);
    free (_op2_buff);
    free (_md1_buff);
    free (_md2_buff);
    for (i = 0; i < 4 * _npar; i++) free (_fwd_buff [i]);
    delete[] _fwd_buff;
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
	{
	    if (_conv [i][j] && (_conv [i][j]->dec_refc () == 0)) delete _conv [i][j];
	}
    }
}


int Convol44::set_conv (unsigned int ip, unsigned int op, Convdata *C)
{
    if ((ip >= 4) || (op >= 4)) return -1;
    if (_conv [ip][op] && (_conv [ip][op]->dec_refc () == 0))
    {
        delete _conv [ip][op];
        _conv [ip][op] = 0;
    }
    if (C)
    {
        if (C->_part != _part) return -1; 
	C->inc_refc ();
        _conv [ip][op] = C;
    }
    return 0;
}


void Convol44::reset ()
{
    unsigned int i;

    memset (_inp_buff, 0, 5 * _part * sizeof (float));
    memset (_op1_buff, 0, 8 * _part * sizeof (float));
    memset (_op2_buff, 0, 8 * _part * sizeof (float));
    memset (_md1_buff, 0, (_part + 1) * sizeof (fftwf_complex));
    memset (_md2_buff, 0, (_part + 1) * sizeof (fftwf_complex));
    for (i = 0; i < 4 * _npar; i++)
    {
	memset (_fwd_buff [i], 0, (_part + 1) * sizeof (fftwf_complex));
    }
    _ipart = 0;
    _ifrag = 0;
    for (i = 0; i < 4; i++)
    {
	_wr_ptr [i] = _inp_buff + (i + 1) * _part - _frag;
        _rd_ptr [i] = _op2_buff + 2 * i * _part;
    } 
}


void Convol44::process ()
{
    unsigned int   i;
    float          *p;

    if (_ifrag == 0)
    {
        if (_ipart == 0) _ipart = _npar;
        _ipart--;
    }

    switch (_mode)
    {
    case 1:
	    fwdfft (3);
	    fwdfft (2);
	    fwdfft (1);
	    fwdfft (0);
	    mulacc (3, 3, _md2_buff);
	    mulacc (3, 2, _md1_buff);
	    mulacc (2, 3, _md2_buff);
	    mulacc (2, 2, _md1_buff);
	    mulacc (1, 3, _md2_buff);
	    mulacc (1, 2, _md1_buff);
	    mulacc (0, 3, _md2_buff);
	    mulacc (0, 2, _md1_buff);
	    invfft (3, _md2_buff);
	    invfft (2, _md1_buff);
	    mulacc (3, 1, _md2_buff);
	    mulacc (3, 0, _md1_buff);
	    mulacc (2, 1, _md2_buff);
	    mulacc (2, 0, _md1_buff);
	    mulacc (1, 1, _md2_buff);
	    mulacc (1, 0, _md1_buff);
	    mulacc (0, 1, _md2_buff);
	    mulacc (0, 0, _md1_buff);
	    invfft (1, _md2_buff);
	    invfft (0, _md1_buff);
	break;

    case 2:
        switch (_ifrag)
	{
	case 0:
	    fwdfft (3);
	    fwdfft (2);
	    fwdfft (1);
	    fwdfft (0);
	    mulacc (3, 3, _md2_buff);
	    mulacc (3, 2, _md1_buff);
	    mulacc (2, 3, _md2_buff);
	    mulacc (2, 2, _md1_buff);
	    mulacc (1, 3, _md2_buff);
	    mulacc (1, 2, _md1_buff);
	    mulacc (0, 3, _md2_buff);
	    mulacc (0, 2, _md1_buff);
	    for (i = 0; i < 4; i++) _wr_ptr [i] = _inp_buff + i * _part;
	    break;
	
	case 1:
	    invfft (3, _md2_buff);
	    invfft (2, _md1_buff);
	    mulacc (3, 1, _md2_buff);
	    mulacc (3, 0, _md1_buff);
	    mulacc (2, 1, _md2_buff);
	    mulacc (2, 0, _md1_buff);
	    mulacc (1, 1, _md2_buff);
	    mulacc (1, 0, _md1_buff);
	    mulacc (0, 1, _md2_buff);
	    mulacc (0, 0, _md1_buff);
	    invfft (1, _md2_buff);
	    invfft (0, _md1_buff);
	    for (i = 0; i < 4; i++) _wr_ptr [i] += _frag;
	    break;
	}
	break;

    case 4:
        switch (_ifrag)
	{
	case 0:
	    fwdfft (3);
	    fwdfft (2);
	    mulacc (3, 3, _md2_buff);
	    mulacc (3, 2, _md1_buff);
	    mulacc (2, 3, _md2_buff);
	    mulacc (2, 2, _md1_buff);
	    p = _inp_buff + 4 * _part;
 	    for (i = 0; i < 4; i++) _wr_ptr [i] =  p + i * _frag;
	    break;
	
	case 1:
	    fwdfft (1);
	    fwdfft (0);
	    mulacc (1, 3, _md2_buff);
	    mulacc (1, 2, _md1_buff);
	    mulacc (0, 3, _md2_buff);
	    mulacc (0, 2, _md1_buff);
	    p = _inp_buff;
	    for (i = 0; i < 4; i++)
	    {
	        memcpy (p, _wr_ptr [i], _frag * sizeof (float));
                _wr_ptr [i] = p + _frag;
		p += _part;
	    }
	    break;
	
	case 2:
	    invfft (3, _md2_buff);
	    invfft (2, _md1_buff);
	    mulacc (3, 1, _md2_buff);
	    mulacc (3, 0, _md1_buff);
	    mulacc (2, 1, _md2_buff);
	    mulacc (2, 0, _md1_buff);
 	    for (i = 0; i < 4; i++) _wr_ptr [i] += _frag;
	    break;
	
	case 3:
	    mulacc (1, 1, _md2_buff);
	    mulacc (1, 0, _md1_buff);
	    mulacc (0, 1, _md2_buff);
	    mulacc (0, 0, _md1_buff);
	    invfft (1, _md2_buff);
	    invfft (0, _md1_buff);
 	    for (i = 0; i < 4; i++) _wr_ptr [i] += _frag;
	    break;
	}	
	break;
    }

    if (++_ifrag == _mode)
    {
	_ifrag = 0;
	p = _op1_buff;
	_op1_buff = _op2_buff;
	_op2_buff = p;
        for (i = 0; i < 4; i++) _rd_ptr [i] = _op2_buff + 2 * i * _part;
    }
    else
    {
        for (i = 0; i < 4; i++) _rd_ptr [i] += _frag;
    }
}


void Convol44::fwdfft (unsigned int j)
{
    float          *p;
    fftwf_complex  *q;

    p = _inp_buff + j * _part;
    q = _fwd_buff [j * _npar + _ipart];
    memset (p + _part, 0, _part * sizeof (float));
    fftwf_execute_dft_r2c (_fwd_plan, p, q);
//    if (_opts) Convdata::swap (q, _part);
}


void Convol44::mulacc (unsigned int j, unsigned int k, fftwf_complex *q)
{
    unsigned int    g, h, i;
    fftwf_complex   *q1, *q2;
    Convdata        *C;

    C = _conv [j][k];
    if (C)
    {
        for (g = 0; g < _npar; g++)
	{
            q2 = C->_fftd [g];
            if (q2)
	    {
                h = g + _ipart;
                if (h >= _npar) h -= _npar;            
                q1 = _fwd_buff [h + j * _npar];
//                if      (_opts == CONV_OPT_SSE) mac_sse (q1, q2, q, _part >> 2); 
//                else if (_opts == CONV_OPT_3DN) mac_3dn (q1, q2, q, _part >> 2);
//                else
                for (i = 0; i <= _part; i++)
		{
		    q [i][0] += q1 [i][0] * q2 [i][0] - q1 [i][1] * q2 [i][1];
		    q [i][1] += q1 [i][0] * q2 [i][1] + q1 [i][1] * q2 [i][0];
		}  
	    }
	}
    }
}


void Convol44::invfft (unsigned int k, fftwf_complex *q)
{
    unsigned int   i;
    float          *p1, *p2;

    k *= 2 * _part;
    p1 = _op1_buff + k;
    p2 = _op2_buff + k + _part;
//    if (_opts) Convdata::swap (q, _part);
    fftwf_execute_dft_c2r (_rev_plan, q, p1);
    memset (q, 0, (_part + 1) * sizeof (fftwf_complex));
    for (i = 0; i < _part; i++) *p1++ += *p2++;
}


