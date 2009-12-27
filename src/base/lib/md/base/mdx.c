/*
:*:extended labels
:de:Erweiterte Labels

$Copyright (C) 2008 Erich Frühstück
This file is part of EFEU.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; see the file COPYING.Library.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <EFEU/mdmat.h>
#include <EFEU/MatchPar.h>

static char *mdx_ident (const void *ptr)
{
	const mdx_lbl *lbl = ptr;
	return msprintf("%s(%d, %d)", lbl->reftype->label,
		lbl->idx, lbl->cols);
}

static void mdx_clean (void *ptr)
{
	mdx_lbl *lbl = ptr;
	rd_deref(lbl->sbuf);
	memfree(lbl);
}

static RefType reftype = REFTYPE_INIT("mdx_lbl", mdx_ident, mdx_clean);

static VECBUF(dim_buf, 100, sizeof(size_t));

static size_t split_label (StrPool *pool, size_t offset, size_t lim)
{
	size_t *dim;
	char *p;
	dim_buf.used = 0;
	dim = vb_next(&dim_buf);
	*dim = offset;
	p = StrPool_get(pool, offset);

	while (p && *p)
	{
		IO *out = StrPool_open(pool);
		dim = vb_next(&dim_buf);
		*dim = StrPool_offset(pool);

		while (*p != 0)
		{
			if	(*p == ';' && (!lim || dim_buf.used < lim))
			{
				p++;
				break;
			}

			io_putc(*p++, out);
		}

		io_close(out);
	}

	while (lim && dim_buf.used < lim)
	{
		dim = vb_next(&dim_buf);
		*dim = 0;
	}

	return dim_buf.used;
}

void mdx_init (mdaxis *x)
{
	mdx_lbl *lbl;
	size_t n;

	if	(x->lbl)	return;

	lbl = memalloc(sizeof *lbl);
	lbl->sbuf = rd_refer(x->sbuf);
	lbl->idx = 0;
	lbl->cols = split_label(x->sbuf, x->i_desc, 0);
	lbl->tab = memalloc((x->dim + 1) * lbl->cols * sizeof(size_t));
	vb_fetch(&dim_buf, lbl->tab);

	for (n = 0; n < x->dim; )
	{
		split_label(x->sbuf, x->idx[n++].i_desc, lbl->cols);
		vb_fetch(&dim_buf, lbl->tab + n * lbl->cols);
	}

	x->lbl = rd_init(&reftype, lbl);
}

void mdx_choice (mdaxis *x, const char *def)
{
	MatchPar *mp;
	size_t n;

	mdx_init(x);
	mp = MatchPar_create(def, x->lbl->cols - 1);
	x->lbl->idx = 0;

	for (n = 1; n < x->lbl->cols; n++)
	{
		char *p = StrPool_get(x->lbl->sbuf, x->lbl->tab[n]);

		if	(MatchPar_exec(mp, p, n))
		{
			x->lbl->idx = n;
			break;
		}
	}

	rd_deref(mp);
}

void mdx_index (mdaxis *x, int n)
{
	mdx_init(x);

	if	(n < 0)		n += x->lbl->cols;

	x->lbl->idx = (n > 0 && n < x->lbl->cols) ? n : 0;
}

char *mdx_head (mdaxis *x)
{
	if	(x->lbl)
	{
		return StrPool_get(x->lbl->sbuf,
			x->lbl->tab[x->lbl->idx]);
	}
	else	return StrPool_get(x->sbuf, x->i_desc);
}

char *mdx_label (mdaxis *x, int n)
{
	if	(x->lbl)
	{
		return StrPool_get(x->lbl->sbuf,
			x->lbl->tab[(n + 1) * x->lbl->cols + x->lbl->idx]);
	}
	else	return StrPool_get(x->sbuf, x->idx[n].i_desc);
}
