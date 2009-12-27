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

#if	HAS_GTK

/*	L-Wert Zugriff auf Widget Argumente
*/

#define	LVAL_SIZE(type) (sizeof(EGtkArg) + type->size)

EfiObj *EGtkLval_alloc (EfiType *type, va_list list)
{
	EGtkArg *obj = (EGtkArg *) Obj_alloc(LVAL_SIZE(type));
	obj->obj = va_arg(list, GtkObject *);
	obj->arg.name = mstrcpy(va_arg(list, char *));
	obj->data = (void *) (obj + 1);
	return (EfiObj *) obj;
}

void EGtkLval_free (EfiObj *ptr)
{
	EGtkArg *obj = (EGtkArg *) ptr;
	CleanData(obj->type, obj->data, 1);
	memfree(obj->arg.name);
	UnrefObj(ptr);
}

char *EGtkLval_ident (const EfiObj *obj)
{
	EGtkArg *arg = (EGtkArg *) obj;
	return arg->arg.name ? mstrcpy(arg->arg.name) : NULL;
}

EfiObj *EGtkObject (GtkObject *obj)
{
	return NewPtrObj(GetType(gtk_type_name(GTK_OBJECT_TYPE(obj))), obj);
}

EfiObj *EGtkArg2Obj (GtkArg *arg)
{
	GtkType gtype;
	EfiType *otype;
	int64_t i64;
	uint64_t u64;
	char *s;
	
	if	(!arg)	return NULL;

	gtype = GTK_FUNDAMENTAL_TYPE(arg->type);

	switch (gtype)
	{
	case GTK_TYPE_INT:
		return int2Obj(GTK_VALUE_INT(*arg));
	case GTK_TYPE_LONG:
		i64 = GTK_VALUE_LONG(*arg);
		return NewObj(&Type_int64, &i64);
	case GTK_TYPE_ULONG:
		u64 = GTK_VALUE_ULONG(*arg);
		return NewObj(&Type_uint64, &u64);
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
		int val = GTK_VALUE_ENUM(*arg);
		return NewObj(otype, &val);
	}

	if	(IsTypeClass(otype, &EGtkObjectType))
	{
		return NewPtrObj(otype, GTK_VALUE_OBJECT(*arg));
	}

	return NULL;
}

static void EGtkArg_update (EfiObj *obj)
{
	EGtkArg *base = (EGtkArg *) obj;
	gtk_object_arg_get(base->obj, &base->arg, NULL);
	obj = EGtkArg2Obj(&base->arg);
	Obj2Data(obj, base->type, base->data);
}

static void EGtkArg_sync (EfiObj *obj)
{
	EGtkArg *base = (EGtkArg *) obj;
	int64_t i64;
	uint64_t u64;
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
		Obj2Data(obj, &Type_int64, &i64);
		GTK_VALUE_LONG(base->arg) = i64;
		break;
	case GTK_TYPE_ULONG:
		Obj2Data(obj, &Type_uint64, &u64);
		GTK_VALUE_ULONG(base->arg) = u64;
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

static EGTK_LVAL(EGtkArg_lval, "EGtkArg", EGtkArg_update, EGtkArg_sync);

EfiObj *EGtkArg2Lval (GtkObject *gtkobj, const char *name)
{
	GtkArg gtkarg;
	EfiObj *obj;
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
	return (EfiObj *) lval;
}

EfiObj *GetEGtkArg (const EfiStruct *st, const EfiObj *obj)
{
	if	(!obj)
		return ConstObj(st->type, NULL);

	return LvalObj(&EGtkArg_lval, st->type, Val_ptr(obj->data), st->par);
}

static EfiObj *ConstEGtkArg (const EfiStruct *st, const EfiObj *obj)
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

void AddEGtkArg(EfiType *type, EGtkArgDef *def, size_t dim)
{
	EfiStruct *var;
	char *p;

	while (dim-- > 0)
	{
		if	((p = strrchr(def->name, ':')) != NULL)
		{
			p++;
		}
		else	p = def->name;

		var = NewEfiStruct(NULL, p, 0);
		var->type = def->type ? GetType(def->type) : NULL;
		var->member = var->type ? GetEGtkArg : ConstEGtkArg;
		var->par = def->name;
		var->desc = NULL;
		AddStruct(type->vtab, var, 1);
		def++;
	}
}

#endif
