/*	Matrizen multiplizieren
	(c) 1994 Erich Frühstück
*/

#include <EFEU/mdmat.h>


typedef struct {
	mdaxis *x1;
	mdaxis *x2;
	EfiFunc *mul;
	EfiFunc *add;
	void *buf;
} GPAR;

static void mainwalk (GPAR *par, mdaxis *x, mdaxis *x1, mdaxis *x2, char *p, char *p1, char *p2);
static void subwalk (GPAR *par, mdaxis *x1, mdaxis *x2, char *p, char *p1, char *p2);


/*	Hauptprogramm
*/

mdmat *md_mul (mdmat *md1, mdmat *md2, int flag)
{
	mdaxis *x1, *x2;
	mdaxis *xpre, *xpost, **ppre, **ppost;
	GPAR par;
	mdmat *md;
	StrPool *sbuf;

	if	(md1 == NULL || md2 == NULL)
	{
		log_error(NULL, "[mdmat:61]", NULL);
		return NULL;
	}

	par.x1 = md1->axis;
	par.x2 = md2->axis;

/*	Multiplikations und Additionsfunktionen zusammenstellen
*/
	par.mul = GetFunc(NULL, GetGlobalFunc("*"), 2,
		md1->type, 0, md2->type, 0); 
	
	if	(par.mul == NULL)
	{
		log_error(NULL, "[mdmat:64]", "mm",
			type2str(md1->type), type2str(md2->type));
		return NULL;
	}

	par.add = mdfunc_add(par.mul->type);

	if	(par.add == NULL)
		return NULL;

/*	Ergebnisachsen zusammenstellen und gemeinsame Achsen auf Kompatiblität
	testen
*/
	x1 = md1->axis;
	x2 = md2->axis;
	xpre = NULL;
	xpost = NULL;
	ppre = &xpre;
	ppost = &xpost;
	sbuf = NewStrPool();

	while (x1 || x2)
	{
		if	(x1 && !(x1->flags & MDXFLAG_MARK))
		{
			*ppre = cpy_axis(sbuf, x1, 0);
			(*ppre)->flags = MDXFLAG_MARK;
			ppre = &(*ppre)->next;
			x1 = x1->next;
		}
		else if	(x2 && !(x2->flags & MDXFLAG_MARK))
		{
			*ppost = cpy_axis(sbuf, x2, 0);
			ppost = &(*ppost)->next;
			x2 = x2->next;
		}
		else if	(x1 && x2)
		{
			int n;

			if	((n = cmp_axis(x1, x2, flag)) != 0)
			{
				log_note(NULL, "[mdmat:62]", "ssd",
					StrPool_get(x1->sbuf, x1->i_name),
					StrPool_get(x2->sbuf, x2->i_name),
					n);
			}

			x1 = x1->next;
			x2 = x2->next;
		}
		else
		{
			del_axis(xpre);
			del_axis(xpost);
			log_error(NULL, "[mdmat:63]", NULL);
			return NULL;
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
	mainwalk(&par, md->axis, md1->axis, md2->axis, md->data, md1->data, md2->data);
	memfree(par.buf);
	return md;
}


/*	Primäre Achsen der ersten Matrix
*/

static void mainwalk (GPAR *par, mdaxis *x, mdaxis *x1, mdaxis *x2, char *p, char *p1, char *p2)
{
	int i;
	size_t s1;
	size_t s2;

	if	(x == NULL)
	{
		subwalk(par, par->x1, par->x2, p, p1, p2);
		return;
	}

	while (x1 && (x1->flags & MDXFLAG_MARK))
		x1 = x1->next;

	while (x2 && (x2->flags & MDXFLAG_MARK))
		x2 = x2->next;

	if	(x1)
	{
		s1 = x1->size;
		x1 = x1->next;
		s2 = 0;
	}
	else
	{
		s2 = x2->size;
		x2 = x2->next;
		s1 = 0;
	}

	for (i = 0; i < x->dim; i++)
	{
		mainwalk(par, x->next, x1, x2, p, p1, p2);
		p += x->size;
		p1 += s1;
		p2 += s2;
	}
}


/*	Gemeinsame Achsen
*/

static void subwalk(GPAR *par, mdaxis *x1, mdaxis *x2, char *p, char *p1, char *p2)
{
	int i, n;

	while (x1 && !(x1->flags & MDXFLAG_MARK))
		x1 = x1->next;

	while (x2 && !(x2->flags & MDXFLAG_MARK))
		x2 = x2->next;

	if	(x1 != NULL)
	{
		n = min(x1->dim, x2->dim);

		for (i = 0; i < n; i++)
		{
			subwalk(par, x1->next, x2->next, p, p1, p2);
			p1 += x1->size;
			p2 += x2->size;
		}
	}
	else
	{
		CallFunc(par->mul->type, par->buf, par->mul, p1, p2);
		CallVoidFunc(par->add, p, par->buf);
	}
}
