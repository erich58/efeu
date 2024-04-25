/*	Zusammenfügen von Dimensionen
	(c) 1994 Erich Frühstück
*/

#include <EFEU/mdmat.h>
#include <assert.h>

static void mkindex (const char *delim, mdaxis *x, char *last, int n);

typedef struct {
	size_t dim;
	size_t size;
} SIZEINFO;

static int check_axis(SIZEINFO *info, mdaxis *x, int n)
{
	if	(n > 1)
	{
		if	(check_axis(info, x->next, n - 1))
			return 1;

		if	(x->size != info->dim * info->size)
			return 1;

		info->dim *= x->dim;
		return 0;
	}
	else
	{
		info->dim = x->dim;
		info->size = x->size;
		return 0;
	}
}


static mdindex *idx;
static StrPool *sbuf;

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
		idx->i_name = StrPool_add(sbuf, last);
		idx->flags = 0;
		idx++;
	}
}

static const char *mkname (StrBuf *buf, const char *delim, mdaxis *x, int n)
{
	sb_puts(StrPool_get(x->sbuf, x->i_name), buf);

	for (size_t i = 1; i < n; ++i)
	{
		x = x->next;
		sb_puts(delim, buf);
		sb_puts(StrPool_get(x->sbuf, x->i_name), buf);
	}

	return sb_cstr(buf);
}


int md_paste(mdmat *md, const char *name, const char *delim, int pos, int n)
{
	mdaxis *old, *new;
	mdaxis **ptr;

	if	(!md)
		return 0;

	int k = md_dim(md->axis);
	
	if	(pos < -k)	pos = 0;
	else if	(pos < 0)  	pos += k;
	else if	(pos >= k)	pos = k - 1;

	if	(n <= 0 || pos + n > k)
		n = k - pos;

fprintf(stderr, "PASTE: dim = %d, pos = %d, n = %d\n", k, pos, n);

/*	Achsen vor der Zusammenfassung
*/
	ptr = &md->axis;

	for (int i = 0; i < pos; i++)
		ptr = &(*ptr)->next;

	old = *ptr;

/*	Test auf notwendige reorganisation
*/
	SIZEINFO info;

	if	(check_axis(&info, old, n))
	{
		md_serialize(md);
		assert(check_axis(&info, old, n) == 0);
	}

/*	Achsen zusammenfassen
*/
	new = new_axis(md->sbuf, info.dim);
	new->size = info.size;

	if	(!name)
	{
		StrBuf *buf = sb_acquire();
		new->i_name = StrPool_add(md->sbuf, mkname(buf, delim, old, n));
		sb_release(buf);
	}
	else	new->i_name = StrPool_add(md->sbuf, name);

	idx = new->idx;
	sbuf = new->sbuf;
	mkindex(delim, old, NULL, n);
	*ptr = new;

	for (int i = 0; i < n; i++)
		old = old->next;

	new->next = old;
	return n;
}
