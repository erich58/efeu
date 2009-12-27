/*
:*:	signal haendler
:de:	Signalhändling

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

static char connect_pfx[] = "connect";

void EGtkSig_var (const char *name, Type_t *type, void *data)
{
	VarDef_t vdef;
	vdef.name = (char *) name;
	vdef.type = type;
	vdef.data = data;
	vdef.desc = NULL;
	AddVarDef(NULL, &vdef, 1);
}

void EGtkSig_start (GtkObject **ptr)
{
	PushVarTab(NULL, NULL);
	EGtkSig_var("this", EGtkType(GTK_OBJECT_TYPE(*ptr)), ptr);
}

void EGtkSig_end (Obj_t *expr)
{
	UnrefEval(RefObj(expr));
	PopVarTab();
}

void EGtkSigFunc_simple (GtkObject *obj, Obj_t *expr)
{
	EGtkSig_start(&obj);
	EGtkSig_end(expr);
}

void EGtkSigFunc_child (GtkObject *obj, GtkObject *child, Obj_t *expr)
{
	EGtkSig_start(&obj);
	EGtkSig_var("child", EGtkType(GTK_OBJECT_TYPE(child)), &child);
	EGtkSig_end(expr);
}

static void Func_connect (Func_t *func, void *rval, void **arg)
{
	Val_int(rval) = gtk_signal_connect(Val_ptr(arg[0]),
		func->name + sizeof(connect_pfx), GTK_SIGNAL_FUNC(func->par),
		RefObj(Val_obj(arg[1])));
}

void AddEGtkSig (Type_t *type, EGtkSigDef *def, size_t dim)
{
	char *p;
	Func_t *func;

	for (; dim-- > 0; def++)
	{
		p = msprintf("%s::%s_%s (Expr_t expr)", type->name,
			connect_pfx, def->name);
		func = Prototype(p, &Type_int, NULL, 0);
		memfree(p);

		if	(!func)	continue;

		func->eval = Func_connect;
		func->par = def->func;
		AddFunc(func);
	}
}
