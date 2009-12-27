/*
Arbeiten mit stückweisen Polynomen

$Copyright (C) 1991 Erich Frühstück
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

#include <Math/pnom.h>

static char *pnident(pnom_t *p)
{
	return msprintf("dim=%d deg=%d", p->dim, p->deg);
}

static pnom_t *pnadmin(pnom_t *tg, const pnom_t *src)
{
	if	(tg)
	{
		memfree(tg->c);
		memfree(tg->x);
		memfree(tg);
		return NULL;
	}
	else	return memalloc(sizeof(pnom_t));
}

ADMINREFTYPE(pnreftype, "Polynom", pnident, pnadmin);


/*	Speicherplatzgenerierung
*/

pnom_t *pnalloc(size_t n, size_t deg)
{
	pnom_t *p;
	int i;

	if	(n == 0)	return NULL;

	p = rd_create(&pnreftype);
	p->refcount = 1;
	p->dim = n;
	p->deg = deg++;
	p->c = memalloc(n * sizeof(double *));
	p->x = memalloc(n * (deg + 1) * sizeof(double));

	for (i = 0; i < n; i++)
		p->c[i] = p->x + n + deg * i;

	return p;
}


/*	Polynom kopieren
*/

#if	0
static pnom_t *cpy_pnom(const pnom_t *pn)
{
	pnom_t *p2;
	int i, j;

	if	(pn == NULL)	return NULL;

	do_debug("cpy_pnom", pn);
	p2 = pnalloc(pn->dim, pn->deg);

	for (i = 0; i < pn->dim; i++)
	{
		p2->x[i] = pn->x[i];

		for (j = 0; j <= pn->deg; j++)
			p2->c[j] = pn->c[j];
	}

	return p2;
}
#endif
