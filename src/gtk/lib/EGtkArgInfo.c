/*
:*:	get GtkObject argument information
:de:	GtkObjekt Argumentinformationen abfragen

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

#if	HAS_GTK

EfiObjList *EGtkArgList (EfiObjList *base, GtkType type)
{
	EfiObjList *list, **ptr;
	GtkArg *args;
	guint i, nargs;
	
	list = NULL;
	ptr = &list;
	args = gtk_object_query_args(type, NULL, &nargs);

	for (i = 0; i < nargs; i++)
	{
		*ptr = NewObjList(str2Obj(mstrcpy(args[i].name)));
		ptr = &(*ptr)->next;
	}
	
	if	(args)
		g_free(args);

	*ptr = base;
	return list;
}


void EGtkArgInfo (IO *io, GtkType type)
{
	GtkArg *args;
	guint i, nargs;
	
	args = gtk_object_query_args(type, NULL, &nargs);

	for (i = 0; i < nargs; i++)
	{
		io_printf(io, "%s ", gtk_type_name(args[i].type));
		io_printf(io, "%s\n", args[i].name);
	}
	
	if	(args)
		g_free(args);
}

#endif
