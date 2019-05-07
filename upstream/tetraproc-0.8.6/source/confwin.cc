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
#include <unistd.h>
#include "styles.h"
#include "global.h"
#include "confwin.h"



static int dx [2] = { 71, 0 };
static int dy [2] = { 19, 0 };
static int nk [2] = { 11, 0 };
static const char *lffmt [3] = { "%7.1lf", "%5.3lf", "%5.2lf" };
static const char *pmfmt [3] = { "%7.0lf", "%5.3lf", "%5.2lf" };


Confwin::Confwin (X_rootwin *parent, X_callback *xcbh, X_resman *xres, int xp, int yp) :
    X_window (parent, xp, yp, XSIZE, YSIZE, XftColors [C_MAIN_BG]->pixel),
    _xcbh (xcbh),
    _xres (xres),
    _format (0),
    _config (0)
{
    X_hints  H;

    _atom = XInternAtom (dpy (), "WM_DELETE_WINDOW", True);
    XSetWMProtocols (dpy (), win (), &_atom, 1);
    _atom = XInternAtom (dpy (), "WM_PROTOCOLS", True);

    makegui ();
    _filewin = new Filewin (parent, this, xres);

    H.position (xp, yp);
    H.size (XSIZE, YSIZE);
    H.minsize (XSIZE, YSIZE);
    H.maxsize (XSIZE, YSIZE);
    x_apply (&H);
    x_set_title ("Tetraproc - Configuration");
}

 
Confwin::~Confwin (void)
{
}

 
void Confwin::handle_event (XEvent *E)
{
    switch (E->type)
    {
    case ClientMessage:
	clmesg ((XClientMessageEvent *) E);
	break;
    }
}


void Confwin::clmesg (XClientMessageEvent *E)
{
    if (E->message_type == _atom) close ();
}


void Confwin::open (ABconfig *C)
{
    delete _config;
    _config = C;
    x_mapraised ();
    conf2disp (C);
}


void Confwin::close (void)
{
    delete (_config);
    _config = 0;
    x_unmap ();
}


void Confwin::handle_callb (int type, X_window *W, XEvent *E)
{
    X_button   *B;
    X_textip   *T;
    X_textmat  *M;

    switch (type)
    {
    case X_callback::BUTTON | X_button::PRESS:
    {
        B = (X_button *) W;
        switch (B->cbid ())
	{
	case B_LOAD:
  	    _filewin->load_dialog (".", "tetra", "");
	    break;
	case B_SAVE:
	    if (disp2conf (_config)) _t_stat->set_text ("Configuration error");
	    else _filewin->save_dialog (".", "tetra", _file);
	    break;
        case B_NEW:
 	    _b_save->set_stat (0);
	    _b_appl->set_stat (2);
	    _config->reset ();
	    _config->_update = ABconfig::EN_ALL;
	    conf2disp (_config);
	    _t_stat->set_text ("");
	    *_file = 0;
	    break;
	case B_CANC:
	    if (_config)
	    {
		delete _config;
		_config = 0;
		_xcbh->handle_callb (CB_CONF_CANC, 0, 0);
		_b_save->set_stat (0);
		_b_appl->set_stat (0);
	    }
	    break;
        case B_APPL:
            if (_config->_update)
	    {
		if (disp2conf (_config)) _t_stat->set_text ("Configuration error");
		else _xcbh->handle_callb (CB_CONF_APPL, 0, 0);
	    }
	    break;

        case B_LFEQ:
	    B->set_stat (B->stat () ^ 1);
	    _b_save->set_stat (2);
	    _b_appl->set_stat (2);
	    _config->_update |= ABconfig::EN_LFEQ;
	    break;

        case B_MATR:
	    B->set_stat (B->stat () ^ 1);
	    _b_conv->set_stat (0);
            _b_save->set_stat (2);
	    _b_appl->set_stat (2);
	    _config->_update |= ABconfig::EN_MATR | ABconfig::EN_CONV;
	    break;

        case B_CONV:
	    B->set_stat (B->stat () ^ 1);
	    _b_matr->set_stat (0);
 	    _b_save->set_stat (2);
	    _b_appl->set_stat (2);
	    _config->_update |= ABconfig::EN_MATR | ABconfig::EN_CONV;
	    break;

        case B_PMEQ:
	    B->set_stat (B->stat () ^ 1);
 	    _b_save->set_stat (2);
	    _b_appl->set_stat (2);
	    _config->_update |= ABconfig::EN_PMEQ;
	    break;
	}
	break;
    }
    case X_callback::TEXTIP | X_textip::BUT:
    {
        T = (X_textip *) W;
        T->enable ();
	T->callb_modified ();
	break;
    }
    case X_callback::TEXTIP | X_textip::MODIF:
    {
        T = (X_textip *) W;
	_b_save->set_stat (2);
	if (T == _t_conv)
	{
	    _config->_update |= ABconfig::EN_CONV;
	    _t_conv->set_color (Mst1.color.modif.bg, Mst1.color.modif.fg);
   	    _b_appl->set_stat (2);
	}
	break;
    }
    case X_textmat::CB_BASE | X_textmat::BUT:
    {
        M = (X_textmat *) W;
	break;
    }
    case X_textmat::CB_BASE | X_textmat::SEL:
    {
        break;
    }
    case X_textmat::CB_BASE | X_textmat::MOD:
    {
        M = (X_textmat *) W;
	M->set_modif (M->row (), M->col ()); 
	_b_save->set_stat (2);
	_b_appl->set_stat (2);
	if      (M == _m_lfeq) _config->_update |= ABconfig::EN_LFEQ; 
	else if (M == _m_scal) _config->_update |= ABconfig::EN_MATR; 
	else if (M == _m_hfeq) _config->_update |= ABconfig::EN_PMEQ; 
	else if (M == _m_eqgc) _config->_update |= ABconfig::EN_PMEQ; 
        break;
    }
    case CB_FILE_LOAD:
    {	
	chdir (_filewin->wdir ());
	if (_config->load (_filewin->file ()))
	{
	    _b_save->set_stat (0);
            _t_stat->set_text ("Load failed");
	}
	else
	{
	    conf2disp (_config);
	    _b_save->set_stat (0);
	    _b_appl->set_stat (2);
	    _t_stat->set_text (_filewin->file ());
	    strcpy (_file, _filewin->file ()); 
	    _config->_update = ABconfig::EN_ALL;
	}
	break;
    }
    case CB_FILE_SAVE:
    {
	chdir (_filewin->wdir ());
        if (_config->save (_filewin->file ()))
	{
            _t_stat->set_text ("Save failed");

	}
	else
	{
	    _b_save->set_stat (0);
	    _t_stat->set_text (_filewin->file ());
	    strcpy (_file, _filewin->file ()); 
	}
	break;
    }
    default:
	;
    }
}


int Confwin::conf2disp (ABconfig *C)
{
    int  i, j;
    char s [64];

    _t_comm->set_text (C->_descript);
    _t_mici->set_text (C->_micident);
    _b_lfeq->set_stat ((C->_enable & ABconfig::EN_LFEQ) ? 1 : 0);
    _b_matr->set_stat ((C->_enable & ABconfig::EN_MATR) ? 1 : 0);
    _b_conv->set_stat ((C->_enable & ABconfig::EN_CONV) ? 1 : 0);
    _b_pmeq->set_stat ((C->_enable & ABconfig::EN_PMEQ) ? 1 : 0);
    _m_lfeq->set_color (Mst1.color.normal.bg, Mst1.color.normal.fg);
    for (j = 0; j < 3; j++) 
    {
        sprintf (s, lffmt [j], C->_lffilt [j]);
        _m_lfeq->set_text (0, j, s);
    }
    _m_scal->set_color (Mst1.color.normal.bg, Mst1.color.normal.fg);
    for (i = 0; i < 4; i++)
    {
	for (j = 0; j < 4; j++) 
	{
            sprintf (s, "%8.4lf", C->_matrix [i][j]);
            _m_scal->set_text (i, j, s);
	}
    }
    _t_conv->set_color (Mst1.color.normal.bg, Mst1.color.normal.fg);
    _t_conv->set_text (C->_convfile);
    _m_hfeq->set_color (Mst1.color.normal.bg, Mst1.color.normal.fg);
    for (i = 0; i < 4; i++)
    {
	for (j = 0; j < 9; j++) 
	{
            sprintf (s, pmfmt [j % 3], C->_hffilt [i][j]);
            _m_hfeq->set_text (3 * i + (j / 3), j % 3, s);
	}
    }
    _m_eqgc->set_color (Mst1.color.normal.bg, Mst1.color.normal.fg);
    for (j = 0; j < 4; j++)
    {
        sprintf (s, pmfmt [2], C->_eqgain [j]);
        _m_eqgc->set_text (0, j, s);
    }

    return 0;
}


int Confwin::disp2conf (ABconfig *C)
{
    int   i, j, err;

    C->_enable = 0;
    if (_b_lfeq->stat ()) C->_enable |= ABconfig::EN_LFEQ;
    if (_b_matr->stat ()) C->_enable |= ABconfig::EN_MATR;
    if (_b_conv->stat ()) C->_enable |= ABconfig::EN_CONV;
    if (_b_pmeq->stat ()) C->_enable |= ABconfig::EN_PMEQ;

    strcpy (C->_descript, _t_comm->text ());
    strcpy (C->_micident, _t_mici->text ());

    err = checkpar (_m_lfeq, 0, 0, &C->_lffilt [0], 1.0f, 24e3f)
        | checkpar (_m_lfeq, 0, 1, &C->_lffilt [1], 0.1f, 10.0f)
        | checkpar (_m_lfeq, 0, 2, &C->_lffilt [2], -30.0f, 30.0f);

    for (i = 0; i < 4; i++)
    {
	for (j = 0; j < 4; j++)
	{
            err |= checkpar (_m_scal, i, j, &C->_matrix [i][j], -10.0f, 10.0f);
	}
    }

    *(C->_convfile) = 0;
    if ((sscanf (_t_conv->text (), "%s", C->_convfile) != 1) && (C->_enable & ABconfig::EN_CONV))
    {
        _t_conv->set_color (Mst1.color.error.bg, Mst1.color.error.fg);
        err = 1;
    }  

    for (i = 0; i < 4; i++)
    {
	j = 3 * i;
        err |= checkpar (_m_hfeq, j,     0, &C->_hffilt [i][0], 1.0f, 24e3f)
            |  checkpar (_m_hfeq, j,     1, &C->_hffilt [i][1], 0.1f, 10.0f)
            |  checkpar (_m_hfeq, j,     2, &C->_hffilt [i][2], -30.0f, 30.0f)
            |  checkpar (_m_hfeq, j + 1, 0, &C->_hffilt [i][3], 1.0f, 24e3f)
            |  checkpar (_m_hfeq, j + 1, 1, &C->_hffilt [i][4], 0.1f, 10.0f)
            |  checkpar (_m_hfeq, j + 1, 2, &C->_hffilt [i][5], -30.0f, 30.0f)
            |  checkpar (_m_hfeq, j + 2, 0, &C->_hffilt [i][6], 1.0f, 24e3f)
            |  checkpar (_m_hfeq, j + 2, 1, &C->_hffilt [i][7], 0.1f, 10.0f)
            |  checkpar (_m_hfeq, j + 2, 2, &C->_hffilt [i][8], -30.0f, 30.0f)
            |  checkpar (_m_eqgc, i, 0, &C->_eqgain [i],    -10.0f, 10.0f);
    }

    return err;
}


int Confwin::checkpar (X_textmat *M, int r, int c, float *v, float v0, float v1)
{
    if ((sscanf (M->text (r, c), "%f", v) != 1) || (*v < v0) || (*v > v1))
    {
	M->set_error (r, c);
        return 1;
    }
    return 0;
}


void Confwin::applyrep (int err)
{
    if (err)
    {
	_t_stat->set_text ("APPLY FAILED");
	_b_appl->set_stat (3);
    }
    else
    {
	_t_stat->set_text ("OK");
	_b_appl->set_stat (0);
	if (_config)
	{
    	    XSetInputFocus (dpy (), PointerRoot, None, CurrentTime);
	    conf2disp (_config);
	    _config->_update = 0;
	}
    }
}


void Confwin::makegui (void)
{
    X_window  *W;
    int        y;    

    Bst0.size.x = 70;
    Bst0.size.y = 18;
    _b_load = new X_tbutton (this, this, &Bst0,   0, 0, "Load",   0, B_LOAD);   
    _b_load->x_map ();
    _b_save = new X_tbutton (this, this, &Bst0,  70, 0, "Save",   0, B_SAVE);   
    _b_save->x_map ();
    _b_new  = new X_tbutton (this, this, &Bst0, 140, 0, "New",    0, B_NEW);   
    _b_new->x_map ();
    _b_canc = new X_tbutton (this, this, &Bst0, 210, 0, "Cancel", 0, B_CANC);   
    _b_canc->x_map ();
    _b_appl = new X_tbutton (this, this, &Bst0, 280, 0, "Apply",  0, B_APPL);   
    _b_appl->x_map ();
    _t_stat = new X_textip (this, 0, &Tst1, 360,  0, 300, 18, 63);
    _t_stat->x_map ();

    Bst0.size.x = 60;
    Bst0.size.y = 18;
    y = 35;
    (new X_textln (this, &Tst1,  20, y, 100, 18, "Description", -1))->x_map ();
    _t_comm = new X_textip (this, this, &Tst0, 125, y, 400, 17, 63);
    _t_comm->x_map ();
    y += 30;
    (new X_textln (this, &Tst1,  20, y, 100, 18, "Microphone ID", -1))->x_map ();
    _t_mici = new X_textip (this, this, &Tst0, 125, y, 400, 17, 63);
    _t_mici->x_map ();

    Bst0.size.x = 60;
    Bst0.size.y = 18;
    y += 35;
    (new X_textln (this, &Tst1,  20, y, 100, 18, "LF equaliser", -1))->x_map ();
    _b_lfeq = new X_tbutton (this, this, &Bst0,  125, y, "Enable",  0, B_LFEQ);   
    _b_lfeq->x_map ();
    W = new X_window (this, 200, y, 242, 45, XftColors [C_TEXT_BG]->pixel);
    W->x_map ();
    (new X_textln (W, &Tst0,   5, 20, 20, 18, "A", -1))->x_map ();
    (new X_textln (W, &Tst0,  35, 3, 50, 18, "Freq",  0))->x_map ();
    (new X_textln (W, &Tst0, 105, 3, 50, 18, "Bandw", 0))->x_map ();
    (new X_textln (W, &Tst0, 175, 3, 50, 18, "Gain",  0))->x_map ();
    _m_lfeq = new X_textmat (W, this, &Mst1, 25, 20, 1, 3, dx, dy, nk);
    _m_lfeq->enable ();
    _m_lfeq->x_map ();

    y += 65;
    (new X_textln (this, &Tst1,  20, y, 100, 18, "Scalar matrix", -1))->x_map ();
    _b_matr = new X_tbutton (this, this, &Bst0,  125, y, "Enable",  0, B_MATR);   
    _b_matr->x_map ();
    W = new X_window (this, 200, y, 312, 99, XftColors [C_TEXT_BG]->pixel);
    W->x_map ();
    (new X_textln (W, &Tst0,   5, 20, 20, 18, "W", -1))->x_map ();
    (new X_textln (W, &Tst0,   5, 38, 20, 18, "X", -1))->x_map ();
    (new X_textln (W, &Tst0,   5, 56, 20, 18, "Y", -1))->x_map ();
    (new X_textln (W, &Tst0,   5, 74, 20, 18, "Z", -1))->x_map ();
    (new X_textln (W, &Tst0,  35, 3, 50, 18, "LFU",  0))->x_map ();
    (new X_textln (W, &Tst0, 105, 3, 50, 18, "RFD", 0))->x_map ();
    (new X_textln (W, &Tst0, 175, 3, 50, 18, "LBD",  0))->x_map ();
    (new X_textln (W, &Tst0, 245, 3, 50, 18, "RBU",  0))->x_map ();
    _m_scal = new X_textmat (W, this, &Mst1, 25, 20, 4, 4, dx, dy, nk);
    _m_scal->enable ();
    _m_scal->x_map ();

    y += 119;
    (new X_textln (this, &Tst1,  20, y, 100, 18, "Convol matrix", -1))->x_map ();
    _b_conv = new X_tbutton (this, this, &Bst0,  125, y, "Enable",  0, B_CONV);   
    _b_conv->x_map ();
    _t_conv = new X_textip (this, this, &Tst0, 200, y, 400, 17, 255);
    _t_conv->x_map ();

    y += 40;
    (new X_textln (this, &Tst1,  20, y, 100, 18, "Post-matrix EQ", -1))->x_map ();
    _b_pmeq = new X_tbutton (this, this, &Bst0,  125, y, "Enable",  0, B_PMEQ);   
    _b_pmeq->x_map ();
    W = new X_window (this, 200, y, 380, 245, XftColors [C_TEXT_BG]->pixel);
    W->x_map ();
    (new X_textln (W, &Tst0,   5,  20, 20, 18, "W", -1))->x_map ();
    (new X_textln (W, &Tst0,   5,  74, 20, 18, "X", -1))->x_map ();
    (new X_textln (W, &Tst0,   5, 128, 20, 18, "Y", -1))->x_map ();
    (new X_textln (W, &Tst0,   5, 182, 20, 18, "Z", -1))->x_map ();
    (new X_textln (W, &Tst0,  35, 3, 50, 18, "Freq",  0))->x_map ();
    (new X_textln (W, &Tst0, 105, 3, 50, 18, "Bandw", 0))->x_map ();
    (new X_textln (W, &Tst0, 175, 3, 50, 18, "Gain",  0))->x_map ();
    (new X_textln (W, &Tst0, 310, 3, 50, 18, "Gcorr",  0))->x_map ();
    (new X_textln (W, &Tst0, 280, 20, 20, 18, "W", -1))->x_map ();
    (new X_textln (W, &Tst0, 280, 38, 20, 18, "X", -1))->x_map ();
    (new X_textln (W, &Tst0, 280, 56, 20, 18, "Y", -1))->x_map ();
    (new X_textln (W, &Tst0, 280, 74, 20, 18, "Z", -1))->x_map ();
    _m_hfeq = new X_textmat (W, this, &Mst1, 25, 20, 12, 3, dx, dy, nk);
    _m_hfeq->enable ();
    _m_hfeq->x_map ();
    _m_eqgc = new X_textmat (W, this, &Mst1, 300, 20, 4, 1, dx, dy, nk);
    _m_eqgc->enable ();
    _m_eqgc->x_map ();
}
