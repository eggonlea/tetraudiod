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
#include <clthreads.h>
#include <sys/mman.h>
#include <signal.h>
#include "styles.h"
#include "jclient.h"
#include "mainwin.h"


#define CP (char *)
#define NOPTS 4


XrmOptionDescRec options [NOPTS] =
{
    {CP"-h", CP".help",    XrmoptionNoArg,  CP"true" },
    {CP"-c", CP".config",  XrmoptionSepArg,  0       },
    {CP"-p", CP".presets", XrmoptionSepArg, CP"."    },
    {CP"-B", CP".bformat", XrmoptionSepArg, CP"fuma" }
};


static Jclient  *jclient = 0;
static Mainwin  *mainwin = 0;



static void help (void)
{
    fprintf (stderr, "\nTetraProcc-%s\n\n", VERSION);
    fprintf (stderr, "  (C) 2006-2014 Fons Adriaensen  <fons@linuxaudio.org>\n\n");
    fprintf (stderr, "Options:\n");
    fprintf (stderr, "  -h         Display this text\n");
    fprintf (stderr, "  -c <file>  Configuration file \n");
    fprintf (stderr, "  -p <path>  Preset directory\n");
    fprintf (stderr, "  -B fuma|snd3d|n3d  B-format weights\n");
    exit (1);
}


static void sigint_handler (int)
{
    signal (SIGINT, SIG_IGN);
    mainwin->stop ();
}


int main (int ac, char *av [])
{
    X_resman       xresman;
    X_display     *display;
    X_handler     *handler;
    X_rootwin     *rootwin;
    int           e;

    xresman.init (&ac, av, CP"tetraproc", options, NOPTS);
    if (xresman.getb (".help", 0)) help ();
            
    display = new X_display (xresman.get (".display", 0));
    if (display->dpy () == 0)
    {
	fprintf (stderr, "Can't open display.\n");
        delete display;
	exit (1);
    }

    styles_init (display, &xresman);
    if (Kmeter::load_images (display, SHARED)) exit (1);

    rootwin = new X_rootwin (display);
    jclient = new Jclient (xresman.rname ());
    mainwin = new Mainwin (rootwin, &xresman, 100, 100, jclient);
    rootwin->handle_event ();
    handler = new X_handler (display, mainwin, EV_X11);
    handler->next_event ();

    ITC_ctrl::connect (jclient, EV_EXIT,    mainwin, EV_EXIT);
    ITC_ctrl::connect (jclient, EV_GO_IDLE, mainwin, EV_GO_IDLE);
    ITC_ctrl::connect (jclient, EV_GO_PROC, mainwin, EV_GO_PROC);
    ITC_ctrl::connect (mainwin, EV_GO_IDLE, jclient, EV_GO_IDLE);
    ITC_ctrl::connect (mainwin, EV_GO_PROC, jclient, EV_GO_PROC);

    if (mlockall (MCL_CURRENT | MCL_FUTURE)) fprintf (stderr, "Warning: memory lock failed.\n");
    signal (SIGINT, sigint_handler); 

    mainwin->loadconf ();
    do
    {
	e = mainwin->process ();
	if (e == EV_X11)
	{
	    rootwin->handle_event ();
	    handler->next_event ();
	}
    }
    while (e != EV_EXIT);	

    delete jclient;
    delete handler;
    delete rootwin;
    delete display;
   
    return 0;
}



