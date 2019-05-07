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


#include <X11/keysym.h>
#include "textmat.h"


typedef X_textip * X_textip_p;


X_textmat::X_textmat (X_window *parent, X_callback *callb, X_textmat_style *style,
                      int xp, int yp, int nrow, int ncol, int *xcs, int *ycs, int *len) :
    X_window (parent, xp, yp, 100, 100, 0),
    _callb (callb),
    _style (style),
    _row (-1),
    _col (-1),
    _nrow (nrow),
    _ncol (ncol),
    _enab (false),
    _focus (0)    
{
    int       r, c, x, y, dx, dy, nk, *px, *py, *pk;
    X_textip  *T;

    _tstyle.font = style->font;
    _tstyle.color.normal.bgnd = style->color.normal.bg;
    _tstyle.color.normal.text = style->color.normal.fg;
    _tstyle.color.focus.bgnd  = style->color.focus.bg;
    _tstyle.color.focus.text  = style->color.focus.fg;
    _tstyle.color.focus.line  = style->color.line;
    _tstyle.color.shadow.bgnd = style->color.grid;
    _tstyle.color.shadow.lite = style->color.grid;
    _tstyle.color.shadow.dark = style->color.grid;

    _textips = new X_textip_p [nrow * ncol];
    T = 0;
    x = 0;
    y = 0;
    py = ycs;
    dy = *py++;
    for (r = 0; r < nrow; r++)
    {
	x = 0;
        px = xcs;
        dx = *px++;
        pk = len; 
        nk = *pk++;
        for (c = 0; c < ncol; c++)
	{
	    T = new X_textip (this, this, &_tstyle, x, y, dx, dy, nk, T);
            T->x_map (); 
	    _textips [c + r * _ncol] = T;
            x += dx - 1;
	    if (*px) dx = *px++;
	    if (*pk) nk = *pk++;
	} 
        y += dy - 1;
        if (*py) dy = *py++;
    }
    _xs = x + 1;
    _ys = y + 1;
    x_resize (_xs, _ys);
}


X_textmat::~X_textmat (void)
{
    delete[] _textips;
}


const char *X_textmat::text (int r, int c)
{
    return _textips [c + r * _ncol]->text ();
}


void X_textmat::clear (void)
{
    for (int i = 0; i < _nrow * _ncol; i++) _textips [i]->set_text ("");
}


void X_textmat::deselect (void)
{
    if (_focus)
    {
	XSetInputFocus (dpy (), PointerRoot, None, CurrentTime);
        _row = -1;
        _col = -1;
        _focus = 0;
    }
}


void X_textmat::select (int r, int c)
{
    if (_enab)
    {
	_row = r;
	_col = c;
        _focus = _textips [c + r * _ncol];
        _focus->enable ();
        _focus->clear_modified ();
        _focus->callb_modified ();
	if (_callb) _callb->handle_callb (CB_BASE + SEL, this, 0);
    }
}


void X_textmat::set_color (long bg, XftColor *fg)  
{
    for (int i = 0; i < _nrow * _ncol; i++) _textips [i]->set_color (bg, fg);
}


void X_textmat::set_color (int r, int c, long bg, XftColor *fg)  
{
    _textips [c + r * _ncol]->set_color (bg, fg);
}


void X_textmat::set_align (int a)  
{
    for (int i = 0; i < _nrow * _ncol; i++) _textips [i]->set_align (a);
}


void X_textmat::set_text (int r, int c, const char *t)
{
    _textips [c + r * _ncol]->set_text (t ? t : "");
}


void X_textmat::handle_event (XEvent *E)
{
}


void X_textmat::handle_callb (int type, X_window *W, XEvent *E)
{
    X_textip     *T = (X_textip *) W;
    int           r, c;

    switch (type)
    {
    case X_callback::TEXTIP | X_textip::BUT:
    {
        XButtonEvent *B = (XButtonEvent *) E;
        _key = 0;
        if (B)
	{
	    _but = B->button;
	    _mod = B->state;
	}
        else _but = _mod = 0;
        if (find_cell ((X_textip *) W, &r, &c))
	{
	    if (_focus && B && (_mod & ShiftMask))
	    {
		T->set_text (_focus->text ());
		select (r, c);
                if (_callb) _callb->handle_callb (CB_BASE + MOD, this, 0);
	    }
	    else if (_enab) select (r, c);
	    else if (_callb) _callb->handle_callb (CB_BASE + BUT, this, 0);
	}
        break;
    }
    case X_callback::TEXTIP | X_textip::KEY:
    {
        XKeyEvent *K = (XKeyEvent *) E;
        _but = 0;
        if (K)
	{
	    _key = T->key (); 
	    _mod = K->state;
	}
	else _key = _mod = 0;
    	switch (T->key ())
	{
        case XK_Up:
	    if (_row == 0) break;
            select (_row - 1, _col); 
	    if (K && (_mod & ShiftMask))
	    {
		_focus->set_text (T->text ());
                if (_callb) _callb->handle_callb (CB_BASE + MOD, this, 0);
	    }
	    break;
        case XK_Down:
	    if (_row == _nrow - 1) break;
            select (_row + 1, _col); 
	    if (K && (_mod & ShiftMask))
	    {
                _focus->set_text (T->text ());
                if (_callb) _callb->handle_callb (CB_BASE + MOD, this, 0);
	    }
	    break;
	}
        break;
    }
    case X_callback::TEXTIP | X_textip::MODIF:
    {
        if (_callb) _callb->handle_callb (CB_BASE + MOD, this, 0);
	break;
    }
    }
}


int X_textmat::find_cell (X_textip *T, int *rr, int *cc)
{
    int r, c; 

    for (r = 0; r < _nrow; r++)
    {
        for (c = 0; c < _ncol; c++)
        {
	    if (_textips [c + r * _ncol] == T) 
	    {
		*rr = r;
		*cc = c;
		return 1;
	    }
	}
    }
    *rr = -1;
    *cc = -1;
    return 0;
}
