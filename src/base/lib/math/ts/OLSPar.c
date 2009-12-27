/*
Referenztype der OLS-Schätzparameter

$Copyright (C) 1997 Erich Frühstück
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

#include <EFEU/object.h>
#include <Math/TimeSeries.h>

static ALLOCTAB(par_tab, 0, sizeof(OLSPar_t));

static OLSPar_t *par_admin (OLSPar_t *tg, const OLSPar_t *src)
{
	if	(tg)
	{
		int i;

		for (i = 0; i < tg->dim; i++)
		{
			memfree(tg->koef[i].name);
			rd_deref(tg->exogen[i]);
		}

		memfree(tg->koef);
		memfree(tg->exogen);
		rd_deref(tg->res);
		del_data(&par_tab, tg);
		return NULL;
	}
	else	return new_data(&par_tab);
}

static char *par_ident (const OLSPar_t *ts)
{
	if	(ts)
	{
		return msprintf("%d", ts->dim);
	}
	else	return mstrcpy("NULL");
}

ADMINREFTYPE(OLSPar_reftype, "OLSPar", par_ident, par_admin);


OLSPar_t *NewOLSPar (size_t dim)
{
	OLSPar_t *par;

	par = rd_create(&OLSPar_reftype);
	par->dim = dim;
	par->koef = memalloc(par->dim * sizeof(OLSKoef_t));
	par->exogen = memalloc(par->dim * sizeof(TimeSeries_t *));
	memset(par->koef, 0, par->dim * sizeof(OLSKoef_t));
	memset(par->exogen, 0, par->dim * sizeof(TimeSeries_t *));
	return par;
}
