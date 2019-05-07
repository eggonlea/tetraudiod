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


#include "styles.h"


XftColor        *XftColors [NXFTCOLORS];
XftFont         *XftFonts [NXFTFONTS];
X_button_style   Bst0, Bst2;
X_textln_style   Tst0, Tst1, Tst2, Tst3;
X_slider_style   Fst1;
X_textmat_style  Mst1;
X_mclist_style   Lst0; 
X_scale_style    sca_hpfr, sca_azim, sca_elev, sca_angle, sca_direc, sca_volume;



void styles_init (X_display *disp, X_resman *xrm)
{
    XftColors [C_MAIN_BG]  = disp->alloc_xftcolor (xrm->get (".color.main.bg",   "gray30"),  0);
    XftColors [C_MAIN_FG]  = disp->alloc_xftcolor (xrm->get (".color.main.fg",   "white"  ), 0);
    XftColors [C_MAIN_LS]  = disp->alloc_xftcolor (xrm->get (".color.main.ls",   "gray50" ), 0);
    XftColors [C_MAIN_DS]  = disp->alloc_xftcolor (xrm->get (".color.main.ds",   "black"  ), 0);
    XftColors [C_TEXT_BG]  = disp->alloc_xftcolor (xrm->get (".color.text.bg",   "white"  ), 0);
    XftColors [C_TEXT_FG]  = disp->alloc_xftcolor (xrm->get (".color.text.fg",   "black"  ), 0);
    XftColors [C_TEXT_HL]  = disp->alloc_xftcolor (xrm->get (".color.text.hl",   "#c0ffa0"), 0);
    XftColors [C_TEXT_CA]  = disp->alloc_xftcolor (xrm->get (".color.text.ca",   "red"    ), 0);
    XftColors [C_TEXT_GR]  = disp->alloc_xftcolor (xrm->get (".color.text.gr",   "gray80" ), 0);
    XftColors [C_TEXT_MO]  = disp->alloc_xftcolor (xrm->get (".color.text.mo",   "#e0e0c0"), 0);
    XftColors [C_TEXT_ER]  = disp->alloc_xftcolor (xrm->get (".color.text.er",   "orange" ), 0);
    XftColors [C_BUTT_BG0] = disp->alloc_xftcolor (xrm->get (".color.butt.bg0",  "#707060"), 0);
    XftColors [C_BUTT_BG1] = disp->alloc_xftcolor (xrm->get (".color.butt.bg1",  "blue"   ), 0);
    XftColors [C_BUTT_BG2] = disp->alloc_xftcolor (xrm->get (".color.butt.bg2",  "red"    ), 0);
    XftColors [C_BUTT_BG3] = disp->alloc_xftcolor (xrm->get (".color.butt.bg3",  "orange" ), 0);
    XftColors [C_BUTT_FG0] = disp->alloc_xftcolor (xrm->get (".color.butt.fg0",  "white"  ), 0);
    XftColors [C_BUTT_FG1] = disp->alloc_xftcolor (xrm->get (".color.butt.fg1",  "white"  ), 0);
    XftColors [C_BUTT_FG2] = disp->alloc_xftcolor (xrm->get (".color.butt.fg2",  "white"  ), 0);
    XftColors [C_BUTT_FG3] = disp->alloc_xftcolor (xrm->get (".color.butt.fg3",  "black"  ), 0);
    XftColors [C_SLID_KN]  = disp->alloc_xftcolor (xrm->get (".color.slid.kn",   "blue"   ), 0);
    XftColors [C_SLID_MK]  = disp->alloc_xftcolor (xrm->get (".color.slid.mk",   "gray90" ), 0);
    XftColors [C_DISP_BG]  = disp->alloc_xftcolor (xrm->get (".color.disp.bg",   "black"  ), 0);
    XftColors [C_DISP_FG1] = disp->alloc_xftcolor (xrm->get (".color.disp.fg1",  "#a0a0ff"), 0);
    XftColors [C_DISP_FG2] = disp->alloc_xftcolor (xrm->get (".color.disp.fg2",  "yellow" ), 0);

    XftFonts [F_MAIN] = disp->alloc_xftfont (xrm->get (".font.main", "luxi:bold:pixelsize=11"));
    XftFonts [F_BUTT] = disp->alloc_xftfont (xrm->get (".font.butt", "luxi:bold:pixelsize=11"));
    XftFonts [F_TEXT] = disp->alloc_xftfont (xrm->get (".font.text", "luxi:pixelsize=11"));
    XftFonts [F_SCAL] = disp->alloc_xftfont (xrm->get (".font.scale","luxi:bold:pixelsize=9"));
    XftFonts [F_FILE] = disp->alloc_xftfont (xrm->get (".font.file", "luxi:pixelsize=12"));
    XftFonts [F_MATR] = disp->alloc_xftfont (xrm->get (".font.matr", "luxi:pixelsize=11"));

    Bst0.font = XftFonts [F_BUTT];
    Bst0.type = X_button_style::RAISED;
    Bst0.color.bg[0] = XftColors [C_BUTT_BG0]->pixel;
    Bst0.color.fg[0] = XftColors [C_BUTT_FG0];
    Bst0.color.bg[1] = XftColors [C_BUTT_BG1]->pixel;
    Bst0.color.fg[1] = XftColors [C_BUTT_FG1];
    Bst0.color.bg[2] = XftColors [C_BUTT_BG2]->pixel;
    Bst0.color.fg[2] = XftColors [C_BUTT_FG2];
    Bst0.color.bg[3] = XftColors [C_BUTT_BG3]->pixel;
    Bst0.color.fg[3] = XftColors [C_BUTT_FG3];
    Bst0.color.shadow.bgnd = XftColors [C_MAIN_BG]->pixel;
    Bst0.color.shadow.lite = XftColors [C_MAIN_LS]->pixel;
    Bst0.color.shadow.dark = XftColors [C_MAIN_DS]->pixel;

    Bst2.font = 0;
    Bst2.type = X_button_style::BORDER;
    Bst2.color.bg [0] = XftColors [C_MAIN_BG]->pixel;
    Bst2.color.fg [0] = XftColors [C_MAIN_FG];
    Bst2.color.bg [1] = XftColors [C_MAIN_BG]->pixel;
    Bst2.color.fg [1] = XftColors [C_MAIN_FG];
    Bst2.color.shadow.bgnd = XftColors [C_MAIN_BG]->pixel;
    Bst2.color.shadow.lite = XftColors [C_MAIN_LS]->pixel;
    Bst2.color.shadow.dark = XftColors [C_MAIN_DS]->pixel;

    Tst0.font = XftFonts [F_TEXT];
    Tst0.color.normal.bgnd = XftColors [C_TEXT_BG]->pixel;
    Tst0.color.normal.text = XftColors [C_TEXT_FG];
    Tst0.color.focus.bgnd  = XftColors [C_TEXT_HL]->pixel;
    Tst0.color.focus.text  = XftColors [C_TEXT_FG];
    Tst0.color.focus.line  = XftColors [C_TEXT_CA]->pixel;
    Tst0.color.shadow.lite = XftColors [C_MAIN_LS]->pixel;
    Tst0.color.shadow.dark = XftColors [C_MAIN_DS]->pixel;
    Tst0.color.shadow.bgnd = XftColors [C_MAIN_BG]->pixel;

    Tst1.font = XftFonts [F_MAIN];
    Tst1.color.normal.bgnd = XftColors [C_MAIN_BG]->pixel;
    Tst1.color.normal.text = XftColors [C_MAIN_FG];
    Tst1.color.shadow.lite = XftColors [C_MAIN_LS]->pixel;
    Tst1.color.shadow.dark = XftColors [C_MAIN_DS]->pixel;
    Tst1.color.shadow.bgnd = XftColors [C_MAIN_BG]->pixel;

    Tst2.font = XftFonts [F_MAIN];
    Tst2.color.normal.bgnd = XftColors [C_DISP_BG]->pixel;
    Tst2.color.normal.text = XftColors [C_DISP_FG1];

    Mst1.font = XftFonts [F_MATR];
    Mst1.color.normal.bg = XftColors [C_TEXT_BG]->pixel;
    Mst1.color.normal.fg = XftColors [C_TEXT_FG];
    Mst1.color.focus.bg  = XftColors [C_TEXT_HL]->pixel;
    Mst1.color.focus.fg  = XftColors [C_TEXT_FG];
    Mst1.color.modif.bg  = XftColors [C_TEXT_MO]->pixel;
    Mst1.color.modif.fg  = XftColors [C_TEXT_FG];
    Mst1.color.error.bg  = XftColors [C_TEXT_ER]->pixel;
    Mst1.color.error.fg  = XftColors [C_TEXT_FG];
    Mst1.color.line = XftColors [C_TEXT_CA]->pixel;
    Mst1.color.grid = XftColors [C_TEXT_GR]->pixel;

    Fst1.bg   = XftColors [C_MAIN_BG]->pixel;
    Fst1.lite = XftColors [C_MAIN_LS]->pixel;
    Fst1.dark = XftColors [C_MAIN_DS]->pixel;
    Fst1.knob = XftColors [C_SLID_KN]->pixel;
    Fst1.mark = XftColors [C_SLID_MK]->pixel;
    Fst1.h = 13;
    Fst1.w = 7;

    Lst0.bg = XftColors [C_TEXT_BG]->pixel;
    Lst0.fg [0] = disp->alloc_xftcolor (xrm->get (".color.flist.fg0", "blue") ,  0);
    Lst0.fg [1] = disp->alloc_xftcolor (xrm->get (".color.flist.fg1", "black"),  0);
    Lst0.fg [2] = disp->alloc_xftcolor (xrm->get (".color.flist.fg2", "red"),    0);
    Lst0.fg [3] = disp->alloc_xftcolor (xrm->get (".color.flist.fg3", "purple"), 0);
    Lst0.font = XftFonts [F_FILE];
    Lst0.dy = Lst0.font->ascent + Lst0.font->descent + 3;

    sca_hpfr.bg = XftColors [C_MAIN_BG]->pixel;
    sca_hpfr.fg = XftColors [C_MAIN_FG];
    sca_hpfr.marg = 0;
    sca_hpfr.font = XftFonts [F_SCAL];
    sca_hpfr.nseg = 8;
    sca_hpfr.set_tick ( 0,  10,   10.0f, "10"  );
    sca_hpfr.set_tick ( 1,  32,   14.1f, 0     );
    sca_hpfr.set_tick ( 2,  54,   20.0f, "20"  );
    sca_hpfr.set_tick ( 3,  76,   28.3f, 0     );
    sca_hpfr.set_tick ( 4,  98,   40.0f, "40"  );
    sca_hpfr.set_tick ( 5, 120,   56.5f, 0     );
    sca_hpfr.set_tick ( 6, 142,   80.0f, "80"  );
    sca_hpfr.set_tick ( 7, 164,  112.0f, 0     );
    sca_hpfr.set_tick ( 8, 186,  160.0f, "160" );

    sca_azim.bg = XftColors [C_MAIN_BG]->pixel;
    sca_azim.fg = XftColors [C_MAIN_FG];
    sca_azim.marg = 0;
    sca_azim.font = XftFonts [F_SCAL];
    sca_azim.nseg = 8;
    sca_azim.set_tick ( 0,  15,  -0.500f, "B"    );
    sca_azim.set_tick ( 1,  33,  -0.375f, 0      );
    sca_azim.set_tick ( 2,  51,  -0.250f, "L"    );
    sca_azim.set_tick ( 3,  69,  -0.125f, 0      );
    sca_azim.set_tick ( 4,  87,   0.000f, "Azim" );
    sca_azim.set_tick ( 5, 105,   0.125f, 0      );
    sca_azim.set_tick ( 6, 123,   0.250f, "R"    );
    sca_azim.set_tick ( 7, 141,   0.375f, 0      );
    sca_azim.set_tick ( 8, 159,   0.500f, "B"    );

    sca_elev.bg = XftColors [C_MAIN_BG]->pixel;
    sca_elev.fg = XftColors [C_MAIN_FG];
    sca_elev.marg = 0;
    sca_elev.font = XftFonts [F_SCAL];
    sca_elev.nseg = 6;
    sca_elev.set_tick ( 0,  15,  -0.250f, "-90"  );
    sca_elev.set_tick ( 1,  39,  -0.167f, 0      );
    sca_elev.set_tick ( 2,  63,  -0.083f, 0      );
    sca_elev.set_tick ( 3,  87,   0,      "Elev" );
    sca_elev.set_tick ( 4, 111,   0.083f,  0     );
    sca_elev.set_tick ( 5, 135,   0.167f, 0      );
    sca_elev.set_tick ( 6, 159,   0.250f, "90"   );

    sca_angle.bg = XftColors [C_MAIN_BG]->pixel;
    sca_angle.fg = XftColors [C_MAIN_FG];
    sca_angle.marg = 0;
    sca_angle.font = XftFonts [F_SCAL];
    sca_angle.nseg = 6;
    sca_angle.set_tick ( 0,  15,  0.000f, "0"     );
    sca_angle.set_tick ( 1,  39,  0.083f, 0       );
    sca_angle.set_tick ( 2,  63,  0.167f, 0       );
    sca_angle.set_tick ( 3,  87,  0.250f, "Angle" );
    sca_angle.set_tick ( 4, 111,  0.333f, 0       );
    sca_angle.set_tick ( 5, 135,  0.417f, 0       );
    sca_angle.set_tick ( 6, 159,  0.500f, "180"   );

    sca_direc.bg = XftColors [C_MAIN_BG]->pixel;
    sca_direc.fg = XftColors [C_MAIN_FG];
    sca_direc.marg = 0;
    sca_direc.font = XftFonts [F_SCAL];
    sca_direc.nseg = 6;
    sca_direc.set_tick ( 0,  15,  0.000f, "0mni"  );
    sca_direc.set_tick ( 1,  39,  0.167f, 0       );
    sca_direc.set_tick ( 2,  63,  0.333f, 0       );
    sca_direc.set_tick ( 3,  87,  0.500f, "Card"  );
    sca_direc.set_tick ( 4, 111,  0.667f, 0       );
    sca_direc.set_tick ( 5, 135,  0.833f, 0       );
    sca_direc.set_tick ( 6, 159,  1.000f, "Fig-8" );

    sca_volume.bg = XftColors [C_MAIN_BG]->pixel;
    sca_volume.fg = XftColors [C_MAIN_FG];
    sca_volume.marg = 2;
    sca_volume.font = XftFonts [F_SCAL];
    sca_volume.nseg = 7;
    sca_volume.set_tick ( 0,  15, -100.0f, 0     );
    sca_volume.set_tick ( 1,  25,  -50.0f, "-50" );
    sca_volume.set_tick ( 2,  55,  -40.0f, "-40" );
    sca_volume.set_tick ( 3,  95,  -30.0f, "-30" );
    sca_volume.set_tick ( 4, 135,  -20.0f, "-20" );
    sca_volume.set_tick ( 5, 175,  -10.0f, "-10" );
    sca_volume.set_tick ( 6, 215,    0.0f, "0"   );
    sca_volume.set_tick ( 7, 255,   10.0f, "10"  );

}


void styles_fini (X_display *disp)
{
}
