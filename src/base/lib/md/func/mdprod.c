/*	Produktmatrix generieren
	(c) 1997 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 2.0
*/

#include <EFEU/mdmat.h>

typedef struct {
	mdaxis_t *axis;
	Func_t *mul;
	Func_t *add;
	void *buf;
} GPAR;

static void mainwalk (GPAR *par, mdaxis_t *x, mdaxis_t *x1, mdaxis_t *x2,
	char *p, char *p1, char *p2);

static void subwalk (GPAR *par, mdaxis_t *x, char *p, char *p1, char *p2);


/*	Hauptprogramm
*/

mdmat_t *md_prod (mdmat_t *base)
{
	mdaxis_t *axis, *xpre, *xpost, **ppre, **ppost;
	GPAR par;
	mdmat_t *md;

	if	(base == NULL)
		return NULL;

/*	Multiplikations und Additionsfunktionen zusammenstellen
*/
	par.mul = GetFunc(NULL, GetGlobalFunc("*"), 2,
		base->type, 0, base->type, 0); 
	
	if	(par.mul == NULL)
	{
		reg_set(1, type2str(base->type));
		reg_set(2, type2str(base->type));
		liberror(MSG_MDMAT, 64);
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

	for (axis = base->axis; axis != NULL; axis = axis->next)
	{
		if	(axis->flags & MDXFLAG_MARK)
		{
			*ppre = cpy_axis(axis, 0);
			ppre = &(*ppre)->next;
			*ppost = cpy_axis(axis, 0);
			(*ppost)->flags = MDXFLAG_MARK;
			ppost = &(*ppost)->next;
		}
	}

	*ppre = xpost;

/*	Datenmatrix generieren
*/
	md = new_mdmat();
	md->axis = xpre;
	md->type = (Type_t *) par.mul->type;
	md->size = md_size(md->axis, md->type->size);
	md->data = memalloc(md->size);
	memset(md->data, 0, (size_t) md->size);
	par.buf = memalloc(md->type->size);
	mainwalk(&par, md->axis, base->axis, base->axis,
		md->data, base->data, base->data);
	FREE(par.buf);
	return md;
}

/*	Ergebnisachse durchwandern
*/

static void mainwalk (GPAR *par, mdaxis_t *x, mdaxis_t *x1, mdaxis_t *x2,
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

static void subwalk (GPAR *par, mdaxis_t *x, char *p, char *p1, char *p2)
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
