/*
:*:access to Tcl variables
:de:Zugriff auf Tcl-Variablen

$Copyright (C) 2002 Erich Frühstück
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
#include <GUI/ETK.h>

#if	HAS_TCL

#if	HAS_TCL_OBJ

typedef Tcl_Obj *(*TCLOBJ) (void *data);

#if	HAS_TCL_VAR2EX

#define	IDXTYPE		char *
#define	NewIdx(name)	mstrcpy(name)
#define	DelIdx(idx)	memfree(idx)
#define	VarName(var)	((var)->idx)
#define	GetVar(ip,idx)	Tcl_GetVar2Ex((ip), (idx), NULL, TCL_GLOBAL_ONLY)

#define	SetVar(v,o)	Tcl_SetVar2Ex((v)->etk->interp,\
	(v)->idx, NULL, (v)->obj((o)->data), TCL_GLOBAL_ONLY)

#else

static Tcl_Obj *NewIdx (const char *name)
{
	Tcl_Obj *idx = Tcl_NewStringObj((char *) name, -1);
	Tcl_IncrRefCount(idx);
	return idx;
}

#define	IDXTYPE		Tcl_Obj *
#define	DelIdx(idx)	Tcl_DecrRefCount(idx)
#define	GetVar(ip,idx)	Tcl_ObjGetVar2((ip), (idx), NULL, TCL_GLOBAL_ONLY)
#define	VarName(var)	Tcl_GetStringFromObj((var)->idx, NULL)
#define	SetVar(v,o)	Tcl_ObjSetVar2((v)->etk->interp, \
	(v)->idx, NULL, (v)->obj((o)->data), TCL_GLOBAL_ONLY)

#endif	/* HAS_TCL_VAR2EX */

#else	/* !HAS_TCL_VAR */

#define	VarName(var)	((var)->name)
#define	SetVar(v,o)	Tcl_SetVar((v)->etk->interp, \
	(v)->name, Val_str((o)->data), TCL_GLOBAL_ONLY)

#endif	/* HAS_TCL_OBJ */

typedef struct {
	ETK *etk;
#if	HAS_TCL_OBJ
	IDXTYPE idx;
	TCLOBJ obj;
#else
	char *name;
#endif
} TCLVAR;

#define	TCLSIZE(type) (sizeof(EfiObj) + sizeof(TCLVAR) + (type)->size)

static EfiObj *tcl_alloc (EfiType *type, va_list list)
{
	EfiObj *obj = Obj_alloc(TCLSIZE(type));
	TCLVAR *var = (void *) (obj + 1);
	var->etk = rd_refer(va_arg(list, ETK *));
#if	HAS_TCL_OBJ
	var->idx = va_arg(list, IDXTYPE);
	var->obj = va_arg(list, TCLOBJ);
#else
	var->name = mstrcpy(va_arg(list, const char *));
#endif
	obj->data = (void *) (var + 1);
	memset(obj->data, 0, type->size);
	return obj;
}

static void tcl_free (EfiObj *obj)
{
	TCLVAR *var = (void *) (obj + 1);
#if	HAS_TCL_OBJ
	DelIdx(var->idx);
#else
	memfree(var->name);
#endif
	rd_deref(var->etk);
	Obj_free(obj, TCLSIZE(obj->type));
}

static void tcl_update (EfiObj *obj)
{
	TCLVAR *var = (void *) (obj + 1);
#if	HAS_TCL_OBJ
	EfiObj *right = ETK_obj(GetVar(var->etk->interp, var->idx));

	if	(right)
	{
		right = EvalObj(right, obj->type);
		AssignData(obj->type, obj->data, right->data);
		UnrefObj(right);
	}
#else
	char *data = Tcl_GetVar(var->etk->interp, var->name, TCL_GLOBAL_ONLY);
	AssignData(obj->type, obj->data, &data);
#endif
}

static void tcl_sync (EfiObj *obj)
{
	TCLVAR *var = (void *) (obj + 1);
	SetVar(var, obj);
}

static char *tcl_ident (const EfiObj *obj)
{
	TCLVAR *var = (void *) (obj + 1);
	return msprintf("tcl::%s", VarName(var));
}

#if	HAS_TCL_OBJ
static Tcl_Obj *obj_str (void *data)
{
	return Tcl_NewStringObj(Val_str(data), -1);
}

static Tcl_Obj *obj_int (void *data)
{
	return Tcl_NewIntObj(Val_int(data));
}

static Tcl_Obj *obj_dbl (void *data)
{
	return Tcl_NewDoubleObj(Val_double(data));
}

static Tcl_Obj *obj_bool (void *data)
{
	return Tcl_NewBooleanObj(Val_bool(data));
}
#endif

static EfiLval Lval_tcl = {
	tcl_alloc,
	tcl_free,
	tcl_update,
	tcl_sync,
	tcl_ident,
};

#if	HAS_TCL_OBJ
static EfiObj *make_lval (ETK *etk, const char *name)
{
	IDXTYPE idx = NewIdx(name);
	Tcl_Obj *obj = GetVar(etk->interp, idx);
	char *type = (obj && obj->typePtr) ? obj->typePtr->name : NULL;

	if	(type == NULL)
		return LvalObj(&Lval_tcl, &Type_str, etk, idx, obj_str);
	if	(mstrcmp("int", type) == 0)
		return LvalObj(&Lval_tcl, &Type_int, etk, idx, obj_int);
	if	(mstrcmp("double", type) == 0)
		return LvalObj(&Lval_tcl, &Type_double, etk, idx, obj_dbl);
	if	(mstrcmp("bool", type) == 0)
		return LvalObj(&Lval_tcl, &Type_bool, etk, idx, obj_bool);

	return LvalObj(&Lval_tcl, &Type_str, etk, idx, obj_str);
}
#endif
#endif

/*
:*:The function |$1| returns an lvalue object to access the
Tcl variable <name>. If the variable <name> was not used, the type
is |str| otherwise it depends on the curent assignment of <name>.
:de:Die Funktion |$1| liefert ein L-Wert Objekt der Tcl-Variable <name>.
Falls die Variable noch nicht verwendet wurde hat sie den Type |str|,
ansonsten hängt der Typ von der aktuellen Belegung der Variablen ab.
*/

EfiObj *ETK_var (ETK *etk, const char *name)
{
#if	HAS_TCL
	if	(etk && name)
	{
#if	HAS_TCL_OBJ
		return make_lval(etk, name);
#else
		return LvalObj(&Lval_tcl, &Type_str, etk, name);
#endif
	}
#endif
	return NULL;
}

/*
$SeeAlso
\mref{ETK(3)},
\mref{ETK_eval(3)},
\mref{ETK_func(3)},
\mref{ETK_parse(3)},
\mref{SetupETK(3)},
\mref{Tcl_GetVar(3)},
\mref{Tcl_SetVar(3)},
\mref{ETK(7)}.
*/
