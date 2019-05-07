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
#include "styles.h"
#include "global.h"
#include "bformat.h"
#include "mainwin.h"


static const char *laba [4] = { "LF", "RF", "LB", "RB" };
static const char *labb [4] = { "W", "X", "Y", "Z" };


Mainwin::Mainwin (X_rootwin *parent, X_resman *xres, int xp, int yp, Jclient *jclient) :
    A_thread ("Main"),
    X_window (parent, xp, yp, 100, 100, XftColors [C_MAIN_BG]->pixel),
    _xres (xres),
    _stop (false),
    _jclient (jclient)
{
    char        s [1024];
    const char  *p;
    X_hints     H;
  
    _atom = XInternAtom (dpy (), "WM_DELETE_WINDOW", True);
    XSetWMProtocols (dpy (), win (), &_atom, 1);
    _atom = XInternAtom (dpy (), "WM_PROTOCOLS", True);

    sprintf (s, "TETRAPROC - Tetrahedral Microphone Processor - %s  [ %s ]", VERSION, jclient->jname ());
    H.position (xp, yp);
    H.size (1000, 170);
    H.minsize (1000, 170);
    H.maxsize (1000, 170);
    H.rname (xres->rname ());
    H.rclas (xres->rclas ());
    x_apply (&H); 
    x_set_title (s);
    x_resize (1000, 170);

    _confwin = new Confwin (parent, this, xres, 100, 100);
    makegui ();
    _bfnorm = Bformat::FM_FUMA;
    p = xres->get (".bformat", "fuma");
    if (p)
    {
	if (!strcmp (p, "sn3d")) _bfnorm = Bformat::FM_SN3D;
	if (!strcmp (p, "n3d"))  _bfnorm = Bformat::FM_N3D;
    }
    setformat ();

    x_map (); 
    XFlush (dpy ());

    _jclient->set_hpfil (_f_hpfil->get_val ());
    _jclient->set_mute (_kmute = 0);
    _jclient->set_invb (_kinvb = 0);
    _jclient->set_endf (0);
    _jclient->set_meter (Jclient::METER_MON);
    _jclient->set_monit (Jclient::MONIT_REC);
    _jclient->set_azim (-_f_azim->get_val ());
    _jclient->set_elev (_f_elev->get_val ());
    _jclient->set_angle (_f_angle->get_val ());
    _jclient->set_direc (_f_direc->get_val ());
    _jclient->set_xtalk (0);
    _jclient->set_mono (0);
    _jclient->set_volum (_f_volum->get_val ());

    set_time (0);
    inc_time (100000);
}

 
Mainwin::~Mainwin (void)
{
}

 
int Mainwin::process (void)
{
    int e;

    if (_stop) handle_stop ();
    e = get_event_timed ();
    switch (e)
    {
    case EV_TIME:
        handle_time ();
	break;
    }
    return e;
}


void Mainwin::handle_event (XEvent *E)
{
    switch (E->type)
    {
    case Expose:
	expose ((XExposeEvent *) E);
	break;  
 
    case ClientMessage:
        clmesg ((XClientMessageEvent *) E);
        break;
    }
}


void Mainwin::expose (XExposeEvent *E)
{
    if (E->count) return;
    if (E->window == _w_level->win ()) redraw ();
}


void Mainwin::clmesg (XClientMessageEvent *E)
{
    if (E->message_type == _atom) _stop = true;
}


void Mainwin::handle_time (void)
{
    int   i;
    float rms, dpk;

    for (i = 0; i < 4; i++)
    {
	_jclient->get_level (i, &rms, &dpk);
	_meters [i]->update (rms, dpk);
    }
    XFlush (dpy ());
    inc_time (50000);
}


void Mainwin::handle_stop (void)
{
    put_event (EV_EXIT, 1);
}


void Mainwin::handle_callb (int type, X_window *W, XEvent *E)
{
    int        i, k;
    X_button   *B;
    X_slider   *F;
    ABconfig  *C;

    switch (type)
    {
    case X_callback::BUTTON | X_button::PRESS:
	B = (X_button *) W;
	k = B->cbid ();
        switch (k)
	{
	case B_HPFIL:
 	    B->set_stat (B->stat () ^ 1);
	    _jclient->set_hpfil (B->stat () ? _f_hpfil->get_val () : 1.0f);
	    break;
	case B_MUTE1:
	case B_MUTE2:
	case B_MUTE3:
	case B_MUTE4:
 	    B->set_stat (B->stat () ^ 2);
	    _kmute ^= 1 << (k - B_MUTE1);
	    _jclient->set_mute (_kmute);
	    break;
	case B_ENDF:
 	    B->set_stat (B->stat () ^ 1);
	    _jclient->set_endf (B->stat ());
	    break;
	case B_INVX:
	case B_INVY:
	case B_INVZ:
 	    B->set_stat (B->stat () ^ 1);
	    _kinvb ^= 1 << (1 + k - B_INVX);
	    _jclient->set_invb (_kinvb);
	    break;
	case B_INPUT:
	    _b_input->set_stat (1);
	    _b_monit->set_stat (0);
	    for (i = 0; i < 4; i++)
	    {
		_t_laba [i]->x_map ();
		_t_labb [i]->x_unmap ();
	    }
	    _jclient->set_meter (Jclient::METER_INP);
	    break;
	case B_MONIT:
	    _b_input->set_stat (0);
	    _b_monit->set_stat (1);
	    for (i = 0; i < 4; i++)
	    {
		_t_laba [i]->x_unmap ();
		_t_labb [i]->x_map ();
	    }
	    _jclient->set_meter (Jclient::METER_MON);
	    break;
	case B_MREC:
	    _b_mrec->set_stat (1);
	    _b_mext->set_stat (0);
	    _jclient->set_monit (Jclient::MONIT_REC);
	    break;
	case B_MEXT:
	    _b_mrec->set_stat (0);
	    _b_mext->set_stat (1);
	    _jclient->set_monit (Jclient::MONIT_EXT);
	    break;
	case B_XTALK:
 	    B->set_stat (B->stat () ^ 1);
            _jclient->set_xtalk (B->stat ());
	    break;
	case B_MONO:
 	    B->set_stat (B->stat () ^ 2);
            _jclient->set_mono (B->stat ());
	    break;
	case B_CONF:
	    C = new ABconfig;
	    *C = _config;
	    _confwin->open (C);
	    break;
	}
	break;

    case X_callback::SLIDER | X_slider::MOVE:
    case X_callback::SLIDER | X_slider::STOP:
	F = (X_slider *) W;
	k = F->cbid ();
        switch (k)
	{
	case F_HPFIL:
	    if (_b_hpfil->stat ()) _jclient->set_hpfil (_f_hpfil->get_val ());
	    break;
	case F_AZIM:
	    _jclient->set_azim (-_f_azim->get_val ());
	    break;
	case F_ELEV:
	    _jclient->set_elev (_f_elev->get_val ());
	    break;
	case F_ANGLE:
	    _jclient->set_angle (_f_angle->get_val ());
	    break;
	case F_DIREC:
	    _jclient->set_direc (_f_direc->get_val ());
	    break;
	case F_VOLUM:
	    _jclient->set_volum (_f_volum->get_val ());
	    break;
	}
	break;

    case CB_CONF_CANC:
	C = new ABconfig;
	*C = _config;;
	_confwin->open (C);
	break;

    case CB_CONF_APPL:
	C = _confwin->config ();
	_config = *C;
	applconf ();
	break;
    }
}


void Mainwin::loadconf (void)
{
    const char *p;
    
    p = _xres->get (".config", 0);
    if (p && *p) _config.load (p);
    _config._update = ABconfig::EN_ALL;
    applconf ();
}


void Mainwin::applconf (void)
{
    int err = 0;
    int upd = _config._update & (ABconfig::EN_CONV | ABconfig::EN_MATR);
    
    if (upd) syncaudio (EV_GO_IDLE);
    if (_config._update & ABconfig::EN_LFEQ) err |= _jclient->set_lffilt (&_config);
    if (_config._update & ABconfig::EN_MATR) err |= _jclient->set_matrix (&_config);
    if (_config._update & ABconfig::EN_CONV) err |= _jclient->set_convol (&_config);
    if (_config._update & ABconfig::EN_PMEQ) err |= _jclient->set_hffilt (&_config); 
    if (upd) syncaudio (EV_GO_PROC);
    _confwin->applyrep (err);
    _t_mcid->set_text (_config._micident);
}


void Mainwin::setformat (void)
{
    _jclient->set_form (_bfnorm);
    switch (_bfnorm)
    {
    case Bformat::FM_FUMA:
        _t_form->set_text ("FuMa");
	_t_form->set_color (XftColors [C_DISP_BG]->pixel, XftColors [C_DISP_FG1]);
	break;
    case Bformat::FM_SN3D:
        _t_form->set_text ("SN3D");
	_t_form->set_color (XftColors [C_DISP_BG]->pixel, XftColors [C_DISP_FG2]);
	break;
    case Bformat::FM_N3D:
        _t_form->set_text ("N3D");
	_t_form->set_color (XftColors [C_DISP_BG]->pixel, XftColors [C_DISP_FG2]);
	break;
    default:	
        _t_form->set_text ("???");
	_t_form->set_color (XftColors [C_DISP_BG]->pixel, XftColors [C_DISP_FG2]);
    }
}


void Mainwin::syncaudio (int e)
{
    send_event (e, 1);
    e = get_event (1 << e);
}


void Mainwin::makegui (void)
{
    int i, x, y;

    x = 10; 
    y = 10;
    Bst0.size.x = 40;
    Bst0.size.y = 18;
    _b_hpfil = new X_tbutton (this, this, &Bst0, x, y + 12, "HPF", 0, B_HPFIL);
    _b_hpfil->set_stat (1);
    _b_hpfil->x_map ();
    (new X_hscale (this, &sca_hpfr, x + 55, y, 18))->x_map ();
    _f_hpfil = new X_hslider (this, this, &Fst1, &sca_hpfr, x + 55, y + 18, 13, F_HPFIL);
    _f_hpfil->set_val (20.0f);
    _f_hpfil->x_map (); 

    y += 50;
    addtext (this, &Tst1, x, y + 2, 45, 12, "Mute", -1);
    _b_mute1 = new X_tbutton (this, this, &Bst0, x +  50, y, "LF", 0, B_MUTE1);
    _b_mute1->x_map ();
    _b_mute2 = new X_tbutton (this, this, &Bst0, x +  92, y, "RF", 0, B_MUTE2);
    _b_mute2->x_map ();
    _b_mute3 = new X_tbutton (this, this, &Bst0, x + 134, y, "LB", 0, B_MUTE3);
    _b_mute3->x_map ();
    _b_mute4 = new X_tbutton (this, this, &Bst0, x + 176, y, "RB", 0, B_MUTE4);
    _b_mute4->x_map ();
    y += 23;
    addtext (this, &Tst1, x, y + 2, 45, 12, "Invert", -1);
    _b_invx = new X_tbutton (this, this, &Bst0, x +  50, y, "X", 0, B_INVX);
    _b_invx->x_map ();
    _b_invy = new X_tbutton (this, this, &Bst0, x +  92, y, "Y", 0, B_INVY);
    _b_invy->x_map ();
    _b_invz = new X_tbutton (this, this, &Bst0, x + 134, y, "Z", 0, B_INVZ);
    _b_invz->x_map ();
    Bst0.size.x = 60;
    Bst0.size.y = 18;
    _b_endf = new X_tbutton (this, this, &Bst0, x + 176, y, "EndFire", 0, B_ENDF);
    _b_endf->x_map ();
    y += 45;
    Bst0.size.x = 50;
    Bst0.size.y = 18;
    _b_conf = new X_tbutton (this, this, &Bst0, x, y, "Config", 0, B_CONF);
    _b_conf->x_map ();

    _w_level = new X_subwin (this, 285, 5, 502, 124, XftColors [C_DISP_BG]->pixel);
    y = 24;
    for (i = 0; i < 4; i++)
    {
        _meters [i] = new Kmeter (_w_level, 26, y + 4, Kmeter::HOR, Kmeter::K20);
	_meters [i]->x_map ();
	_t_laba [i] = new X_textln (_w_level, &Tst2, 2, y, 24, 14, laba [i], 0);
	_t_labb [i] = new X_textln (_w_level, &Tst2, 2, y, 24, 14, labb [i], 0);
	_t_labb [i]->x_map ();
         y += Kmeter::LINEW + 8;
    }
    _t_mcid = new X_textip (_w_level, 0, &Tst2,   2, 102, 300, 18, 64);
    _t_mcid->x_map ();
    _t_form = new X_textip (_w_level, 0, &Tst2, 420, 102,  80, 18, 64);
    _t_form->x_map ();
    _w_level->x_add_events (ExposureMask); 
    _w_level->x_map ();

    x = 805;
    y = 5;
    (new X_hscale (this, &sca_elev, x, y, 18))->x_map ();
    _f_elev = new X_hslider (this, this, &Fst1, &sca_elev, x, y + 17, 13, F_ELEV);
    _f_elev->set_val (0.0f);
    _f_elev->x_map ();
    y += 30;
    (new X_hscale (this, &sca_azim, x, y, 18))->x_map ();
    _f_azim = new X_hslider (this, this, &Fst1, &sca_azim, x, y + 17, 13, F_AZIM);
    _f_azim->set_val (0.0f);
    _f_azim->x_map (); 
    y += 30;
    (new X_hscale (this, &sca_angle, x, y, 18))->x_map ();
    _f_angle = new X_hslider (this, this, &Fst1, &sca_angle, x, y + 17, 13, F_ANGLE);
    _f_angle->set_val (0.25f);
    _f_angle->x_map ();
    y += 30;
    (new X_hscale (this, &sca_direc, x, y, 18))->x_map ();
    _f_direc = new X_hslider (this, this, &Fst1, &sca_direc, x, y + 17, 13, F_DIREC);
    _f_direc->set_val (1.00f);
    _f_direc->x_map ();

    x = 190;
    y = 142; 
    addtext (this, &Tst1, x, y, 50, 18, "Meters", 1);
    Bst0.size.x = 45;
    Bst0.size.y = 18;
    _b_input = new X_tbutton (this, this, &Bst0, x +  60, y, "Inp", 0, B_INPUT);
    _b_input->x_map ();
    _b_monit = new X_tbutton (this, this, &Bst0, x + 106, y, "Mon", 0, B_MONIT);
    _b_monit->x_map ();
    _b_monit->set_stat (1);
    x += 160;
    addtext (this, &Tst1, x, y, 50, 18, "Monit", 1);
    _b_mrec  = new X_tbutton (this, this, &Bst0, x +  60, y, "Rec", 0, B_MREC);
    _b_mrec->x_map ();
    _b_mrec->set_stat(1);
    _b_mext  = new X_tbutton (this, this, &Bst0, x + 106, y, "Ext", 0, B_MEXT);
    _b_mext->x_map ();
    x += 180;
    addtext (this, &Tst1, x, y, 50, 18, "Volume",  1);
    (new X_hscale (this, &sca_volume, x + 60, y - 12, 18))->x_map ();
    _f_volum = new X_hslider (this, this, &Fst1, &sca_volume, x + 60, y + 5, 13, F_VOLUM);
    _f_volum->set_val (-10.0f);
    _f_volum->x_map ();
    Bst0.size.x = 50;
    Bst0.size.y = 18;
    _b_xtalk = new X_tbutton (this, this, &Bst0, x + 355, y, "Xtalk", 0, B_XTALK);
    _b_xtalk->x_map ();
    _b_mono  = new X_tbutton (this, this, &Bst0, x + 406, y, "Mono", 0, B_MONO);
    _b_mono->x_map ();
}


void Mainwin::addtext (X_window *W, X_textln_style *T, int xp, int yp, int xs, int ys, const char *text, int align)
{
    (new X_textln (W, T, xp, yp, xs, ys, text, align))->x_map ();
}


void Mainwin::redraw (void)
{
    int     i, y;
    X_draw  D (dpy (), _w_level->win (), dgc (), 0);
    XImage  *I = _meters [0]->scale ();

    D.setcolor (XftColors [C_MAIN_DS]->pixel);
    D.move (0, 122);
    D.draw (0, 0);
    D.draw (500, 0);
    D.setcolor (XftColors [C_MAIN_LS]->pixel);
    D.move (501, 0);
    D.draw (501, 123);
    D.draw (1, 123);

    y = 24;
    XPutImage (dpy (), _w_level->win (), dgc (), I, 0, 4, 26, y - 14, I->width, 16);
    for (i = 1; i < 4; i++)
    {
        XPutImage (dpy (), _w_level->win (), dgc (), I, 0, 0, 26, y + 12, I->width, 4);
	y += Kmeter::LINEW + 8;
    }
    XPutImage (dpy (), _w_level->win (), dgc (), I, 0, 0, 26, y + 12, I->width, 16);

}

