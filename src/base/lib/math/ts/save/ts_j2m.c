/*	Jahreszeitreihen Mittelwertstabil auf Monatszeitreihen umrechnen
	(c) 1997 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#include <Math/TimeSeries.h>
#include <Math/pnom.h>

static pnom_t *intpol(double *y, size_t dim)
{
	double *a;
	double *b;
	pnom_t *pn;
	int i;

	pn = pnalloc(dim, 2);
	a = ALLOC(dim + 1, double);
	b = ALLOC(dim + 1, double);
	b[0] = 0.;

	for (i = 1; i < dim; i++)
		b[i] = (y[i] - y[i-1]) / 4.;

	b[i] = 0.;
	a[0] = 4.;

	for (i = 1; i <= dim; i++)
	{
		a[i] = 4. - 1. / a[i-1];
		b[i] -= b[i-1] / a[i-1];
	}

	b[dim] = b[dim] / a[dim];

	for (i = dim - 1; i >= 0; i--)
		b[i] = (b[i] - b[i+1]) / a[i];

	for (i = 0; i < dim; i++)
	{
		pn->x[i] = 12. * i;
		pn->c[i][0] = y[i] - 8. * b[i] - 4 * b[i+1];
		pn->c[i][1] = 2. * b[i];
		pn->c[i][2] = (b[i+1] - b[i]) / 12.;
	}

	return pn;
}

void TimeSeriesKonv_j2m (TimeSeries_t *ts)
{
	pnom_t *pn;
	int i;

	if	(ts == NULL || ts->base.type != TS_YEAR)
	{
		errmsg(MSG_TS, 12);
		return;
	}

	pn = intpol(ts->data, ts->dim);
	ExpandTimeSeries(ts, ts->dim * 12);
	ts->base.type = TS_MONTH;
	ts->base.value *= 12;

	for (i = 0; i < ts->dim; i++)
		ts->data[i] = pneval(pn, 0, i + 0.5);
}
