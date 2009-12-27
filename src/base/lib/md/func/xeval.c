/*	Achse einer Datenmatrix auswerten
	(c) 1995 Erich Frühstück
*/

#include <EFEU/mdmat.h>

static void maineval(Obj_t *expr, mdaxis_t *axis, Var_t *var, mdaxis_t *x, unsigned mask, void *data)
{
	int i;

	if	(axis && x == axis)	x = x->next;

	if	(x == NULL)
	{
		if	(axis)
		{
			for (i = 0; i < axis->dim; i++)
			{
				if	(axis->idx[i].flags & mask)	continue;

				var->data = (char *) data + i * axis->size;
				var++;
			}
		}
		else	var->data = data;

		UnrefEval(RefObj(expr));
		return;
	}

	for (i = 0; i < x->dim; i++)
	{
		if	(x->idx[i].flags & mask)	continue;

		var->data = &x->idx[i].name;
		maineval(expr, axis, var + 1, x->next, mask, (char *) data + i * x->size);
	}
}

static void md_xeval(mdmat_t *md, const char *name, unsigned mask, Obj_t *expr)
{
	mdaxis_t *x;
	mdaxis_t *axis;
	mdtest_t *test;
	Var_t *var;
	int i, n, dim;
	int vdim;

	if	(md == NULL)	return;

/*	Auswertungsachse bestimmen
*/
	dim = md_dim(md->axis);
	test = new_test(name, dim);
	axis = NULL;

	for (x = md->axis, n = 1; x != NULL; x = x->next, n++)
		if (mdtest(test, x->name, n)) axis = x;

	del_test(test);

/*	Variablentabelle generieren
*/
	if	(axis)
	{
		vdim = dim - 1;

		for (i = 0; i < axis->dim; i++)
			if (!(axis->idx[i].flags & mask)) vdim++;
	}
	else	vdim = dim + 1;

	var = ALLOC(vdim, Var_t);
	memset(var, 0, vdim * sizeof(Var_t));
	n = 0;

	for (x = md->axis; x != NULL; x = x->next)
	{
		if	(x == axis)	continue;
	
		var[n].name = x->name;
		var[n].type = &Type_str;
		var[n].data = NULL;
		var[n].dim = 0;
		n++;
	}

	if	(axis)
	{
		for (i = 0; i < axis->dim; i++)
		{
			if	(axis->idx[i].flags & mask)	continue;

			var[n].name = axis->idx[i].name;
			var[n].type = md->type;
			var[n].data = NULL;
			var[n].dim = 0;
			n++;
		}
	}
	else
	{
		var[n].name = "this";
		var[n].type = md->type;
		var[n].data = NULL;
		var[n].dim = 0;
		n++;
	}

	PushVarTab(NULL, NULL);
	AddVar(NULL, var, vdim);

	maineval(expr, axis, var, md->axis, mask, md->data);

	PopVarTab();
	memfree(var);
}

void MF_xeval(Func_t *func, void *rval, void **arg)
{
	mdmat_t *md;
	/*
	Obj_t *expr;
	io_t *io;

	io = io_cstr(Val_str(arg[2]));
	expr = Parse_block(io, EOF);
	io_close(io);
	md_xeval(md, Val_str(arg[1]), MDFLAG_LOCK, expr);
	UnrefObj(expr);
	*/
	md = ref_mdmat(Val_mdmat(arg[0]));
	md_xeval(md, Val_str(arg[1]), MDFLAG_LOCK, Val_xobj(arg[2]));
	Val_mdmat(rval) = md;
}
