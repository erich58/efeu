/*
:*:	setting up GtkList widgets
:de:	GtkList Widgets initialisieren

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

#define	STR(arg)	(Val_ptr(arg) ? Val_ptr(arg) : "")


static void f_mode (EfiFunc *func, void *rval, void **arg)
{
	gtk_list_set_selection_mode(Val_ptr(arg[0]), Val_int(arg[1]));
}

/*	Listeneinträge
*/

static void f_item (EfiFunc *func, void *rval, void **arg)
{
	Val_ptr(rval) = gtk_list_item_new_with_label(STR(arg[0]));
}

static void f_witem (EfiFunc *func, void *rval, void **arg)
{
	Val_ptr(rval) = gtk_list_item_new();
	gtk_container_add(Val_ptr(rval), Val_ptr(arg[0]));
}

static GList *item_list (EfiObjList *olist)
{
	GList *ilist;
	EfiType *type;
	void *p;
	
	type = EGtkType(gtk_list_item_get_type());

	for (ilist = NULL; olist != NULL; olist = olist->next)
		if ((p = Obj2Ptr(RefObj(olist->obj), type)))
			ilist = g_list_append(ilist, p);

	return ilist;
}

static void f_append (EfiFunc *func, void *rval, void **arg)
{
	gtk_list_append_items(Val_ptr(arg[0]), item_list(Val_list(arg[1])));
}

static void f_index (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = gtk_list_child_position(Val_ptr(arg[0]),
		Val_ptr(arg[1]));
}

static EfiFuncDef fdef[] = {
	{ 0, NULL, "GtkListItem GtkListItem (str label)", f_item },
	{ 0, NULL, "GtkListItem GtkListItem (GtkWidget widget)", f_witem },
	{ 0, NULL, "GtkList GtkList::mode (GtkSelectionMode mode)", f_mode },
	{ 0, NULL, "GtkList GtkList::append (...)", f_append },
	{ 0, NULL, "int GtkList::index (GtkListItem child)", f_index },
};

/*	Signalhändling
*/

static EGtkSigDef sig_list[] = {
	{ "selection_changed", EGtkSigFunc_simple },
	{ "select_child", EGtkSigFunc_child },
	{ "unselect_child", EGtkSigFunc_child },
};

/*
:de:
Die Funktion |$1| erweitert den esh-Interpreter zur Nutzung
von Listen Widgets.
:*:
The function |$1| expands the esh-Interpreter with list widgets.
*/

void EGtkList_setup (void)
{
	EGtkWidgetClass(gtk_list_get_type(), NULL, 0,
		sig_list, tabsize(sig_list));
	EGtkWidgetClass(gtk_list_item_get_type(), NULL, 0, NULL, 0);
	/*
	EGtkWidgetClass(gtk_clist_get_type(),
		NULL, 0, NULL, 0);
	*/
	AddFuncDef(fdef, tabsize(fdef));
}

/*
$SeeAlso
\mref{EGtk(7)}.
*/
