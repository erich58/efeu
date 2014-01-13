/*
:*:	setting up GtkMisc widget
:de:	GtkMisc Widgets initialisieren

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

#define	GTK_1_2	0

#define	STR(arg)	(Val_ptr(arg) ? Val_ptr(arg) : "")


/*	Widget - Variablen
*/

static EGtkArgDef arg_misc[] = {
	{ "double", "GtkMisc::xalign" },
	{ "double", "GtkMisc::yalign" },
	{ "int", "GtkMisc::xpad" },
	{ "int", "GtkMisc::ypad" },
};

static EGtkArgDef arg_label[] = {
	{ "str", "GtkLabel::label" },
	{ "str", "GtkLabel::pattern" },
	{ "bool", "GtkLabel::wrap" },
	{ "GtkJustification", "GtkLabel::justify" },
};

static EGtkArgDef arg_arrow[] = {
	{ "GtkArrowType", "GtkArrow::arrow_type" },
	{ "GtkShadowType", "GtkArrow::shadow_type" },
};

/*	Widget Funktionen
*/

static void f_label (EfiFunc *func, void *rval, void **arg)
{
	Val_ptr(rval) = gtk_label_new(STR(arg[0]));
}

#if	GTK_1_2
static void f_label_uline (EfiFunc *func, void *rval, void **arg)
{
	gtk_label_parse_uline(Val_ptr(arg[0]), Val_str(arg[1]));
}
#endif

static void f_arrow (EfiFunc *func, void *rval, void **arg)
{
	Val_ptr(rval) = gtk_arrow_new(Val_int(arg[0]), Val_int(arg[1]));
}

static EfiFuncDef fdef[] = {
	{ 0, NULL, "GtkLabel GtkLabel (str title)", f_label },
#if	GTK_1_2
	{ 0, &Type_void, "GtkLabel::parse_uline (str label)", f_label_uline },
#endif
	{ 0, NULL, "GtkWidget str ()", f_label },
	{ 0, NULL, "GtkArrow GtkArrow (GtkArrowType, GtkShadowType = \"out\"",
		f_arrow },
};

/*
:de:
Die Funktion |$1| erweitert den esh-Interpreter zur Nutzung
der GtkMisc Widgets.
:*:
The function |$1| expands the esh-Interpreter with the GtkMisc widgets.
*/

void EGtkMisc_setup (void)
{
	EGtkWidgetClass(gtk_misc_get_type(),
		arg_misc, tabsize(arg_misc), NULL, 0);
	EGtkWidgetClass(gtk_label_get_type(),
		arg_label, tabsize(arg_label), NULL, 0);
	EGtkWidgetClass(gtk_arrow_get_type(),
		arg_arrow, tabsize(arg_arrow), NULL, 0);
	EGtkWidgetClass(gtk_image_get_type(), NULL, 0, NULL, 0);
	EGtkWidgetClass(gtk_pixmap_get_type(), NULL, 0, NULL, 0);
	AddFuncDef(fdef, tabsize(fdef));
}

#endif

/*
$SeeAlso
\mref{EGtk(7)}.
*/
