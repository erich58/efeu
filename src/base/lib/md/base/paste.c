/*	Zusammenfügen von Dimensionen
	(c) 1994 Erich Frühstück
*/

#include <EFEU/mdmat.h>

static mdindex *idx;

static void mkindex (const char *delim, mdaxis *x, char *last, int n);

static size_t getdim (mdaxis *x, int n);


static size_t getdim(mdaxis *x, int n)
{
	if	(n > 0)
	{
		return x->dim * getdim(x->next, n - 1);
	}
	else	return 1;
}


static void mkindex (const char *delim, mdaxis *x, char *last, int n)
{
	if	(--n >= 0)
	{
		int j;
		char *p;

		for (j = 0; j < x->dim; j++)
		{
			p = mstrpaste(delim, last,
				StrPool_get(x->sbuf, x->idx[j].i_name));
			mkindex(delim, x->next, p, n);
			memfree(p);
		}
	}
	else
	{
		idx->i_name = StrPool_add(x->sbuf, last);
		idx->flags = 0;
		idx++;
	}
}


void md_paste(mdmat *md, const char *name, const char *delim, int pos, int n)
{
	mdaxis *old, *new;
	mdaxis **ptr;
	int i;

	if	(n == 0)	return;

	if	(pos + n > md_dim(md->axis))
	{
		dbg_error(NULL, "[mdmat:41]", NULL);
		return;
	}

/*	Achsen vor der Zusammenfassung
*/
	ptr = &md->axis;

	for (i = 0; i < pos; i++)
		ptr = &(*ptr)->next;

	old = *ptr;

/*	Achsen zusammenfassen
*/
	new = new_axis(md->sbuf, getdim(old, n));

	if	(!name)
	{
		char *p = msprintf("%d", pos);
		new->i_name = StrPool_add(md->sbuf, p);
		memfree(p);
	}
	else	new->i_name = StrPool_add(md->sbuf, name);

	new->size = old->size;
	idx = new->idx;
	mkindex(delim, old, NULL, n);
	*ptr = new;

	for (; n > 0; n--)
		old = old->next;

	new->next = old;
}
