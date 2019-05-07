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


#ifndef __KMETER_H
#define	__KMETER_H


#include <clxclient.h>


class Kmeter : public X_window
{
public:

    enum { HOR, VER };
    enum { K20, K14 };
    enum { LINEW = 6 };

    Kmeter (X_window *parent, int xpos, int ypos, int geom, int kval);
    ~Kmeter (void);
    Kmeter (const Kmeter&);
    Kmeter& operator=(const Kmeter&);

    void update (float v1, float v2);

    int xs (void) const { return _xs; }
    int ys (void) const { return _ys; }

    XImage *scale (void) const
    {
	if (_kval == K20) return (_geom == HOR) ? _k20_scaleH : _k20_scaleV;
        else              return (_geom == HOR) ? _k14_scaleH : _k14_scaleV;
    }

    static int load_images (X_display *disp, const char *path);

    static XImage *_k20_scaleH;
    static XImage *_k20_scaleV;
    static XImage *_k14_scaleH;
    static XImage *_k14_scaleV;

private:

    int mapk20 (float v);
    int mapk14 (float v);

    int        _geom;
    int        _kval;
    int        _xs;
    int        _ys;
    int        _kr;
    int        _kp;
    int        _dp;
    Pixmap     _pixm;
    XImage    *_imag0;
    XImage    *_imag1;

    static unsigned long _peakcol;
    static XImage *_k20_meterH0;
    static XImage *_k20_meterH1;
    static XImage *_k20_meterV0;
    static XImage *_k20_meterV1;
    static XImage *_k14_meterH0;
    static XImage *_k14_meterH1;
    static XImage *_k14_meterV0;
    static XImage *_k14_meterV1;
};


#endif
