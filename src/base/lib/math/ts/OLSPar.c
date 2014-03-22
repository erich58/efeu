/*
:*:parameter of OLS estimation
:de:OLS-Schätzparameter

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

static ALLOCTAB(par_tab, "OLSPar", 0, sizeof(OLSPar));

static char *par_ident (const void *ptr)
{
	const OLSPar *ts = ptr;
	return msprintf("%d", ts->dim);
}

static void par_clean (void *ptr)
{
	OLSPar *tg = ptr;
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
}

RefType OLSPar_reftype = REFTYPE_INIT("OLSPar", par_ident, par_clean);

OLSPar *OLSPar_create (size_t dim)
{
	OLSPar *par;

	par = new_data(&par_tab);
	par->dim = dim;
	par->koef = memalloc(par->dim * sizeof(OLSCoeff));
	par->exogen = memalloc(par->dim * sizeof(TimeSeries *));
	return rd_init(&OLSPar_reftype, par);
}

int OLSPar_print (IO *io, OLSPar *par)
{
	int i, n;
	char *p1, *p2;

	if	(par == NULL)	return 0;

	p1 = TimeIndex2str(par->base, 0);
	p2 = TimeIndex2str(par->base, par->nr - 1);
	n  = io_xprintf(io, "Schätzzeitraum:%10s\t%25s\n", p1, p2);
	memfree(p1);
	memfree(p2);
	n += io_xprintf(io, "Beobachtungen: %10d\t", par->nr);
	n += io_xprintf(io, "Freiheitsgrade:%10d\n", par->df);
	n += io_xprintf(io, "R**2:          %10.6f\t", par->r2);
	n += io_xprintf(io, "RBAR**2:       %10.6f\n", par->rbar2);
	n += io_xprintf(io, "Durbin Watson: %10.6f\t", par->dw);
	n += io_xprintf(io, "SEE:           %10.3f\n", par->see);
	n += io_xprintf(io, "\n");
	n += io_xprintf(io, "%-9s %15s %15s %15s\n", "Variable",
		"Koeffizient", "Standardfehler", "T-Statistic");

	for (i = 0; i < par->dim; i++)
		n += OLSCoeff_print(io, par->koef + i);

	return n;
}
