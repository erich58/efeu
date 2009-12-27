/*	Referenztype der OLS-Schätzparameter
	(c) 1997 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
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

REFTYPE(OLSPar_reftype, "OLSPar", par_ident, par_admin);


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
