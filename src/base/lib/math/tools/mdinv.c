/*	Matrixinversion
	(c) 1997 Erich Fr�hst�ck
	A-1090 Wien, W�hringer Stra�e 64/6

	Version 2.0
*/

#include <Math/mdmath.h>
#include <Math/linalg.h>

static void getdata (Konv_t *konv, mdaxis_t *x, mdaxis_t *x1, mdaxis_t *x2,
	char *p, char *data);

mdmat_t *mdinv (mdmat_t *base)
{
	mdaxis_t *axis, *xpre, *xpost, **ppre, **ppost;
	Konv_t konv;
	mdmat_t *md;
	int lines, cols;

	if	(base == NULL)
	{
		liberror(MSG_MDMATH, 1);
		return NULL;
	}

	if	(GetKonv(&konv, base->type, &Type_double) == NULL)
	{
		reg_set(1, type2str(base->type));
		errmsg(MSG_MDMATH, 2);
		return NULL;
	}

/*	Ergebnisachsen zusammenstellen
*/
	axis = base->axis;
	xpre = NULL;
	xpost = NULL;
	ppre = &xpre;
	ppost = &xpost;
	lines = cols = 1;

	for (axis = base->axis; axis != NULL; axis = axis->next)
	{
		if	(axis->flags & MDXFLAG_MARK)
		{
			*ppost = cpy_axis(axis, 0);
			(*ppost)->flags = MDXFLAG_MARK;
			ppost = &(*ppost)->next;
			cols *= axis->dim;
		}
		else
		{
			*ppre = cpy_axis(axis, 0);
			ppre = &(*ppre)->next;
			lines *= axis->dim;
		}
	}

	*ppre = xpost;

	if	(lines != cols)
	{
		liberror(MSG_MDMATH, 3);
		return NULL;
	}

/*	Datenmatrix generieren
*/
	md = new_mdmat();
	md->axis = xpre;
	md->type = &Type_double;
	md->size = md_size(md->axis, md->type->size);
	md->data = memalloc(md->size);

	getdata(&konv, md->axis, base->axis, base->axis, md->data, base->data);

	if	((cols = GaussJordan(md->data, lines)) != lines)
	{
		del_mdmat(md);
		reg_fmt(1, "%d", lines);
		reg_fmt(2, "%d", cols);
		liberror(MSG_MDMATH, 4);
		return NULL;
	}

	md->axis = xpost;
	*ppost = xpre;
	*ppre = NULL;
	md->size = md_size(md->axis, md->type->size);
	return md;
}


static void getdata (Konv_t *konv, mdaxis_t *x, mdaxis_t *x1, mdaxis_t *x2,
	char *p, char *data)
{
	if	(x != NULL)
	{
		int i;
		size_t size;

		while (x1 && (x1->flags & MDXFLAG_MARK))
			x1 = x1->next;

		while (x2 && !(x2->flags & MDXFLAG_MARK))
			x2 = x2->next;

		if	(x->flags & MDXFLAG_MARK)
		{
			size = x2->size;
			x2 = x2->next;
		}
		else
		{
			size = x1->size;
			x1 = x1->next;
		}

		for (i = 0; i < x->dim; i++)
		{
			getdata(konv, x->next, x1, x2, p, data);
			p += x->size;
			data += size;
		}
	}
	else	KonvData(konv, p, data);
}
