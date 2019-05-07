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


#ifndef __MAINWIN_H
#define	__MAINWIN_H


#include <clxclient.h>
#include "confwin.h"
#include "kmeter.h"
#include "global.h"
#include "jclient.h"
#include "abconfig.h"



class Mainwin : public A_thread, public X_window, public X_callback
{
public:

    Mainwin (X_rootwin *parent, X_resman *xres, int xp, int yp, Jclient *jclient);
    ~Mainwin (void);
    Mainwin (const Mainwin&);
    Mainwin& operator=(const Mainwin&);

    void stop (void) { _stop = true; }
    void loadconf (void);
    void makegui (void);
    int  process (void);

private:

    enum { F_HPFIL, F_AZIM, F_ELEV, F_ANGLE, F_DIREC, F_VOLUM };
    enum { B_HPFIL, B_MUTE1, B_MUTE2, B_MUTE3, B_MUTE4, B_INVX, B_INVY, B_INVZ,
           B_ENDF, B_INPUT, B_MONIT, B_MREC, B_MEXT, B_XTALK, B_MONO, B_CONF };
    
    virtual void thr_main (void) {}

    void handle_time (void);
    void handle_stop (void);
    void handle_event (XEvent *);
    void handle_callb (int type, X_window *W, XEvent *E);
    void expose (XExposeEvent *E);
    void clmesg (XClientMessageEvent *E);
    void syncaudio (int e);
    void applconf (void);
    void setformat (void);
    void addtext (X_window *W, X_textln_style *T, int xp, int yp, int xs, int ys, const char *text, int align);
    void redraw (void);

    X_resman       *_xres;
    Atom            _atom;
    bool            _stop;
    Jclient        *_jclient;
    Confwin        *_confwin;
    ABconfig        _config;
    int             _bfnorm;
    int             _kmute;
    int             _kinvb;
    X_tbutton      *_b_hpfil;
    X_slider       *_f_hpfil;
    X_tbutton      *_b_mute1;
    X_tbutton      *_b_mute2;
    X_tbutton      *_b_mute3;
    X_tbutton      *_b_mute4;
    X_tbutton      *_b_invx;
    X_tbutton      *_b_invy;
    X_tbutton      *_b_invz;
    X_tbutton      *_b_endf;
    X_tbutton      *_b_conf;
    X_subwin       *_w_level;
    Kmeter         *_meters [4];
    X_textln       *_t_laba [4];
    X_textln       *_t_labb [4];
    X_textip       *_t_mcid;
    X_textip       *_t_form;
    X_tbutton      *_b_input;
    X_tbutton      *_b_monit;
    X_tbutton      *_b_mrec;
    X_tbutton      *_b_mext;
    X_slider       *_f_azim;
    X_slider       *_f_elev;
    X_slider       *_f_angle;
    X_slider       *_f_direc;
    X_tbutton      *_b_xtalk;
    X_tbutton      *_b_mono;
    X_slider       *_f_volum;
};


#endif
