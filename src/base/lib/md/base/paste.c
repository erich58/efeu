/*	Zusammenfügen von Dimensionen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 2
*/

#include <EFEU/mdmat.h>

static mdidx_t *idx;

static void mkindex (const char *delim, mdaxis_t *x, char *last, int n);

static size_t getdim (mdaxis_t *x, int n);


static size_t getdim(mdaxis_t *x, int n)
{
	if	(n > 0)
	{
		return x->dim * getdim(x->next, n - 1);
	}
	else	return 1;
}


static void mkindex(const char *delim, mdaxis_t *x, char *last, int n)
{
	if	(--n >= 0)
	{
		int j;
		char *p;

		for (j = 0; j < x->dim; j++)
		{
			p = mstrpaste(delim, last, x->idx[j].name);
			mkindex(delim, x->next, p, n);
		}

		FREE(last);
	}
	else
	{
		idx->name = last;
		idx->flags = 0;
		idx++;
	}
}


void md_paste(mdmat_t *md, const char *name, const char *delim, int pos, int n)
{
	mdaxis_t *old, *new;
	mdaxis_t **ptr;
	int i;

	if	(n == 0)	return;

	if	(pos + n > md_dim(md->axis))
	{
		liberror(MSG_MDMAT, 41);
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
	new = new_axis(getdim(old, n));
	new->name = (name ? mstrcpy(name) : msprintf("%d", pos));
	new->size = old->size;
	idx = new->idx;
	mkindex(delim, old, NULL, n);
	*ptr = new;

	for (; n > 0; n--)
		old = old->next;

	new->next = old;
}
