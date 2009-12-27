/*	Strukturfunktionen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/stfunc.h>


static ALLOCTAB(tab_stfunc, 0, sizeof(StFunc_t));


StFunc_t *NewStFunc(void)
{
	StFunc_t *func;

	func = new_data(&tab_stfunc);
	memset(func, 0, sizeof(StFunc_t));
	return func;
}


void DelStFunc(StFunc_t *func)
{
	if	(func)
	{
		DelStFunc(func->next);
		del_data(&tab_stfunc, func);
	}
}


static const char *st_name = NULL;
static VirFunc_t *st_tab = NULL;
static FuncArg_t *st_arg = NULL;
static int st_narg = 0;
static StFunc_t **st_ptr = NULL;


static void add_stdef (int offset, Type_t *type, size_t dim)
{
	StFunc_t *st;

	if	(type->list != NULL)
	{
		Var_t *s;
		StFunc_t **ptr;

		ptr = &st->next;

		for (s = type->list; s != NULL; s = s->next)
		{
			add_stdef(s->offset, s->type, s->dim);
		}
	}
	else
	{
		st = NewStFunc();
		st->offset = offset;
		st->size = type->size;
		st->dim = dim ? dim : 1;
		st_arg[0].type = type;
		st->func = XGetFunc(NULL, st_tab, st_arg, st_narg);

		if	(st->func == NULL)
		{
			reg_cpy(1, st_name);
			reg_cpy(2, type->name);
			errmsg(MSG_EFMAIN, 74);
		}

		*st_ptr = st;
		st_ptr = &st->next;
	}
}


/* VARARGS 4 */

StFunc_t *VaGetStFunc (const char *name, int narg, ...)
{
	StFunc_t *st;
	va_list list;

	if	(narg == 0)	return NULL;

	st_narg = narg;
	va_start(list, narg);
	st_arg = VaFuncArg(narg, list);
	va_end(list);
	st_name = name;
	st_tab = VirFunc(GetTypeFunc(st_arg[0].type, st_name));
	st_ptr = &st;
	st = NULL;
	add_stdef(0, st_arg[0].type, 0);
	FREE(st_arg);
	return st;
}


/* VARARGS 3 */

int CallStFunc (StFunc_t *stf, EvalStFunc_t eval, void *ptr, ...)
{
	StFunc_t *x;
	va_list list;
	void **args;
	char *base;
	char *pos;
	int i;
	int dim;

	if	(eval == NULL)	eval = IgnoreStRetVal;

	for (dim = 0, x = stf; x != NULL; x = x->next)
		if (dim < x->func->dim)	dim = x->func->dim;

	args = memalloc(dim * sizeof(void *));
	va_start(list, ptr);

	for (i = 0; i < dim; i++)
		args[i] = va_arg(list, void *);

	va_end(list);

	base = args[0];

	for (x = stf; x != NULL; x = x->next)
	{
		pos = base + x->offset;

		if	(x->func == NULL)	continue;

		for (i = 0; i < x->dim; i++)
		{
			int n;

			va_start(list, ptr);
			args[0] = pos;
			n = eval(x->func, ptr, args);
			va_end(list);

			if	(n == 0)	return 0;

			pos += x->size;
		}
	}

	return 1;
}


int IgnoreStRetVal (Func_t *func, void *ptr, void **arg)
{
	IgnoreRetVal(func, arg);
	return 1;
}

int CountStRetVal (Func_t *func, void *ptr, void **arg)
{
	int n;

	n = 0;
	KonvRetVal(&Type_int, &n, func, arg);
	Val_int(ptr) += n;
	return 1;
}

int TestStRetVal (Func_t *func, void *ptr, void **arg)
{
	int n;

	n = 0;
	KonvRetVal(&Type_int, &n, func, arg);
	return n;
}
