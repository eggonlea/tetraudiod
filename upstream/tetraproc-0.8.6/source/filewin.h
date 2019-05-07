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


#ifndef __FILEWIN_H
#define __FILEWIN_H


#include <clxclient.h>
#include "global.h"


class Filewin : public X_window , public X_callback
{
public:

    Filewin (X_window *parw, X_callback *callb, X_resman *resman);
    ~Filewin (void);

    void save_dialog (const char *path, const char *fext, const char *file = 0);
    void load_dialog (const char *path, const char *fext, const char *file = 0);
    const char *file (void) const { return _tfname->text (); }
    const char *wdir (void) const { return _wdir; }
    const char *home (void) const { return _home; }
    const char *pres (void) const { return _pres; }
 
private:

    enum { LOAD, SAVE };
    enum { B_CANCEL, B_ACTION, B_NEWDIR, B_DELETE, B_PARENT, B_GOHOME, B_PRESET };
    enum
    {
	XDEF = 600, YDEF = 400,
	XMIN = 450, YMIN = 300,
	XMAX = 900, YMAX = 600,
	XPOS = 150, YPOS = 150,
	LM = 4, RM = 4
    };

    virtual void handle_event (XEvent *xe);
    virtual void handle_callb (int, X_window*, _XEvent*);

    void expose (XExposeEvent *E);
    void resize (XConfigureEvent *E);
    void xcmesg (XClientMessageEvent *E);
    void redraw (void);
    void scandir (const char *path, const char *fext);
    void newdir (void);
    void remove (void);
    void select (XButtonEvent *E);
    void action (void);
    void show (const char *path, const char *fext);
    void hide (int cbarg);

    Atom         _xatom;
    X_callback  *_xcbh;
    int          _xs;
    int          _ys;
    int          _cbarg;
    char         _fext [64];
    char         _cdir [1024];
    char         _wdir [1024];
    char         _home [1024];
    char         _pres [1024];
    X_mclist    *_mclist;
    X_window    *_wbottom;
    X_scroll    *_scroll;
    X_textip    *_tfpath; 
    X_textip    *_tfname;
    X_tbutton   *_bcancel;
    X_tbutton   *_baction;
    X_tbutton   *_bnewdir;
    X_tbutton   *_bdelete;
    X_tbutton   *_bparent;
    X_tbutton   *_bgohome;
    X_tbutton   *_bpreset;
};


#endif
