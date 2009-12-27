/*	Multidimensiomnale Datenmatrix über EDB-Datenfile laden
	$Copyright (C) 2005 Erich Frühstück
	A-3423 St.Andrä/Wördern, Wildenhaggasse 38
*/

#include <EFEU/mdmat.h>
#include <EFEU/printobj.h>

/*	Konvertierungsdefinition
*/

typedef struct {
	REFVAR;
	mdmat *md;
	IO *out;
	EDBAggregate *aggr;
	EfiStruct *var;
	EfiObj *obj;
	EfiFunc *f_add;
	char *pmode;
} EDB2MD;

static void edb2md_free (EDB2MD *par);

static void edb2md_clean (void *data)
{
	EDB2MD *par = data;
	edb2md_free(par);

	if	(par->pmode)
	{
		memfree(PrintListDelim);
		PrintListDelim = mstrcpy("\t");
		md_setflag(par->md, "#-1", 0, mdsf_mark, MDXFLAG_MARK, NULL, 0);
		md_print(par->out, par->md, par->pmode);
		memfree(par->pmode);
	}
	else
	{
		md_save(par->out, par->md, 0);
		rd_deref(par->out);
	}

	rd_deref(par->md);
	memfree(par);
}

static RefType edb2md_reftype = REFTYPE_INIT("EDB2MD", NULL, edb2md_clean);


/*	Indexstruktur
*/

typedef struct {
	const char *name;
	const char *desc;
	size_t offset;
	int val;
} IDX;

static int idx_cmp (const void *ap, const void *bp)
{
	const IDX *a = ap;
	const IDX *b = bp;

	if	(a->offset < b->offset)	return -1;
	if	(a->offset > b->offset)	return 1;

	if	(a->val < b->val)	return -1;
	if	(a->val > b->val)	return 1;

	return mstrcmp(a->name, b->name);
}

static VECBUF(idx_buf, 100, sizeof(IDX));

typedef struct IGRP {
	struct IGRP *next;
	size_t offset;
	int dim;
	int maxval;
	int *tab;
} IGRP;

static IGRP *igrp_alloc (IDX *base)
{
	IGRP *grp = memalloc(sizeof *grp);
	grp->next = NULL;
	grp->offset = base->offset;
	grp->maxval = 0;
	grp->tab = NULL;
	grp->dim = 0;
	return grp;
}

static void igrp_clean (IGRP *grp)
{
	if	(grp)
	{
		igrp_clean(grp->next);
		memfree(grp->tab);
		memfree(grp);
	}
}

static void add_type (EfiType *type, size_t offset)
{
	if	(type->dim)
		return;

	if	(type->list)
	{
		EfiStruct *st;

		for (st = type->list; st; st = st->next)
			if (!st->dim) add_type(st->type, offset + st->offset);

		return;
	}

	if	(!IsTypeClass(type, &Type_enum))
		return;

	while (type)
	{
		VarTabEntry *p;
		size_t k;
		IDX *idx;

		if	(!type->vtab)	continue;

		p = type->vtab->tab.data;
		k = type->vtab->tab.used;

		for (; k-- > 0; p++)
		{
			if	(p->obj && p->obj->type == type)
			{
				idx = vb_next(&idx_buf);
				idx->name = p->name;
				idx->desc = p->desc;
				idx->offset = offset;
				idx->val = Val_int(p->obj->data);
			}
		}

		type = type->base;
	}
}

static size_t idx_load (EfiType *type, size_t offset)
{
	idx_buf.used = 0;
	add_type(type, offset);
	return idx_buf.used;
}

static mdaxis *make_axis (StrPool *pool, const char *name, const char *desc)
{
	mdaxis *x;
	IDX *idx;
	IGRP *grp, **gp;
	size_t n, k;

	vb_qsort(&idx_buf, idx_cmp);
	x = new_axis(pool, idx_buf.used);
	x->i_name = StrPool_xadd(pool, name);
	x->i_desc = StrPool_xadd(pool, desc);

	idx = idx_buf.data;
	grp = igrp_alloc(idx);
	gp = &grp;

	for (n = 0; n < idx_buf.used; n++)
	{
		if	((*gp)->offset != idx[n].offset)
		{
			gp = &(*gp)->next;
			*gp = igrp_alloc(idx + n);
		}

		(*gp)->dim++;
		x->idx[n].i_name = StrPool_add(pool, idx[n].name);
		x->idx[n].i_desc = StrPool_add(pool, idx[n].desc);
		(*gp)->maxval = idx[n].val;
	}

	x->priv = grp;
	n = 1;

	for (grp = x->priv; grp; grp = grp->next)
	{
		grp->tab = memalloc((grp->maxval + 1) * sizeof grp->tab[0]);

		for (k = 0; k < grp->dim; k++)
		{
			grp->tab[idx->val] = n++;
			idx++;
		}

	}

	return x;
}

static void add_val(EDBAggregate *aggr, mdaxis *axis, char *tg, char *src)
{
	if	(axis)
	{
		IGRP *grp;

		for (grp = axis->priv; grp; grp = grp->next)
		{
			int n = Val_int(src + grp->offset);

			if	(n < 0 || n > grp->maxval)	continue;

			n = grp->tab[n];

			if	(n == 0)	continue;

			add_val(aggr, axis->next,
				tg + (n - 1) * axis->size, src);
		}
	}
	else	EDBAggregateData(aggr, tg, src);
}

static void edb2md_init (EDB2MD *par, EfiType *type)
{
	mdaxis **xptr;
	EfiStruct *st, **ptr;

	par->md = new_mdmat();
	par->md->sbuf = NewStrPool();
	par->var = NULL;
	ptr = &par->var;
	par->aggr = NULL;
	xptr = &par->md->axis;

	for (st = type->list; st; st = st->next)
	{
		if	(idx_load(st->type, st->offset))
		{
			*xptr = make_axis(par->md->sbuf, st->name, st->desc);
			xptr = &(*xptr)->next;
		}
		else
		{
			*ptr = NewEfiStruct(st->type, st->name, st->dim);
			par->aggr = NewEDBAggregate(par->aggr, "+=", *ptr,
				st->offset);
			ptr = &(*ptr)->next;
		}
	}

	if	(par->var == NULL)
	{
		if	(type->list)
		{
			par->md->type = &Type_uint;
			par->obj = NewObj(par->md->type, NULL);
			Val_uint(par->obj->data) = 1;
		}
		else
		{
			par->md->type = type;
			par->obj = NewObj(par->md->type, NULL);
		}

		par->var = NewEfiStruct(par->md->type, NULL, 0);
	}
	else if	(par->var->dim == 0 && par->var->next == NULL)
	{
		par->md->type = par->var->type;
		par->obj = NewObj(par->md->type, NULL);
	}
	else
	{
		par->md->type = MakeStruct(NULL, NULL, RefEfiStruct(par->var));
		par->obj = NewObj(par->md->type, NULL);
	}

	md_alloc(par->md);
	par->f_add = mdfunc_add(par->md->type);
}

static void edb2md_add (EDB2MD *p, void *data)
{
 	add_val(p->aggr, p->md->axis, p->md->data, data);
}

static void edb2md_free (EDB2MD *par)
{
	mdaxis *x;

	for (x = par->md->axis; x; x = x->next)
		igrp_clean(x->priv);

	UnrefObj(par->obj);
	rd_deref(par->f_add);
	DelEfiStruct(par->var);
	rd_deref(par->aggr);
}

mdmat *edb2md (EDB *edb)
{
	EDB2MD buf;

	if	(!edb)	return NULL;

	edb2md_init(&buf, edb->obj->type);

	while (edb_read(edb))
		edb2md_add(&buf, edb->obj->data);

	edb2md_free(&buf);
	return buf.md;
}

static size_t write_edb2md (EfiType *type, void *data, void *par)
{
	edb2md_add(par, data);
	return 1;
}

static void init_edb2md (EDB *edb, EDBPrintMode *mode, IO *io)
{
	EDB2MD *par;

	par = memalloc(sizeof *par);
	edb2md_init(par, edb->obj->type);
	par->out = io;
	par->pmode = mode->par;
	mode->par = NULL;
	edb->write = write_edb2md;
	edb->opar = rd_init(&edb2md_reftype, par);
}

static void pset_edb2md (EDBPrintMode *mode, const EDBPrintDef *def,
	const char *opt, const char *arg)
{
	mode->name = def->name;
	mode->init = init_edb2md;
	mode->split = 0;
	mode->par = arg ? msprintf("mode=%s %s", arg, opt) : NULL;
}

static EDBPrintDef pdef_edb2md = { "md", "[flags]=pmode", pset_edb2md, NULL,
	":*:write out as data cube"
	":de:Ausgabe als Datenmatrix"
};

void Setup_edb2md (void)
{
	static int setup_done = 0;

	if	(setup_done)	return;

	setup_done = 1;
	AddEDBPrintDef(&pdef_edb2md, 1);
}
