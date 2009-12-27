/*	Achse einer Datenmatrix auswerten
	(c) 1995 Erich Frühstück
*/

#include <EFEU/mdmat.h>
#include <EFEU/MatchPar.h>

static void maineval (EfiObj *expr, mdaxis *axis, EfiObj **xobj,
	mdaxis *x, unsigned mask, void *data)
{
	int i;

	if	(axis && x == axis)	x = x->next;

	if	(x)
	{
		for (i = 0; i < x->dim; i++)
		{
			if	(x->idx[i].flags & mask)
				continue;

			Val_str((*xobj)->data) = StrPool_get(x->sbuf,
					x->idx[i].i_name);
			maineval(expr, axis, xobj + 1, x->next, mask,
				(char *) data + i * x->size);
		}

		Val_str((*xobj)->data) = NULL;
	}
	else
	{
		if	(axis)
		{
			for (i = 0; i < axis->dim; i++)
			{
				if	(axis->idx[i].flags & mask)
					continue;

				(*xobj)->data = (char *) data + i * axis->size;
				xobj++;
			}
		}
		else	(*xobj)->data = data;

		UnrefEval(RefObj(expr));
	}
}

static void md_xeval (mdmat *md, const char *name, unsigned mask, EfiObj *expr)
{
	mdaxis *x;
	mdaxis *axis;
	MatchPar *match;
	EfiObj **xobj;
	int i, n, dim, vdim;
	VarTabEntry *entry;

	if	(md == NULL)	return;

/*	Auswertungsachse bestimmen
*/
	dim = md_dim(md->axis);
	match = MatchPar_create(name, dim);
	axis = NULL;

	for (x = md->axis, n = 1; x != NULL; x = x->next, n++)
		if (MatchPar_exec(match, StrPool_get(x->sbuf, x->i_name), n))
			axis = x;

	rd_deref(match);

	PushVarTab(NULL, NULL);
	
	if	(axis)
	{
		vdim = dim - 1;

		for (i = 0; i < axis->dim; i++)
			if (!(axis->idx[i].flags & mask)) vdim++;
	}
	else	vdim = dim + 1;

	xobj = memalloc(vdim * sizeof *xobj);
	n = 0;

/*	Variablen für Achsennamen generieren
*/
	for (x = md->axis; x != NULL; x = x->next)
	{
		if	(x == axis)	continue;
	
		xobj[n] = ConstObj(&Type_str, NULL);
		entry = VarTab_next(NULL);
		entry->name = StrPool_get(x->sbuf, x->i_name);
		entry->type = &Type_str;
		entry->desc = NULL;
		entry->obj = xobj[n];
		entry->get = NULL;
		entry->data = NULL;
		entry->entry_clean = NULL;
		n++;
	}

/*	Variablen für Achsenwerte generieren
*/
	if	(axis)
	{
		for (i = 0; i < axis->dim; i++)
		{
			if	(axis->idx[i].flags & mask)
				continue;

			xobj[n] = LvalObj(&Lval_ptr, md->type, NULL);
			entry = VarTab_next(NULL);
			entry->name = StrPool_get(axis->sbuf,
				axis->idx[i].i_name);
			entry->type = md->type;
			entry->desc = NULL;
			entry->obj = xobj[n];
			entry->get = NULL;
			entry->data = NULL;
			entry->entry_clean = NULL;
			n++;
		}
	}
	else
	{
		xobj[n] = LvalObj(&Lval_ptr, md->type, NULL);
		entry = VarTab_next(NULL);
		entry->name = "this";
		entry->type = &Type_str;
		entry->desc = NULL;
		entry->obj = xobj[n];
		entry->get = NULL;
		entry->data = NULL;
		entry->entry_clean = NULL;
	}

	VarTab_qsort(NULL);
	maineval(expr, axis, xobj, md->axis, mask, md->data);
	PopVarTab();
	memfree(xobj);
}

void MF_xeval(EfiFunc *func, void *rval, void **arg)
{
	mdmat *md = ref_mdmat(Val_mdmat(arg[0]));
	md_xeval(md, Val_str(arg[1]), MDFLAG_LOCK, Val_xobj(arg[2]));
	Val_mdmat(rval) = md;
}
