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


#ifndef __CONVOL44_H
#define __CONVOL44_H


#include <fftw3.h>


enum { CONV_OPT_SSE = 1, CONV_OPT_3DN = 2 };


class Convdata
{
public:

    Convdata (unsigned int part, unsigned int size, unsigned int opts);
    ~Convdata (void);

    void init (float *data, unsigned int size, unsigned int step, float gain);
    int get_refc (void) const { return _refc; }
    int inc_refc (void) { return ++_refc; }
    int dec_refc (void) { return --_refc; }

private:

    static void swap (fftwf_complex *p, unsigned int n);

    friend class Convol44;

    int                _refc;      // reference counter
    unsigned int       _part;      // partition size in frames
    unsigned int       _opts;      // optimization flags
    unsigned int       _npar;      // number of partitions
    float              _norm;      // gain normalization
    fftwf_complex    **_fftd;      // transformed partitions
};



class Convol44
{
public:

    Convol44 (unsigned int part, unsigned int size, unsigned int frag, unsigned int opts);
    ~Convol44 (void);

    float *wr_ptr (unsigned int i) const { return _wr_ptr [i]; }
    float *rd_ptr (unsigned int i) const { return _rd_ptr [i]; }
    int set_conv (unsigned int ip, unsigned int op, Convdata *C);
    void reset (void);
    void process (void);

private:

    void fwdfft (unsigned int j);
    void mulacc (unsigned int j, unsigned int k, fftwf_complex *q);
    void invfft (unsigned int k, fftwf_complex *q);

    // configuration
    //
    unsigned int       _part;        // partition size in frames
    unsigned int       _npar;        // number of partitions
    unsigned int       _frag;        // IO fragment size
    unsigned int       _opts;        // optimization flags
    unsigned int       _mode;        // fragments per partition
    Convdata          *_conv [4][4]; // array of Convdata pointers
    fftwf_plan         _fwd_plan;    // fftw plans
    fftwf_plan         _rev_plan;    //

    // data buffers
    //
    float             *_inp_buff;    // input buffer
    float             *_op1_buff;    // alternating output buffers
    float             *_op2_buff;    //
    fftwf_complex    **_fwd_buff;    // circular array of transformed input parts  
    fftwf_complex     *_md1_buff;    // multiplied data accumulators  
    fftwf_complex     *_md2_buff;    //

    // interface state
    //
    float             *_wr_ptr [4];   // current input pointer
    float             *_rd_ptr [4];   // current output pointer

    // processing state
    //
    unsigned int       _ifrag;
    unsigned int       _ipart;
};


#endif

