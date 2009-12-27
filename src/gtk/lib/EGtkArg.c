/*
:*:	get/set GtkWidget argument
:de:	GtkWidget Argumente abfragen/setzen

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

/*	L-Wert Zugriff auf Widget Argumente
*/

#define	LVAL_SIZE(type) (sizeof(EGtkArg) + type->size)

Obj_t *EGtkLval_alloc (Type_t *type, va_list list)
{
	EGtkArg *obj = (EGtkArg *) Obj_alloc(LVAL_SIZE(type));
	obj->obj = va_arg(list, GtkObject *);
	obj->arg.name = mstrcpy(va_arg(list, char *));
	obj->data = (void *) (obj + 1);
	return (Obj_t *) obj;
}

void EGtkLval_free (Obj_t *ptr)
{
	EGtkArg *obj = (EGtkArg *) ptr;
	CleanData(obj->type, obj->data);
	memfree(obj->arg.name);
	Obj_free(ptr, LVAL_SIZE(obj->type));
}

char *EGtkLval_ident (Obj_t *obj)
{
	EGtkArg *arg = (EGtkArg *) obj;
	return arg->arg.name ? mstrcpy(arg->arg.name) : NULL;
}

Obj_t *EGtkObject (GtkObject *obj)
{
	return NewPtrObj(GetType(gtk_type_name(GTK_OBJECT_TYPE(obj))), obj);
}

Obj_t *EGtkArg2Obj (GtkArg *arg)
{
	GtkType gtype;
	Type_t *otype;
	char *s;
	
	if	(!arg)	return NULL;

	gtype = GTK_FUNDAMENTAL_TYPE(arg->type);

	switch (gtype)
	{
	case GTK_TYPE_INT:
		return int2Obj(GTK_VALUE_INT(*arg));
	case GTK_TYPE_LONG:
		return long2Obj(GTK_VALUE_LONG(*arg));
	case GTK_TYPE_ULONG:
		return size2Obj(GTK_VALUE_ULONG(*arg));
	case GTK_TYPE_BOOL:
		return bool2Obj((GTK_VALUE_BOOL(*arg) == TRUE));
	case GTK_TYPE_FLOAT:
		return double2Obj(GTK_VALUE_FLOAT(*arg));
	case GTK_TYPE_DOUBLE:
		return double2Obj(GTK_VALUE_DOUBLE(*arg));
	case GTK_TYPE_STRING:
		s = mstrcpy(GTK_VALUE_STRING(*arg));
		g_free(GTK_VALUE_STRING(*arg));
		return str2Obj(s);
	case GTK_TYPE_INVALID:
	case GTK_TYPE_NONE:
		return NULL;
	default:
		break;
	}

	otype = GetType(gtk_type_name(arg->type));

	if	(IsTypeClass(otype, &Type_enum))
	{
		Buf_int = GTK_VALUE_ENUM(*arg);
		return NewObj(otype, &Buf_int);
	}

	if	(IsTypeClass(otype, &EGtkObjectType))
	{
		return NewPtrObj(otype, GTK_VALUE_OBJECT(*arg));
	}

	return NULL;
}

static void EGtkArg_update (Obj_t *obj)
{
	EGtkArg *base = (EGtkArg *) obj;
	gtk_object_arg_get(base->obj, &base->arg, NULL);
	obj = EGtkArg2Obj(&base->arg);
	Obj2Data(obj, base->type, base->data);
}

static void EGtkArg_sync (Obj_t *obj)
{
	EGtkArg *base = (EGtkArg *) obj;
	RefObj(obj);

	if	(IsTypeClass(obj->type, &Type_enum))
	{
		GTK_VALUE_ENUM(base->arg) = Val_int(obj->data);
		gtk_object_setv(base->obj, 1, &base->arg);
		return;
	}

	switch (GTK_FUNDAMENTAL_TYPE(base->arg.type))
	{
	case GTK_TYPE_INT:
		GTK_VALUE_INT(base->arg) = Obj2int(obj);
		break;
	case GTK_TYPE_LONG:
		GTK_VALUE_LONG(base->arg) = Obj2long(obj);
		break;
	case GTK_TYPE_ULONG:
		GTK_VALUE_ULONG(base->arg) = Obj2size(obj);
		break;
	case GTK_TYPE_BOOL:
		GTK_VALUE_BOOL(base->arg) = Obj2bool(obj) ? TRUE : FALSE;
		break;
	case GTK_TYPE_FLOAT:
		GTK_VALUE_FLOAT(base->arg) = Obj2double(obj);
		break;
	case GTK_TYPE_DOUBLE:
		GTK_VALUE_DOUBLE(base->arg) = Obj2double(obj);
		break;
	case GTK_TYPE_STRING:
		GTK_VALUE_STRING(base->arg) = Obj2str(obj);
		gtk_object_setv(base->obj, 1, &base->arg);
		memfree(GTK_VALUE_STRING(base->arg));
		return;
	default:
		UnrefObj(obj);
		break;
	}

	gtk_object_setv(base->obj, 1, &base->arg);
}

static EGTK_LVAL(EGtkArg_lval, EGtkArg_update, EGtkArg_sync);

Obj_t *EGtkArg2Lval (GtkObject *gtkobj, const char *name)
{
	GtkArg gtkarg;
	Obj_t *obj;
	EGtkArg *lval;
	
	gtkarg.name = (char *) name;
	gtk_object_arg_get(gtkobj, &gtkarg, NULL);
	obj = EGtkArg2Obj(&gtkarg);

	if	(obj == NULL)	return NULL;

	lval = (EGtkArg *) Obj_alloc(LVAL_SIZE(obj->type));
	lval->obj = gtkobj;
	lval->arg = gtkarg;
	lval->arg.name = mstrcpy(name);
	lval->data = (void *) (lval + 1);
	lval->reftype = NULL;
	lval->refcount = 1;
	lval->type = obj->type;
	lval->lval = &EGtkArg_lval;
	Obj2Data(obj, lval->type, lval->data);
	return (Obj_t *) lval;
}

Obj_t *GetEGtkArg (const Var_t *st, const Obj_t *obj)
{
	if	(!obj)
		return ConstObj(st->type, NULL);

	return LvalObj(&EGtkArg_lval, st->type, Val_ptr(obj->data), st->par);
}

static Obj_t *ConstEGtkArg (const Var_t *st, const Obj_t *obj)
{
	if	(obj)
	{
		GtkArg gtkarg;

		gtkarg.name = st->par;
		gtk_object_arg_get(Val_ptr(obj->data), &gtkarg, NULL);
		return EGtkArg2Obj(&gtkarg);
	}

	return NULL;
}

void AddEGtkArg(Type_t *type, EGtkArgDef *def, size_t dim)
{
	Var_t *var;
	char *p;

	while (dim-- > 0)
	{
		if	((p = strrchr(def->name, ':')) != NULL)
		{
			p++;
		}
		else	p = def->name;

		var = NewVar(NULL, p, 0);
		var->type = def->type ? GetType(def->type) : NULL;
		var->member = var->type ? GetEGtkArg : ConstEGtkArg;
		var->par = def->name;
		var->desc = NULL;
		AddVar(type->vtab, var, 1);
		def++;
	}
}
