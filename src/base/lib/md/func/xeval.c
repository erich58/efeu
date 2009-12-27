/*	Achse einer Datenmatrix auswerten
	(c) 1995 Erich Frühstück
*/

#include <EFEU/mdtest.h>

static void maineval(EfiObj *expr, mdaxis *axis, EfiVar *var,
	mdaxis *x, unsigned mask, void *data)
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

static void md_xeval(mdmat *md, const char *name, unsigned mask, EfiObj *expr)
{
	mdaxis *x;
	mdaxis *axis;
	mdtest *test;
	EfiVar *var;
	int i, n, dim;
	int vdim;

	if	(md == NULL)	return;

/*	Auswertungsachse bestimmen
*/
	dim = md_dim(md->axis);
	test = mdtest_create(name, dim);
	axis = NULL;

	for (x = md->axis, n = 1; x != NULL; x = x->next, n++)
		if (mdtest_eval(test, x->name, n)) axis = x;

	mdtest_clean(test);

/*	Variablentabelle generieren
*/
	if	(axis)
	{
		vdim = dim - 1;

		for (i = 0; i < axis->dim; i++)
			if (!(axis->idx[i].flags & mask)) vdim++;
	}
	else	vdim = dim + 1;

	var = memalloc(vdim * sizeof(EfiVar));
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

void MF_xeval(EfiFunc *func, void *rval, void **arg)
{
	mdmat *md;
	/*
	EfiObj *expr;
	IO *io;

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
