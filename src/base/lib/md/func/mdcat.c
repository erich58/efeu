/*	Achsen einer Datenmatrix aufsummieren
	(c) 1994 Erich Frühstück
*/

#include <EFEU/mdmat.h>

static mdaxis *add_axis(mdaxis **tab, size_t dim);
static void setval(EfiKonv *konv, mdaxis *x, char *px,
	mdaxis *y, char *py);

mdmat *md_cat(const char *def, mdmat **tab, size_t dim)
{
	size_t i;
	mdmat *md;
	mdlist *list;
	StrBuf *sb;
	mdaxis **x;
	char *fmt;
	char *p;
	mdaxis **ptr;
	EfiKonv konv;
	size_t ndim;

	if	(dim == 0)	return NULL;

	list = str2mdlist(def, 0);
	md = new_mdmat();

	if	(list == NULL)	list = str2mdlist("X", 0);

	fmt = NULL;

	if	(list->name && (fmt = strchr(list->name, '%')))
		*fmt++ = 0;

/*	Hauptachse und Datentype bestimmen, Achsenpointer setzen
*/
	sb = sb_create(0);
	sb_putstr(list->name, sb);
	md->type = NULL;
	ndim = max(dim, list->dim);
	x = memalloc(ndim * sizeof(mdaxis *));

	for (i = 0; i < ndim; i++)
	{
		if	(i >= list->dim)
		{
			if	(fmt)
			{
				p = mpsubarg(fmt, "ssd",
					tab[i] ? tab[i]->title : NULL,
					list->name, i + 1);
			}
			else	p = msprintf("%s%d", list->name, i + 1);

			sb_putstr(p, sb);
			memfree(p);
		}
		else	sb_putstr(list->list[i], sb);

		if	(i >= dim || tab[i] == NULL)
		{
			x[i] = NULL;
			continue;
		}
		else	x[i] = tab[i]->axis;

		if	(md->type == NULL || KonvDist(md->type, tab[i]->type)
				< KonvDist(tab[i]->type, md->type))
		{
			md->type = tab[i]->type;
		}
	}

	del_mdlist(list);
	md->axis = new_axis(ndim);
	p = (char *) sb->data;
	md->axis->name = mstrcpy(nextstr(&p));

	for (i = 0; i < ndim; i++)
		md->axis->idx[i].name = mstrcpy(nextstr(&p));

	if	(md->type == NULL)	md->type = &Type_ptr;

	sb_destroy(sb);

/*	Teilachsen bestimmen
*/
	ptr = &md->axis->next;

	while ((*ptr = add_axis(x, dim)) != NULL)
		ptr = &(*ptr)->next;

	memfree(x);

/*	Datenmatrix generieren
*/
	md->size = md_size(md->axis, md->type->size);
	md->data = memalloc(md->size);
	memset(md->data, 0, md->size);

/*	Werte einfügen
*/
	p = md->data;

	for (i = 0; i < dim; i++)
	{
		if	(tab[i])
		{
			Md_KonvDef(&konv, tab[i]->type, md->type);
			setval(&konv, md->axis->next, p,
				tab[i]->axis, tab[i]->data);
		}

		p += md->axis->size;
	}

	return md;
}


static mdaxis *add_axis(mdaxis **tab, size_t dim)
{
	int i;
	mdaxis *axis;

	axis = NULL;

	for (i = 0; i < dim; i++)
	{
		if	(tab[i] == NULL)	continue;

		if	(axis == NULL || axis->dim < tab[i]->dim)
			axis = tab[i];

		tab[i] = tab[i]->next;
	}

	return cpy_axis(axis, 0);
}

static void setval(EfiKonv *konv, mdaxis *x, char *px,
	mdaxis *y, char *py)
{
	if	(x != NULL)
	{
		int dim, sx, sy;

		dim = x->dim;
		sx = x->size;
		x = x->next;

		if	(y != NULL)
		{
			if	(dim > y->dim)	dim = y->dim;

			sy = y->size;
			y = y->next;
		}
		else	sy = 0;
			
		while (dim-- > 0)
		{
			setval(konv, x, px, y, py);
			py += sy;
			px += sx;
		}
	}
	else	KonvData(konv, px, py);
}
