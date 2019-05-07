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
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <X11/keysym.h>
#include "filewin.h"
#include "styles.h"


Filewin::Filewin (X_window *parw, X_callback *xcbh, X_resman *xres) :
    X_window (parw, XPOS, YPOS, XDEF, YDEF, XftColors [C_MAIN_BG]->pixel),
    _xcbh (xcbh),
    _xs (XDEF),
    _ys (YDEF)
{
    X_hints     H;
    const char  *p;

    _xatom = XInternAtom (dpy (), "WM_DELETE_WINDOW", True);
    XSetWMProtocols (dpy (), win (), &_xatom, 1);
    _xatom = XInternAtom (dpy (), "WM_PROTOCOLS", True);

    H.size (XDEF, YDEF);
    H.minsize (XMIN, YMIN);
    H.maxsize (XMAX, YMAX);
    H.position (XPOS, YPOS);
    x_apply (&H); 
    x_set_title ("TetraCal");

    x_add_events (ExposureMask | StructureNotifyMask);

    _tfpath = new X_textip (this, 0,     &Tst1,  0,  0, XMAX, 18, 1023);
    _mclist = new X_mclist (this, this,  &Lst0,  LM, 19, _xs - LM - RM, _ys - 94, 1000, 50000);
    _scroll  = new X_scroll  (this, this,  &Bst2,  LM, _ys - 74, _xs - LM - RM, 13, 0);
    _scroll->x_set_win_gravity (SouthWestGravity);

    _wbottom = new X_window (this, 0, _ys - 60, _xs, 60, XftColors [C_MAIN_BG]->pixel);
    _wbottom->x_set_win_gravity (SouthWestGravity);
    _wbottom->x_map ();

    Bst0.size.x = 60;
    Bst0.size.y = 18;
    _tfname  = new X_textip  (_wbottom, this,  &Tst0,  15, 10, 350, 18, 63);
    _baction = new X_tbutton (_wbottom, this,  &Bst0, 380, 10, "",         0, B_ACTION);
    _bcancel = new X_tbutton (_wbottom, this,  &Bst0, 380, 35, "Cancel",   0, B_CANCEL);
    _bnewdir = new X_tbutton (_wbottom, this,  &Bst0,  15, 35, "New dir",  0, B_NEWDIR);
    _bdelete = new X_tbutton (_wbottom, this,  &Bst0,  75, 35, "Delete",   0, B_DELETE);
    _bparent = new X_tbutton (_wbottom, this,  &Bst0, 185, 35, "Up dir",   0, B_PARENT);
    _bgohome = new X_tbutton (_wbottom, this,  &Bst0, 245, 35, "Home",     0, B_GOHOME);
    _bpreset = new X_tbutton (_wbottom, this,  &Bst0, 305, 35, "Presets",  0, B_PRESET);
    _baction->set_stat (1);

    _tfpath->x_map ();
    _tfname->x_map ();
    _mclist->x_map ();   
    _scroll->x_map ();
    _baction->x_map ();   
    _bcancel->x_map ();   
    _bnewdir->x_map ();   
    _bdelete->x_map ();   
    _bparent->x_map ();   
    _bgohome->x_map ();   
    _bpreset->x_map ();   

    p = xres->get (".presets", 0);
    if (p && *p) strcpy (_pres, p);
    else getcwd (_pres, 1024);
    p = getenv ("HOME");
    if (p && *p) strcpy (_home, p);
    else getcwd (_home, 1024);
}


Filewin::~Filewin (void)
{
    delete _mclist;
}


void Filewin::handle_callb (int type, X_window *W, _XEvent *X)
{
    X_textip  *T;
    X_button  *B;
    XButtonEvent  *E;

    E = (XButtonEvent *) X;
    switch (type)
    {
    case TEXTIP | X_textip::BUT:
        T = (X_textip *) W;
	T->enable ();
	break;
    case TEXTIP | X_textip::KEY:
        T = (X_textip *) W;
        if (T->key () == XK_Return) action ();
        if (T->key () == XK_Escape) hide (CB_FILE_CANC);
	break;
    case MCLIST | X_mclist::SEL:
        select (E);
        break;
    case BUTTON | X_button::RELSE:
        B = (X_button *) W;
	switch (B->cbid ())
        {
        case B_CANCEL:
            hide (CB_FILE_CANC);
	    break;
        case B_ACTION:
            action (); 
	    break;
        case B_NEWDIR:
            newdir (); 
	    break;
        case B_DELETE:
            remove (); 
	    break;
        case B_PARENT:
	    scandir ("..", _fext);
	    break;
        case B_GOHOME:
	    scandir (_home, _fext);
	    break;
        case B_PRESET:
	    if (E->state & ShiftMask) strcpy (_pres, _wdir);
	    else scandir (_pres, _fext);
	    break;
	}
        break;
    case SCROLL | X_scroll::MOVE:
	_mclist->move ((int)(_scroll->offs () * (_mclist->span () - _xs) + 0.5f));
        break;
    case SCROLL | X_scroll::C1LU:
    case SCROLL | X_scroll::C3RD:
	_scroll->move (-1.0f);
	_mclist->move ((int)(_scroll->offs () * (_mclist->span () - _xs) + 0.5f));
        break;
    case SCROLL | X_scroll::C1RD:
    case SCROLL | X_scroll::C3LU:
	_scroll->move (1.0f);
	_mclist->move ((int)(_scroll->offs () * (_mclist->span () - _xs) + 0.5f));
        break;
    }
}


void Filewin::handle_event (XEvent *E)
{
    switch (E->type)
    {
    case Expose:
        expose ((XExposeEvent *) E);
        break;  
    case ConfigureNotify:
        resize ((XConfigureEvent *) E);
        break;
    case ClientMessage:
        xcmesg ((XClientMessageEvent *) E);
        break;
    }
}


void Filewin::expose (XExposeEvent *E)
{
    if (E->count == 0) redraw ();
}


void Filewin::resize (XConfigureEvent *E)
{
    while (XCheckTypedWindowEvent (dpy (), win (),  ConfigureNotify, (XEvent *) E) == True);
    if ((_xs != E->width) || (_ys != E->height))
    {
	_xs = E->width;
	_ys = E->height;
        _mclist->resize (_xs - LM - RM, _ys - 94);
        _scroll->geom (0.0f, (_mclist->span () > _xs - LM - RM) ? (float) _xs / _mclist->span () : 1.0f, _xs - LM - RM);  
    }
}


void Filewin::xcmesg (XClientMessageEvent *E)
{
    if (E->message_type == _xatom) hide (CB_FILE_CANC);
}


void Filewin::redraw (void)
{
    X_draw  D (dpy (), win (), dgc (), 0);
    D.clearwin ();
    D.setfunc (GXcopy);
    D.setcolor (XftColors [C_MAIN_DS]->pixel);
    D.move (LM, 18);
    D.rdraw (_xs - LM - RM, 0);
    D.move (0, _ys - 61);
    D.rdraw (_xs, 0);
    D.setcolor (XftColors [C_MAIN_LS]->pixel);
    D.move (0, _ys - 60);
    D.rdraw (_xs, 0);
}


void Filewin::save_dialog (const char *path, const char *fext, const char *file)
{
    _cbarg = CB_FILE_SAVE;
    _baction->set_text ("Save", 0);
    _baction->redraw ();
    _tfname->set_text (file ? file : "");
    show (path, fext);
}


void Filewin::load_dialog (const char *path, const char *fext, const char *file)
{
    _cbarg = CB_FILE_LOAD;
    _baction->set_text ("Load", 0);
    _baction->redraw ();
    _tfname->set_text (file ? file : "");
    show (path, fext);
}


void Filewin::show (const char *path, const char *fext)
{
    strncpy (_fext, fext, 64);
    _fext [63] = 0;
    getcwd (_cdir, 1024);
    x_mapraised ();
    scandir (path, fext);
}


void Filewin::hide (int cbarg)
{
    chdir (_cdir); 
    x_unmap ();
    _xcbh->handle_callb (cbarg, 0, 0);
}


void Filewin::scandir (const char *path, const char *fext)
{
    DIR            *D;
    struct dirent  *E;
    struct stat     S;
    int             k, ke, kn;

    if (fext) ke = strlen (fext); 
    else ke = 0;
    if (chdir (path))
    {
        perror ("chdir");
        return;
    }
    if (getcwd (_wdir, 1024) == 0)
    {
        perror ("getcwd");
        return;
    }
    x_clear ();
    redraw ();
    if ((D = opendir (".")) == 0)
    {
        perror ("opendir");
        return;
    }
    _mclist->reset ();
    while (1)
    {
        if ((E = readdir (D)) == 0) break; 
        kn = strlen (E->d_name);
        if (stat (E->d_name, &S)) k = 3;
        else if (S_ISREG (S.st_mode))
	{
            if (ke && ((kn < ke) || strcmp (E->d_name + kn - ke, fext))) continue;
            k = 0;
	}
        else if (S_ISDIR (S.st_mode)) k = 1;
        else                          k = 2;
        _mclist->item (E->d_name, k, kn);
    }  
    closedir (D);
    _mclist->sort ();
    _mclist->show ();
    _tfpath->set_text (_wdir);
    k = _mclist->span ();
    _scroll->geom (0.0f, (k > _xs) ? (float) _xs / k : 1.0f);  
}


void Filewin::select (XButtonEvent *E)
{
    if ((_mclist->col () != 1) || (E->state & ShiftMask))
    {
	_tfname->set_text (_mclist->ptr ());
	_tfname->enable ();
    }
    else
    {
	scandir (_mclist->ptr (), _fext);
    }
}


void Filewin::action ()
{
    char        s [64];
    struct stat S;

    *s = 0;
    sscanf (_tfname->text (), "%s", s);
    if (*s == 0) hide (CB_FILE_CANC);
    else if (stat (s, &S) || S_ISREG (S.st_mode))
    {
	_tfname->set_text (s);
	hide (_cbarg);
    }
    else if (S_ISDIR (S.st_mode))
    {
        _tfname->set_text ("");
        scandir (s, _fext);
    }
}


void Filewin::newdir (void)
{
    char s [64];

    *s = 0;
    sscanf (_tfname->text (), "%s", s);
    mkdir (s, 0777);
    _tfname->set_text ("");
    scandir (s, _fext);
}


void Filewin::remove (void)
{
    char  s [64];
    struct stat S;

    *s = 0;
    sscanf (_tfname->text (), "%s", s);
    if (stat (s, &S)) return;
    if (S_ISDIR (S.st_mode)) rmdir (s); 
    else unlink (s);
    _tfname->set_text ("");
    scandir (".", _fext);
}


