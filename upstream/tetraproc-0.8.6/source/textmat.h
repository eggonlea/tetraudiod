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


#ifndef __TEXTMAT_H
#define __TEXTMAT_H


#include <clxclient.h>


class X_textmat_style
{
public:

    XftFont *font;
    struct 
    {
	struct
	{
	    unsigned long bg;
	    XftColor     *fg;
	} normal;
	struct 
	{ 
	    unsigned long bg;
	    XftColor     *fg;
	} focus;
	struct 
	{ 
	    unsigned long bg;
	    XftColor     *fg;
	} modif;
	struct 
	{ 
	    unsigned long bg;
	    XftColor     *fg;
	} error;
        unsigned long line; 
        unsigned long grid; 
    } color;
};


class X_textmat : public X_window, public X_callback
{
public:

    enum { CB_BASE = 0x020000, BUT = 0, SEL = 1, MOD = 2 };

    X_textmat (X_window *parent, X_callback *callb, X_textmat_style *style,
               int xp, int yp, int nrow, int ncol, int *xcs, int *ycs, int *len);
    virtual ~X_textmat (void);

    const char *text (int r, int c);

    void clear (void);
    void enable (void) { _enab = true; }
    void disable (void) { _enab = false; deselect (); }
    void deselect (void);
    void set_color (long bg, XftColor *fg);   
    void set_align (int a);   

    void set_color (int r, int c, long bg, XftColor *fg);   
    void set_modif (int r, int c) { set_color (r, c, _style->color.modif.bg, _style->color.modif.fg); }
    void set_error (int r, int c) { set_color (r, c, _style->color.error.bg, _style->color.error.fg); }
    void set_text (int r, int c, const char *t);
    void select (int r, int c);

    int xsize (void) const { return _xs; }
    int ysize (void) const { return _ys; }
    int nrow (void) const { return _nrow; }
    int ncol (void) const { return _ncol; }
    int row (void) const { return _row; }
    int col (void) const { return _col; }
    int but (void) const { return _but; }
    int key (void) const { return _key; }
    int mod (void) const { return _mod; }

private:

    virtual void handle_callb (int type, X_window *W, XEvent *E);
    virtual void handle_event (XEvent *E);

    int find_cell (X_textip *T, int *rr, int *cc);

    X_callback       *_callb;
    X_textmat_style  *_style; 
    int               _xs;
    int               _ys;
    int              *_xcs;
    int              *_ycs;
    int               _row;
    int               _col;
    int               _but;
    int               _key;
    int               _mod;
    int               _nrow;
    int               _ncol;
    bool              _enab;
    X_textln_style    _tstyle;
    X_textip        **_textips;
    X_textip         *_focus;
};


#endif
