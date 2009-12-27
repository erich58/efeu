/*
:*:	setting up GtkEditable widget
:de:	GtkEditable Widgets initialisieren

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

#if	HAS_GTK

#define	STR(arg)	(Val_ptr(arg) ? Val_ptr(arg) : "")

static void get_text (EfiObj *obj)
{
	char *p = gtk_entry_get_text(Val_ptr(obj + 1));
	Obj2Data(str2Obj(mstrcpy(p)), obj->type, obj->data);
}

static void set_text (EfiObj *obj)
{
	char *p = Val_str(obj->data);
	gtk_entry_set_text(GTK_ENTRY(Val_ptr(obj + 1)), p ? p : "");
}

static EGTK_LVAL(lval_text, get_text, set_text);


static void f_entry (EfiFunc *func, void *rval, void **arg)
{
	GtkWidget *entry;

	entry = gtk_entry_new ();
	gtk_entry_set_text (GTK_ENTRY(entry), STR(arg[0]));
	gtk_editable_select_region (GTK_EDITABLE(entry), 0, -1);
	Val_ptr(rval) = entry;
}

static EfiObj *text_var (const EfiObj *obj, void *data)
{
	return LvalObj(&lval_text, &Type_str, Val_ptr(obj->data), NULL);
}

static EfiMember entry_member[] = {
	{ "text", &Type_str, text_var, NULL, "GtkEntry::text" },
};

static EfiFuncDef fdef[] = {
	{ 0, NULL, "GtkEntry GtkEntry (str text = NULL)", f_entry },
};

/*
:de:
Die Funktion |$1| erweitert den esh-Interpreter zur Nutzung
der GtkEditable Widgets.
:*:
The function |$1| expands the esh-Interpreter with the GtkEditable widgets.
*/

void EGtkEditable_setup (void)
{
	EGtkWidgetClass(gtk_editable_get_type(), NULL, 0, NULL, 0);
	EGtkWidgetClass(gtk_entry_get_type(), NULL, 0, NULL, 0);
	AddEfiMember(GetType("GtkEntry")->vtab,
		entry_member, tabsize(entry_member));
	AddFuncDef(fdef, tabsize(fdef));
}

#endif

/*
$SeeAlso
\mref{EGtk(7)}.
*/
