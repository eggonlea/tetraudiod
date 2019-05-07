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


#ifndef __STYLES_H
#define __STYLES_H


#include <clxclient.h>
#include "textmat.h"


enum
{
    C_MAIN_FG, C_MAIN_BG, C_MAIN_DS, C_MAIN_LS,
    C_BUTT_BG0, C_BUTT_BG1, C_BUTT_BG2, C_BUTT_BG3,C_BUTT_FG0, C_BUTT_FG1, C_BUTT_FG2, C_BUTT_FG3, 
    C_TEXT_BG,  C_TEXT_FG, C_TEXT_HL, C_TEXT_CA, C_TEXT_GR, C_TEXT_MO, C_TEXT_ER,
    C_SLID_KN, C_SLID_MK, C_DISP_BG, C_DISP_FG1, C_DISP_FG2,
    NXFTCOLORS
};

enum
{
    F_MAIN, F_BUTT, F_TEXT, F_SCAL, F_FILE, F_MATR,
    NXFTFONTS
};



extern XftColor       *XftColors [NXFTCOLORS];
extern XftFont        *XftFonts [NXFTFONTS];

extern void styles_init (X_display *disp, X_resman *xrm);
extern void styles_fini (X_display *disp);

extern X_button_style   Bst0, Bst2;
extern X_textln_style   Tst0, Tst1, Tst2, Tst3;
extern X_slider_style   Fst1;
extern X_mclist_style   Lst0; 
extern X_textmat_style  Mst1;
extern X_scale_style    sca_hpfr, sca_azim, sca_elev, sca_angle, sca_direc, sca_volume;


#endif
