/*	Produktmatrix generieren
	(c) 1997 Erich Frühstück
*/

#include <EFEU/mdmat.h>

typedef struct {
	mdaxis *axis;
	EfiFunc *mul;
	EfiFunc *add;
	void *buf;
} GPAR;

static void mainwalk (GPAR *par, mdaxis *x, mdaxis *x1, mdaxis *x2,
	char *p, char *p1, char *p2);

static void subwalk (GPAR *par, mdaxis *x, char *p, char *p1, char *p2);


/*	Hauptprogramm
*/

mdmat *md_prod (mdmat *base)
{
	mdaxis *axis, *xpre, *xpost, **ppre, **ppost;
	GPAR par;
	mdmat *md;
	StrPool *sbuf;

	if	(base == NULL)
		return NULL;

/*	Multiplikations und Additionsfunktionen zusammenstellen
*/
	par.mul = GetFunc(NULL, GetGlobalFunc("*"), 2,
		base->type, 0, base->type, 0); 
	
	if	(par.mul == NULL)
	{
		log_error(NULL, "[mdmat:64]", "mm",
			type2str(base->type), type2str(base->type));
		return NULL;
	}

	par.add = mdfunc_add(par.mul->type);

	if	(par.add == NULL)
		return NULL;

	par.axis = base->axis;

/*	Ergebnisachsen zusammenstellen
*/
	axis = base->axis;
	xpre = NULL;
	xpost = NULL;
	ppre = &xpre;
	ppost = &xpost;
	sbuf = NewStrPool();

	for (axis = base->axis; axis != NULL; axis = axis->next)
	{
		if	(axis->flags & MDXFLAG_MARK)
		{
			*ppre = cpy_axis(sbuf, axis, 0);
			ppre = &(*ppre)->next;
			*ppost = cpy_axis(sbuf, axis, 0);
			(*ppost)->flags = MDXFLAG_MARK;
			ppost = &(*ppost)->next;
		}
	}

	*ppre = xpost;

/*	Datenmatrix generieren
*/
	md = new_mdmat();
	md->sbuf = sbuf;
	md->axis = xpre;
	md->type = (EfiType *) par.mul->type;
	md_alloc(md);
	memset(md->data, 0, (size_t) md->size);
	par.buf = memalloc(md->type->size);
	mainwalk(&par, md->axis, base->axis, base->axis,
		md->data, base->data, base->data);
	memfree(par.buf);
	return md;
}

/*	Ergebnisachse durchwandern
*/

static void mainwalk (GPAR *par, mdaxis *x, mdaxis *x1, mdaxis *x2,
	char *p, char *p1, char *p2)
{
	size_t s1, s2;
	int i;

	if	(x == NULL)
	{
		subwalk(par, par->axis, p, p1, p2);
		return;
	}

	while (x1 && !(x1->flags & MDXFLAG_MARK))
		x1 = x1->next;

	while (x2 && !(x2->flags & MDXFLAG_MARK))
		x2 = x2->next;

	if	(x->flags & MDXFLAG_MARK)
	{
		s2 = x2->size;
		x2 = x2->next;
		s1 = 0;
	}
	else
	{
		s1 = x1->size;
		x1 = x1->next;
		s2 = 0;
	}

	for (i = 0; i < x->dim; i++)
	{
		mainwalk(par, x->next, x1, x2, p, p1, p2);
		p += x->size;
		p1 += s1;
		p2 += s2;
	}
}


/*	Eliminationsachsen durchwandern
*/

static void subwalk (GPAR *par, mdaxis *x, char *p, char *p1, char *p2)
{
	int i;

	while (x && (x->flags & MDXFLAG_MARK))
		x = x->next;

	if	(x != NULL)
	{
		for (i = 0; i < x->dim; i++)
		{
			subwalk(par, x->next, p, p1, p2);
			p1 += x->size;
			p2 += x->size;
		}
	}
	else
	{
		CallFunc(par->mul->type, par->buf, par->mul, p1, p2);
		CallVoidFunc(par->add, p, par->buf);
	}
}
