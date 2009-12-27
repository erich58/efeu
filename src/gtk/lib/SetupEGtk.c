/*
:*:	seting up esh-interface to GTK+
:de:	Esh-Interface zu GTK+ initialisieren

$Copyright (C) 2001 Erich Frühstück
This file is part of EFEU.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; see the file COPYING.Library.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <GUI/EGtkWidget.h>
#include <EFEU/CmdPar.h>
#include <EFEU/parsedef.h>

static void f_init (EfiFunc *func, void *rval, void **arg)
{
	EGtkInit();
}

static void f_gtkres (EfiFunc *func, void *rval, void **arg)
{
	EGtkInit();
	gtk_rc_parse_string(Val_str(arg[0]));
}

static EfiFuncDef fdef[] = {
	{ 0, &Type_void, "gtkinit ()", f_init },
	{ 0, &Type_void, "gtkres (str def)", f_gtkres },
};

/*
:de:
Die Funktion |$1| erweitert den esh-Interpreter zur Nutzung
des Gtk+ Interfaces. Diese Funktion wird implizit beim
Laden der gemeinsam genutzen Programmbibliothek aufgerufen.
:*:
The function |$1| expands the esh-Interpreter with the Gtk+ interface.
This function is implicitly called on loading the shared library.
*/

void SetupEGtk (void)
{
	static int setup_done = 0;

	if	(setup_done)	return;

	setup_done = 1;
	SetupStd();
	EGtkRes(ProgName);
	AddFuncDef(fdef, tabsize(fdef));
}

void _init (void);

void _init (void)
{
	SetupEGtk();
}

/*
$SeeAlso
\mref{GTK(7)}.
*/
