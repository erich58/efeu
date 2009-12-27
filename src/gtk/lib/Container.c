/*
:*:	setting up GtkContainer widget
:de:	GtkContainer Widgets initialisieren

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


static void f_add (EfiFunc *func, void *rval, void **arg)
{
	gtk_container_add(Val_ptr(arg[0]), Val_ptr(arg[1]));
	gtk_widget_show_all(Val_ptr(arg[0]));
}

static void f_remove (EfiFunc *func, void *rval, void **arg)
{
	gtk_container_remove(Val_ptr(arg[0]), Val_ptr(arg[1]));
}

static void f_children (EfiFunc *func, void *rval, void **arg)
{
	GList *glist;
	EfiObjList **ptr;
	
	glist = gtk_container_children(Val_ptr(arg[0]));
	ptr = rval;
	*ptr = NULL;

	while (glist != NULL)
	{
		*ptr = NewObjList(EGtkObject(glist->data));
		ptr = &(*ptr)->next;
		glist = glist->next;
	}
}

static void f_focus (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = gtk_container_focus(Val_ptr(arg[0]), Val_int(arg[1]));
}

static void f_put (EfiFunc *func, void *rval, void **arg)
{
	gtk_fixed_put(Val_ptr(arg[0]), Val_ptr(arg[1]),
		(gint16) Val_int(arg[2]), (gint16) Val_int(arg[3]));
}

static void f_move (EfiFunc *func, void *rval, void **arg)
{
	gtk_fixed_move(Val_ptr(arg[0]), Val_ptr(arg[1]),
		(gint16) Val_int(arg[2]), (gint16) Val_int(arg[3]));
}

static EfiFuncDef fdef[] = {
	{ 0, &Type_void, "GtkContainer::add (GtkWidget)", f_add },
	{ 0, &Type_void, "GtkContainer::operator+= (GtkWidget)", f_add },
	{ 0, &Type_void, "GtkContainer::remove (GtkWidget)", f_remove },
	{ 0, &Type_void, "GtkContainer::operator-= (GtkWidget)", f_remove },
	{ 0, &Type_int, "GtkContainer::focus (GtkDirectionType)", f_focus },
	{ 0, &Type_list, "GtkContainer::children ()", f_children },
	{ 0, &Type_void, "GtkFixed::put (GtkWidget, int x, int y)", f_put },
	{ 0, &Type_void, "GtkFixed::move (GtkWidget, int x, int y)", f_move },
};

/*	Argumente und Signale
*/

static EGtkArgDef arg_container[] = {
	{ "size_t", "GtkContainer::border_width" },
	{ "GtkResizeMode", "GtkContainer::resize_mode" },
};

static EGtkSigDef sig_container[] = {
	{ "add", EGtkSigFunc_child },
	{ "remove", EGtkSigFunc_child },
	{ "check_resize", EGtkSigFunc_simple },
	/*
	{ "focus", EGtkSigFunc_focus },
	*/
	{ "set_focus_child", EGtkSigFunc_child },
};

/*
:de:
Die Funktion |$1| erweitert den esh-Interpreter zur Nutzung
der GtkContainer Widgets.
:*:
The function |$1| expands the esh-Interpreter with the GtkContainer widgets.
*/

void EGtkContainer_setup (void)
{
	EGtkWidgetClass(gtk_container_get_type(),
		arg_container, tabsize(arg_container),
		sig_container, tabsize(sig_container));
	EGtkType(gtk_fixed_get_type());
	EGtkBox_setup();
	EGtkButton_setup();
	EGtkList_setup();
	EGtkWindow_setup();
	AddFuncDef(fdef, tabsize(fdef));
}

/*
$SeeAlso
\mref{EGtk(7)}.
*/
