/*
:*:	setting up GtkWindow widget
:de:	GtkWindow Widgets initialisieren

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
#include <EFEU/stdtype.h>
#include <EFEU/CmdPar.h>


/*	Toplevel - Widget
*/

static int toplevel_count = 0;

static void sig_bye (GtkWidget *widget, gpointer data)
{
	if	(--toplevel_count <= 0)
		gtk_exit(0);
}

static void f_mainloop (void)
{
	gtk_main();
}

void EGtkMain (void)
{
	static int need_init = 1;

	if	(need_init)
	{
		atexit(f_mainloop);
		need_init = 0;
	}
}


static void f_toplevel (EfiFunc *func, void *rval, void **arg)
{
	char *title;
	GtkWidget *TopLevel;
	GtkWidget *sub;
	
	EGtkInit();
	TopLevel = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_signal_connect (GTK_OBJECT(TopLevel), "destroy",
		GTK_SIGNAL_FUNC(sig_bye), NULL);
	EGtkMain();
	toplevel_count++;

	if	((title = Val_str(arg[0])))
		gtk_window_set_title(GTK_WINDOW(TopLevel), title);

	if	((sub = Val_ptr(arg[1])))
		gtk_container_add(GTK_CONTAINER(TopLevel), sub);

	gtk_widget_show_all(TopLevel);
	Val_ptr(rval) = TopLevel;
}

static EfiFuncDef fdef[] = {
	{ 0, NULL, "GtkWindow GtkTopLevel"
		"(str title = NULL, GtkWidget = NULL)", f_toplevel },
};

/*
:de:
Die Funktion |$1| erweitert den esh-Interpreter zur Nutzung
der GtkWindow Widgets.
:*:
The function |$1| expands the esh-Interpreter with the GtkWindow widgets.
*/

void EGtkWindow_setup (void)
{
	EGtkWidgetClass(gtk_window_get_type(),
		NULL, 0, NULL, 0);
	AddFuncDef(fdef, tabsize(fdef));
}

/*
$SeeAlso
\mref{EGtk(7)}.
*/
