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


#ifndef __CONFWIN_H
#define	__CONFWIN_H


#include <clxclient.h>
#include "textmat.h"
#include "filewin.h"
#include "abconfig.h"


class Confwin : public X_window, public X_callback
{
public:

    Confwin (X_rootwin *parent, X_callback *xcbh, X_resman *xres, int xp, int yp);
    ~Confwin (void);

    void open (ABconfig *);
    void close (void);
    void handle_time (void);
    void applyrep (int);

    ABconfig *config (void) { return _config; }

    Confwin (const Confwin&);           // disabled, not to be used
    Confwin& operator=(const Confwin&); // disabled, not to be used

private:

    enum { XSIZE = 755, YSIZE = 590 };
    enum { B_LOAD, B_SAVE, B_NEW, B_CANC, B_APPL,
           B_LFEQ, B_MATR, B_CONV, B_PMEQ
         };

    void handle_event (XEvent *);
    void handle_callb (int type, X_window *W, XEvent *E);
    void clmesg (XClientMessageEvent *E);
    void makegui (void);
    int  conf2disp (ABconfig *);
    int  disp2conf (ABconfig *);
    int  checkpar (X_textmat *M, int r, int c, float *v, float v0, float v1);

    X_callback     *_xcbh;
    X_resman       *_xres;
    Atom            _atom;
    Filewin        *_filewin;
    int             _format;
    ABconfig       *_config;

    X_textip       *_t_stat;
    X_textip       *_t_comm;
    X_textip       *_t_mici;
    X_textmat      *_m_lfeq;
    X_textmat      *_m_scal;
    X_textip       *_t_conv;
    X_textmat      *_m_hfeq;
    X_textmat      *_m_eqgc;

    X_tbutton      *_b_load;
    X_tbutton      *_b_save;
    X_tbutton      *_b_new;
    X_tbutton      *_b_canc;
    X_tbutton      *_b_appl;

    X_tbutton      *_b_lfeq;
    X_tbutton      *_b_matr;
    X_tbutton      *_b_conv;
    X_tbutton      *_b_pmeq;

    char            _file [1024];
};


#endif
