/*
:*:	seting up GtkObject class
:de:	GtkObject Klasse initialisieren

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
#include <EFEU/object.h>

#if	HAS_GTK

EfiType EGtkObjectType = STD_TYPE("GtkObject", GtkObject *,
	&Type_ptr, NULL, NULL);

EfiType EGtkDataType = STD_TYPE("GtkData", GtkData *,
	&EGtkObjectType, NULL, NULL);

#define	TYPE_NAME(ptr) gtk_type_name(GTK_OBJECT_TYPE(ptr))

static void f_arglist (EfiFunc *func, void *rval, void **arg)
{
	EfiObjList *list;
	GtkObject *obj;
	GtkType type;

	list = NULL;
	obj = Val_ptr(arg[0]);
	type = obj ? GTK_OBJECT_TYPE(obj) : 0;

	while (type)
	{
		list = EGtkArgList(list, type);
		type = gtk_type_parent(type);
	}

	Val_list(rval) = list;
}

static void f_arginfo (EfiFunc *func, void *rval, void **arg)
{
	IO *io;
	GtkObject *obj;
	GtkType type;

	io = Val_ptr(arg[1]);
	obj = Val_ptr(arg[0]);
	type = obj ? GTK_OBJECT_TYPE(obj) : 0;

	while (type)
	{
		EGtkArgInfo(io, type);
		type = gtk_type_parent(type);
	}
}

static void f_fprint (EfiFunc *func, void *rval, void **arg)
{
	GtkObject *ptr = Val_ptr(arg[1]);
	IO *io = Val_io(arg[0]);

	Val_int(rval) = ptr ?
		io_printf(io, "(%s) %p", TYPE_NAME(ptr), ptr) :
		io_printf(io, "(%s) NULL", func->arg[1].type->name);
}

static void f_gtkobj (EfiFunc *func, void *rval, void **arg)
{
	GtkType type = gtk_type_from_name(Val_str(arg[0]));
	Val_ptr(rval) = gtk_object_new(type, NULL);
}

static void f_objarg (EfiFunc *func, void *rval, void **arg)
{
	Val_obj(rval) = EGtkArg2Lval(Val_ptr(arg[0]), Val_str(arg[1]));
}

static void f_set (EfiFunc *func, void *rval, void **arg)
{
	gtk_object_set_data_full(Val_ptr(arg[0]), Val_str(arg[1]),
		(gpointer) RefObj(arg[2]), (GtkDestroyNotify) rd_deref);
}

static void f_get (EfiFunc *func, void *rval, void **arg)
{
	Val_obj(rval) = RefObj(gtk_object_get_data(Val_ptr(arg[0]),
		Val_str(arg[1])));
}

static void f_signal (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = gtk_signal_connect(Val_ptr(arg[0]),
		Val_str(arg[1]), GTK_SIGNAL_FUNC(EGtkSigFunc_simple),
		RefObj(Val_obj(arg[2])));
}

static void f_disconnect (EfiFunc *func, void *rval, void **arg)
{
	gtk_signal_disconnect(Val_ptr(arg[0]), Val_int(arg[1]));
}

static void f_disconnect_data (EfiFunc *func, void *rval, void **arg)
{
	gtk_signal_disconnect_by_data(Val_ptr(arg[0]), Val_obj(arg[1]));
}

static EfiFuncDef fdef[] = {
	{ 0, &Type_list, "arglist (GtkObject)", f_arglist },
	{ 0, &Type_void, "arginfo (GtkObject, IO = iostd)", f_arginfo },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, GtkObject)", f_fprint },
	{ 0, &EGtkObjectType, "GtkObject (str type)", f_gtkobj },
	{ FUNC_VIRTUAL, &Type_obj, "operator[] (GtkObject, str)", f_objarg },
	{ 0, &Type_void, "GtkObject::set (str name, .)", f_set },
	{ 0, &Type_obj, "GtkObject::get (str name)", f_get },
	{ 0, &Type_int, "GtkObject::signal (str name, Expr_t expr)",
		f_signal },
	{ FUNC_VIRTUAL, &Type_void, "GtkObject::disconnect (int id)",
		f_disconnect },
	{ FUNC_VIRTUAL, &Type_void, "GtkObject::disconnect (Expr_t expr)",
		f_disconnect_data },
};

/*
:de:
Die Funktion |$1| erweitert den esh-Interpreter zur Nutzung
der GtkObject Klasse.
:*:
The function |$1| expands the esh-Interpreter with the GtkObject class.
*/

void EGtkObject_setup (void)
{
	AddType(&EGtkObjectType);
	AddType(&EGtkDataType);
	AddFuncDef(fdef, tabsize(fdef));
	EGtkWidget_setup();
}

#endif

/*
$SeeAlso
\mref{EGtk(7)}.
*/
