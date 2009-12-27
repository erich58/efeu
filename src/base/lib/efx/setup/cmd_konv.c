/*
Standardkonvertierungen

$Copyright (C) 1994 Erich Frühstück
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

#include <EFEU/object.h>
#include <EFEU/cmdconfig.h>

void Ptr2bool (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = Val_ptr(arg[0]) != NULL;
}

void Vec2List (EfiFunc *func, void *rval, void **arg)
{
	long i, start, end;
	EfiObjList *list, **ptr;
	EfiVec *vec;

	list = NULL;
	ptr = &list;

	vec = Val_ptr(arg[0]);

	if	(func->dim > 1)
	{
		start = Val_int(arg[1]);
	}
	else	start = 0;

	if	(func->dim > 2)
	{
		end = start + Val_int(arg[2]);

		if	(!vec)			end = 0;
		if	(end > vec->buf.used)	end = vec->buf.used;
	}
	else	end = vec ? vec->buf.used : 0;

	for (i = start; i < end; i++)
	{
		*ptr = NewObjList(Vector(vec, i));
		ptr = &(*ptr)->next;
	}

	Val_list(rval) = list;
}

static void Type2vtab (EfiFunc *func, void *rval, void **arg)
{
	EfiType *type = Val_type(arg[0]);
	Val_vtab(rval) = type ? rd_refer(type->vtab) : NULL;
}

static void vtab2List (EfiFunc *func, void *rval, void **arg)
{
	EfiObjList *list, **ptr;
	EfiVarTab *vtab;
	
	vtab = Val_vtab(arg[0]);
	list = NULL;
	ptr = &list;

	if	(vtab)
	{
		VarTabEntry *p = vtab->tab.data;
		size_t n = vtab->tab.used;

		for (; n-- > 0; p++)
		{
			*ptr = NewObjList(str2Obj(mstrcpy(p->name)));
			ptr = &(*ptr)->next;
		}
	}

	Val_list(rval) = list;
}

/*
static void any2obj (EfiFunc *func, void *rval, void **arg)
{
	EfiObj *x = arg[0];
	Val_obj(rval) = ConstObj(x->type, x->data);
}
*/

/*	Ganzzahlkonvertierungen
*/

#define	CONV(name, src, tg)	\
static void name (EfiFunc *func, void *rval, void **arg) \
{ *((tg *) rval) = *((src *) arg[0]); }

CEXPR(k_bool2int, Val_int(rval) = Val_bool(arg[0]))
CEXPR(k_bool2uint, Val_uint(rval) = Val_bool(arg[0]))
CEXPR(k_bool2uint64, *((uint64_t *) rval) = Val_bool(arg[0]))

/*	Gleitkommakonvertierung
*/

CEXPR(k_double2bool, Val_bool(rval) = Val_double(arg[0]) != 0.)

CEXPR(k_bool2double, Val_double(rval) = Val_bool(arg[0]))

CEXPR(k_double2float, Val_float(rval) = Val_double(arg[0]))
CEXPR(k_float2double, Val_double(rval) = Val_float(arg[0]))

static void str2type (EfiFunc *func, void *rval, void **arg)
{
	Val_type(rval) = XGetType(Val_str(arg[0]));
}

static EfiFuncDef konv_func[] = {
/*	Ganzzahlkonvertierungen
*/
	{ FUNC_PROMOTION, &Type_int, "bool ()", k_bool2int },
	{ 0, &Type_uint, "bool ()", k_bool2uint },
	{ 0, &Type_uint64, "bool ()", k_bool2uint64 },

/*	Gleitkommakonvertierungen
*/
	{ FUNC_PROMOTION, &Type_double, "float ()", k_float2double },
	{ 0, &Type_double, "bool ()", k_bool2double },

	{ FUNC_RESTRICTED, &Type_float, "double ()", k_double2float },
	{ 0, &Type_bool, "bool (double)", k_double2bool },

/*	Sonstige Konvertierungen
*/
	{ 0, &Type_bool, "bool (_Ptr_)", Ptr2bool },
	{ 0, &Type_vtab, "Type_t ()", Type2vtab },
	{ 0, &Type_type, "str ()", str2type },
	/*
	{ FUNC_RESTRICTED, &Type_bool, "_Ptr_ ()", Ptr2bool },
	*/
	{ FUNC_RESTRICTED, &Type_list, "EfiVec ()", Vec2List },
	{ 0, &Type_list, "List_t (EfiVec, int start)", Vec2List },
	{ 0, &Type_list, "List_t (EfiVec, int start, int dim)", Vec2List },
	{ FUNC_RESTRICTED, &Type_list, "VarTab ()", vtab2List },
	/*
	{ FUNC_RESTRICTED, &Type_obj, "Object (.)", any2obj },
	*/
};

/*	Konvertierungen eintragen
*/

void CmdSetup_konv(void)
{
	AddFuncDef(konv_func, tabsize(konv_func));
}
