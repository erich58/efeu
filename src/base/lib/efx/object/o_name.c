/*	Namensobjekt
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/object.h>
#include <EFEU/stdtype.h>


/*	Kopier und Auswertungsfunktionen
*/

static void n_clean (const Type_t *type, Name_t *tg);
static void n_copy (const Type_t *type, Name_t *tg, const Name_t *src);
static Obj_t *n_meval (const Type_t *type, const Name_t *ptr);
static Obj_t *n_seval (const Type_t *type, const Name_t *ptr);

Type_t Type_mname = EVAL_TYPE("_MemberName_", Name_t,
	(Eval_t) n_meval, (Clean_t) n_clean, (Copy_t) n_copy);
Type_t Type_sname = EVAL_TYPE("_ScopeName_", Name_t,
	(Eval_t) n_seval, (Clean_t) n_clean, (Copy_t) n_copy);

Name_t Buf_name = { NULL, NULL };

static void n_clean(const Type_t *type, Name_t *tg)
{
	memfree(tg->name);
	UnrefObj(tg->obj);
	tg->name = NULL;
	tg->obj = NULL;
}


static void n_copy(const Type_t *type, Name_t *tg, const Name_t *src)
{
	tg->name = mstrcpy(src->name);
	tg->obj = RefObj(src->obj);
}


/*	Namen auswerten
*/

static Obj_t *n_meval(const Type_t *type, const Name_t *name)
{
	return GetMember(RefObj(name->obj), name->name);
}

static Obj_t *n_seval(const Type_t *type, const Name_t *name)
{
	return GetScope(RefObj(name->obj), name->name);
}

Obj_t *Var2Obj (Var_t *var, const Obj_t *obj)
{
	if	(var == NULL)
		return NULL;

	if	(obj && var->member)
		return var->member(var, obj);

	if	(!var->data)
		return ConstObj(var->type, NULL);

	if	(var->dim)
	{
		Buf_vec.type = var->type;
		Buf_vec.data = var->data;
		Buf_vec.dim = var->dim;
		return NewObj(&Type_vec, &Buf_vec);
	}

	if	(var->type->dim)
	{
		Buf_vec.type = var->type->base;
		Buf_vec.dim = var->type->dim;
		Buf_vec.data = var->data;
		return NewObj(&Type_vec, &Buf_vec);
	}

	return LvalObj(var->type, NULL, var->data);
}


Obj_t *GetMember (Obj_t *obj, const char *name)
{
	Type_t *type;
	Obj_t *x;

	obj = EvalObj(obj, NULL);

	if	(obj == NULL || obj->type == NULL || name == NULL)
		return NULL;

	for (type = obj->type; type != NULL; type = type->base)
	{
		if	((x = GetVar(type->vtab, name, obj)) != NULL)
		{
			UnrefObj(obj);
			return x;
		}

		if	(type->dim)	break;
	}

	reg_cpy(1, obj->type->name);
	reg_cpy(2, name);
	errmsg(MSG_EFMAIN, 164);
	return NULL;
}


Obj_t *GetScope (Obj_t *obj, const char *name)
{
	VarTab_t *tab = Obj2Ptr(obj, &Type_vtab);

	if	(tab)
	{
		obj = GetVar(tab, name, NULL);
		DelVarTab(tab);

		if	(obj)	return obj;
	}

	message(NULL, MSG_EFMAIN, 166, 1, name);
	return NULL;
}
