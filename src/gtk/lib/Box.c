/*
:*:	setting up GtkBox widgets
:de:	GtkBox Widgets initialisieren

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

GtkWidget *Obj2GtkWidget (EfiObj *obj)
{
	obj = EvalObj(obj, NULL);

	if	(obj == NULL)
		return NULL;

	if	(IsTypeClass(obj->type, &EGtkWidgetType))
		return Obj2Ptr(obj, obj->type);

	return Obj2Ptr(obj, &EGtkWidgetType);
}

static GtkWidget *make_box (GtkWidget *box, EfiObjList *list)
{
	GtkWidget *sub;

	while (list != NULL)
	{
		if	((sub = Obj2GtkWidget(RefObj(list->obj))))
			gtk_box_pack_start(GTK_BOX(box), sub, TRUE, TRUE, 0);

		list = list->next;
	}

	gtk_widget_show_all(box);
	return box;
}

static void f_vbox (EfiFunc *func, void *rval, void **arg)
{
	Val_ptr(rval) = make_box(gtk_vbox_new(TRUE, 0), Val_list(arg[0]));
}

static void f_hbox (EfiFunc *func, void *rval, void **arg)
{
	Val_ptr(rval) = make_box(gtk_hbox_new(TRUE, 0), Val_list(arg[0]));
}

static void f_vbutton_box (EfiFunc *func, void *rval, void **arg)
{
	Val_ptr(rval) = make_box(gtk_vbutton_box_new(), Val_list(arg[0]));
}

static void f_hbutton_box (EfiFunc *func, void *rval, void **arg)
{
	Val_ptr(rval) = make_box(gtk_hbutton_box_new(), Val_list(arg[0]));
}

static void f_pack_start (EfiFunc *func, void *rval, void **arg)
{
	gtk_box_pack_start(Val_ptr(arg[0]), Val_ptr(arg[1]),
		Val_bool(arg[2]), Val_bool(arg[3]), Val_uint(arg[4]));
}

static void f_pack_end (EfiFunc *func, void *rval, void **arg)
{
	gtk_box_pack_end(Val_ptr(arg[0]), Val_ptr(arg[1]),
		Val_bool(arg[2]), Val_bool(arg[3]), Val_uint(arg[4]));
}

static void f_reorder (EfiFunc *func, void *rval, void **arg)
{
	gtk_box_reorder_child(Val_ptr(arg[0]), Val_ptr(arg[1]),
		Val_int(arg[2]));
}

static void f_layout (EfiFunc *func, void *rval, void **arg)
{
	gtk_button_box_set_layout(Val_ptr(arg[0]), Val_int(arg[1]));
}

static void f_size (EfiFunc *func, void *rval, void **arg)
{
	gtk_button_box_set_child_size(Val_ptr(arg[0]),
		Val_int(arg[1]), Val_int(arg[2]));
}

static void f_padding (EfiFunc *func, void *rval, void **arg)
{
	gtk_button_box_set_child_ipadding(Val_ptr(arg[0]),
		Val_int(arg[1]), Val_int(arg[2]));
}

#define	PACKARG	"(GtkWidget," "bool expand = true," \
	"bool fill = true," "unsigned padding = 0)"

static EfiFuncDef fdef[] = {
	{ 0, NULL, "GtkVBox GtkVBox (...)", f_vbox },
	{ 0, NULL, "GtkHBox GtkHBox (...)", f_hbox },
	{ 0, NULL, "GtkVButtonBox GtkVButtonBox (...)", f_vbutton_box },
	{ 0, NULL, "GtkHButtonBox GtkHButtonBox (...)", f_hbutton_box },
	{ 0, NULL, "GtkBox GtkBox::start" PACKARG, f_pack_start },
	{ 0, NULL, "GtkBox GtkBox::end" PACKARG, f_pack_end },
	{ 0, &Type_void, "GtkBox::reorder (GtkWidget child, int pos)",
		f_reorder },
	{ 0, &Type_void, "GtkButtonBox::layout (GtkButtonBoxStyle style)",
		f_layout },
	{ 0, &Type_void, "GtkButtonBox::size (int min_width, int min_height)",
		f_size },
	{ 0, &Type_void, "GtkButtonBox::padding (int ipad_x, int ipad_y)",
		f_padding },
};

/*	Argumente und Signale
*/

static EGtkArgDef arg_box[] = {
	{ "int", "GtkBox::spacing" },
	{ "bool", "GtkBox::homogeneous" },
};

/*
:de:
Die Funktion |$1| erweitert den esh-Interpreter zur Nutzung
der GtkBox Widgets.
:*:
The function |$1| expands the esh-Interpreter with the GtkBox widgets.
*/

void EGtkBox_setup (void)
{
	EGtkWidgetClass(gtk_box_get_type(),
		arg_box, tabsize(arg_box), NULL, 0);
	EGtkType(gtk_hbox_get_type());
	EGtkType(gtk_vbox_get_type());
	EGtkType(gtk_hbutton_box_get_type());
	EGtkType(gtk_vbutton_box_get_type());
	AddFuncDef(fdef, tabsize(fdef));
}

/*
$SeeAlso
\mref{EGtk(7)}.
*/
