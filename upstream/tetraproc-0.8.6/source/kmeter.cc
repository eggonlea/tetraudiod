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


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "png2img.h"
#include "kmeter.h"


unsigned long Kmeter::_peakcol = 0;
XImage *Kmeter::_k20_meterH0 = 0;
XImage *Kmeter::_k20_meterH1 = 0;
XImage *Kmeter::_k20_meterV0 = 0;
XImage *Kmeter::_k20_meterV1 = 0;
XImage *Kmeter::_k20_scaleH  = 0;
XImage *Kmeter::_k20_scaleV  = 0;
XImage *Kmeter::_k14_meterH0 = 0;
XImage *Kmeter::_k14_meterH1 = 0;
XImage *Kmeter::_k14_meterV0 = 0;
XImage *Kmeter::_k14_meterV1 = 0;
XImage *Kmeter::_k14_scaleH  = 0;
XImage *Kmeter::_k14_scaleV  = 0;


Kmeter::Kmeter (X_window *parent, int xpos, int ypos, int geom, int kval) :
    X_window (parent, xpos, ypos, LINEW, LINEW, 0),
    _geom (geom),
    _kval (kval),
    _xs (0),
    _ys (0),
    _kr (-1),
    _kp (-1),
    _dp (0),
    _pixm (0),
    _imag0 (0),
    _imag1 (0)
{
    switch (kval)
    {
    case K20:
        _imag0 = (geom == HOR) ? _k20_meterH0 : _k20_meterV0; 
        _imag1 = (geom == HOR) ? _k20_meterH1 : _k20_meterV1; 
	break;
    case K14:
        _imag0 = (geom == HOR) ? _k14_meterH0 : _k14_meterV0; 
        _imag1 = (geom == HOR) ? _k14_meterH1 : _k14_meterV1; 
	break;
    default:
	return;
    }
    if (!_imag0 || !_imag1) return;

    _xs = _ys = LINEW;
    switch (geom)
    {
    case HOR:
	_xs = _imag0->width;
	break;
    case VER:
	_ys = _imag0->height;
	break;
    default:
	return;
    }

    _pixm = XCreatePixmap (dpy (), win (), _xs, _ys, disp ()->depth ());
    if (! _pixm) return;
    XPutImage (dpy (), _pixm, dgc (), _imag0, 0, 0, 0, 0, _xs, _ys); 
    XSetWindowBackgroundPixmap (dpy (), win (), _pixm);
    x_resize (_xs, _ys);
}


Kmeter::~Kmeter (void)
{
    if (_pixm) XFreePixmap (dpy (), _pixm);
}


void Kmeter::update (float r, float p)
{
    int x, y, kr, kp, dp;

    kr = (_kval == K20) ? mapk20 (r) : mapk14 (r);
    kp = (_kval == K20) ? mapk20 (p) : mapk14 (p);
    dp = kp - kr;
    if (dp >  3) dp = 3;
    if (kp < 24) dp = 0;
    XSetForeground (dpy (), dgc (), _peakcol);
    if (_geom == HOR)
    {
        if (_dp > 0)
	{
	    x = _kp + 11;
            XPutImage (dpy (), _pixm, dgc (), _imag0, x - _dp, 0, x - _dp, 0, _dp, LINEW);
	}
	if (kr > _kr)
	{
	    x = _kr + 11;
            XPutImage (dpy (), _pixm, dgc (), _imag1, x, 0, x, 0, kr - _kr, LINEW); 
	}	    
	else if (kr < _kr)
	{
	    x = kr + 11;
            XPutImage (dpy (), _pixm, dgc (), _imag0, x, 0, x, 0, _kr - kr, LINEW); 
	}	    
	if (dp > 0)
	{
	    x = kp + 11;
            XFillRectangle (dpy (), _pixm, dgc (), x - dp, 0, dp, LINEW);
	}
    }
    else
    {
        if (_dp > 0)
	{
	    y = _ys - 11- _kp;
            XPutImage (dpy (), _pixm, dgc (), _imag0, 0, y, 0, y, LINEW, _dp);
	}
	if (kr > _kr)
	{
	    y = _ys - 11 - kr;
            XPutImage (dpy (), _pixm, dgc (), _imag1, 0, y, 0, y, LINEW, kr - _kr); 
	}	    
	else if (kr < _kr)
	{
	    y = _ys - 11 - _kr;
            XPutImage (dpy (), _pixm, dgc (), _imag0, 0, y, 0, y, LINEW, _kr - kr); 
	}	    
	if (dp > 0)
	{
	    y = _ys - 11 - kp;
            XFillRectangle (dpy (), _pixm, dgc (), 0, y, LINEW, dp);
	}
    }
    _kr = kr;
    _kp = kp;
    _dp = dp;
    x_clear ();
}


int Kmeter::mapk20 (float v)
{
    if (v < 1e-3f) return (int)(24e3f * v);
    v = log10f (v);
    if (v < -1.25f) return (int)(468.3f + v * (196 + v * 16));
    if (v > 0) v = 0;
    return (int)(448.3f + v * 160.0f);
}


int Kmeter::mapk14 (float v)
{
    if (v < 2e-3f) return (int)(12e3f * v);
    v = log10f (v) - 0.3;
    if (v < -1.25f) return (int)(468.3f + v * (196 + v * 16));
    if (v > 0) v = 0;
    return (int)(448.3f + v * 160.0f);
}

 
int Kmeter::load_images (X_display *disp, const char *path)
{
    char s [1024];

    if (_k20_meterH0 && _k20_meterH1 && _k20_scaleH) return 0;
    _peakcol = disp->whitepixel ();
    snprintf (s, 1024, "%s/k20-meterH0.png", path);
    _k20_meterH0 = png2img (s, disp, 0);
    snprintf (s, 1024, "%s/k20-meterH1.png", path);
    _k20_meterH1 = png2img (s, disp, 0);
    snprintf (s, 1024, "%s/k20-scaleH.png", path);
    _k20_scaleH = png2img (s, disp, 0);
    if (_k20_meterH0 && _k20_meterH1 && _k20_scaleH) return 0;
    return 1;
}
