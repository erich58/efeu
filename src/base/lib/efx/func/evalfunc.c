/*	Funktionen bestimmen und auswerten
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/object.h>
#include <EFEU/konvobj.h>

Obj_t *Expr_func (void *par, const ObjList_t *list)
{
	return EvalFunc(par, list);
}


typedef struct {
	Func_t *func;
	Obj_t **obj;
	void **arg;
} FUNCPAR;


static void fpar_init(FUNCPAR *par, Func_t *func)
{
	par->func = func;
	par->obj = ALLOC(func->dim, Obj_t *);
	par->arg = ALLOC(func->dim, void *);
}

static void fpar_clear(FUNCPAR *par, int n)
{
	int i;

	for (i = 0; i < n; i++)
		UnrefObj(par->obj[i]);

	memfree(par->obj);
	memfree(par->arg);
}

static int fpar_error(FUNCPAR *par, int n, int num)
{
	errmsg(MSG_EFMAIN, 60);
	ListFunc(ioerr, par->func);
	reg_fmt(1, "%d", n);
	errmsg(MSG_EFMAIN, num);
	fpar_clear(par, n);
	return 1;
}

static int test_lval (const Obj_t *obj, const Type_t *type)
{
	const Type_t *old;

	if	(obj == NULL)		return 1;
	if	(obj->lref == NULL)	return 1;
	if	(type == NULL)		return 0;

	for (old = obj->type; old != NULL; old = old->dim ? NULL : old->base)
		if (old == type) return 0;

	return 1;
}


static int fpar_set(FUNCPAR *par, int n, Obj_t *obj)
{
	register FuncArg_t *arg = par->func->arg + n;

	if	(arg->lval)
	{
		par->obj[n] = EvalObj(RefObj(obj), NULL);

		if	(test_lval(par->obj[n], arg->type))
			return fpar_error(par, n, 62);
	}
	else
	{
		par->obj[n] = EvalObj(RefObj(obj), arg->type);

		if	(par->obj[n] == NULL)
			return fpar_error(par, n, 61);
	}

	par->arg[n] = arg->type ? par->obj[n]->data : par->obj[n];
	return 0;
}

#define	listtype(type)	((type) == &Type_list || (type) == &Type_explist)
#define	is_list(obj)	((obj) && listtype((obj)->type))

Obj_t *EvalFunc(Func_t *func, const ObjList_t *list)
{
	FUNCPAR par;
	Obj_t *obj;
	int i, dim;

	if	(func == NULL)
		return NULL;

	if	(func->reftype == &VirFuncRefType)
		return EvalVirFunc((void *) func, list);

	if	(func->reftype != &FuncRefType)
	{
		liberror(MSG_EFMAIN, 65);
		return NULL;
	}

	if	(FuncDebugFlag)
	{
		ListFunc(ioerr, func);
		io_putc('\n', ioerr);
	}
	
	if	(func->dim == 0)
	{
		if	(list != NULL)
		{
			fpar_init(&par, func);
			return fpar_error(&par, 0, 63), NULL;
		}

		return MakeRetVal(func, NULL);
	}

	fpar_init(&par, func);
	dim = func->dim - func->vaarg;

	for (i = 0; i < dim; i++)
	{
		if	(list)
		{
			obj = list->obj;
			list = list->next;
		}
		else	obj = func->arg[i].defval;

		if	(fpar_set(&par, i, obj))
			return NULL;
	}

	if	(func->vaarg)
	{
		list = EvalObjList(list);
		par.arg[dim] = (list && !list->next && is_list(list->obj)) ?
			list->obj->data : &list;
	}
	else if	(list != NULL)
		return fpar_error(&par, dim, 63), NULL;

	obj = MakeRetVal(func, par.arg);
	fpar_clear(&par, dim);
	return obj;
}
