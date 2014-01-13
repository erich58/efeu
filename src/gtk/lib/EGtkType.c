/*
:*:	get/create EFEU type from Gtk+ type
:de:	EFEU Datentype aus Gtk+ Datentype ableiten

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

#if	HAS_GTK

void EGtkTypeCast (EfiFunc *func, void *rval, void **arg)
{
	Val_ptr(rval) = Val_ptr(arg[0]);
}

static void create_object (EfiFunc *func, void *rval, void **arg)
{
	GtkType type = gtk_type_from_name(func->type->name);
	Val_ptr(rval) = gtk_object_new(type, NULL);
}

EfiType *EGtkType (GtkType gtktype)
{
	EfiType *type;
	char *p, *name;

	if	(!gtk_type_is_a(gtktype, GTK_TYPE_OBJECT))
		return NULL;

	name = gtk_type_name(gtktype);
	type = GetType(name);

	if	(type)	return type;

	type = NewType(name);
	type->size = sizeof(GtkWidget *);
	gtktype = gtk_type_parent(gtktype);

	if	(GTK_FUNDAMENTAL_TYPE(gtktype) == GTK_TYPE_OBJECT)
		type->base = EGtkType(gtktype);

	AddType(type);
	p = msprintf("%s (void)", type->name);
	SetFunc(FUNC_VIRTUAL, type, p, create_object);
	memfree(p);
	p = msprintf("%s ()", type->name);
	SetFunc(FUNC_RESTRICTED, &EGtkObjectType, p, EGtkTypeCast);

	if	(gtk_type_is_a(gtktype, GTK_TYPE_WIDGET))
		SetFunc(FUNC_RESTRICTED, &EGtkWidgetType, p, EGtkTypeCast);

	memfree(p);
	return type;
}

#endif
