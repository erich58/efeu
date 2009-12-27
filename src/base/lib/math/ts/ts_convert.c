/*
:*:convert time series
:de:Zeitreihen konvertieren

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

#include <EFEU/calendar.h>
#include <Math/TimeSeries.h>
#include <Math/pnom.h>
#include <ctype.h>

/*	Interpolationstype
*/

#define	IP_MASK		0xf
#define	IP_CONST	0x0	/* Konstant */
#define	IP_LINEAR	0x1	/* Linear */
#define	IP_MEAN		0x2	/* Mittelwertstabil */
#define	IP_SPLINE	0x3	/* Spline-Interpolation */
#define	IP_DSPLINE	0x4	/* Spline-Interpolation mit Ableitungsvorgabe */


/*	Auswertungstype
*/

#define	E_MASK		0xf0
#define	E_MEAN		0x10	/* Mittelwert */
#define	E_FIRST		0x20	/* Anfangswert */
#define	E_LAST		0x30	/* Endwert */
#define	E_CENTER	0x40	/* Zentralwert */
#define	E_SUM		0x50	/* Wertesumme */


/*	Steuerflags
*/

#define	F_DMODE		0x100	/* Tagesumwandlung forcieren */

static struct {
	char *key;
	int mask;
	int flag;
} key_tab[] = {
	{ "anteil", E_MASK, E_SUM },
	{ "first", E_MASK, E_FIRST },
	{ "last", E_MASK, E_LAST },
	{ "center", E_MASK, E_CENTER },

	{ "const", IP_MASK, IP_CONST },
	{ "linear", IP_MASK, IP_LINEAR },
	{ "mittel", IP_MASK, IP_MEAN },
	{ "spline", IP_MASK, IP_SPLINE },
	{ "dspline", IP_MASK, IP_DSPLINE },

	{ "dmode", 0, F_DMODE },
};


static int get_mode(const char *str)
{
	char **list;
	int i, j, dim, flags;

	if	(str == NULL)	return 0;

	flags = 0;
	dim = mstrsplit(str, "%s", &list);

	for (i = 0; i < dim; i++)
	{
		for (j = 0; j < tabsize(key_tab); j++)
		{
			if	(strcmp(key_tab[j].key, list[i]) == 0)
			{
				if	(key_tab[j].mask)
					flags &= ~key_tab[j].mask;

				flags |= key_tab[j].flag;
				break;
			}
		}
	}

	memfree(list);
	return flags;
}

typedef int (*IdxConv) (int idx);

static Polynom *ip_const (TimeSeries *ts, IdxConv iconv, int anteil);
static Polynom *ip_linear (TimeSeries *ts, IdxConv iconv, int anteil);
static Polynom *ip_mean (TimeSeries *ts, IdxConv iconv, int anteil);
static Polynom *ip_spline (TimeSeries *ts, IdxConv iconv, int anteil);
static Polynom *ip_dspline (TimeSeries *ts, IdxConv iconv, int anteil);

static double e_first (Polynom *pn, double a, double b);
static double e_last (Polynom *pn, double a, double b);
static double e_center (Polynom *pn, double a, double b);
static double e_mean (Polynom *pn, double a, double b);
static double e_sum (Polynom *pn, double a, double b);


/*	Indexkonvertierung
*/

#define	MBASE(x)	((x) == TS_YEAR || (x) == TS_QUART || (x) == TS_MONTH)
#define	MONTAG(x)	(7 * (x) + 0)


/*	Monatsindex
*/

static int jmonidx (int x) { return 12 * x; }
static int qmonidx (int x) { return 3 * x; }
static int mmonidx (int x) { return x; }


/*	Tagesindex
*/

static int jdayidx (int x) { return CalendarIndex(1, 1, x); }
static int qdayidx (int x) { return CalendarIndex(1, 1 + 3 * (x % 4), x / 4); }
static int mdayidx (int x) { return CalendarIndex(1, 1 + x % 12, x / 12); }
static int wdayidx (int x) { return MONTAG(x); }
static int ddayidx (int x) { return x; }

static IdxConv geticonv (int type, int mbase)
{
	if	(mbase)
	{
		switch (type)
		{
		case TS_YEAR:	return jmonidx;
		case TS_QUART:	return qmonidx;
		default:	return mmonidx;
		}
	}

	switch (type)
	{
	case TS_YEAR:	return jdayidx;
	case TS_QUART:	return qdayidx;
	case TS_MONTH:	return mdayidx;
	case TS_WEEK:	return wdayidx;
	default:	return ddayidx;
	}
}

static int day2mon(int x)
{
	CalInfo cal;
	int m;

	Calendar(x, &cal);
	m = 12 * cal.year + cal.month - 1;

	if	(cal.day > 1)	m++;

	return m;
}

static int newbase(int type, int mbase, int x)
{
	if	(mbase)
	{
		switch (type)
		{
		case TS_YEAR:	return x / 12;
		case TS_QUART:	return x / 3;
		default:	return x;
		}
	}

	switch (type)
	{
	case TS_YEAR:	return (day2mon(x) + 11) / 12;
	case TS_QUART:	return (day2mon(x) + 2) / 3;
	case TS_MONTH:	return day2mon(x);
	case TS_WEEK:	return (x + 6) / 7;
	default:	return x;
	}
}


/*	Interpolationsfunktionen
*/

static Polynom *ip_const(TimeSeries *ts, IdxConv konv, int flag)
{
	Polynom *pn;
	int i;

	pn = pnalloc(ts->dim, 0);
	
	for (i = 0; i < ts->dim; i++)
	{
		pn->x[i] = konv(ts->base.value + i) - 0.5;
		pn->c[i][0] = ts->data[i];
	}

	if	(flag)
	{
		double x = konv(ts->base.value + ts->dim) - 0.5;

		for (i = ts->dim - 1; i >= 0; i--)
		{
			pn->c[i][0] /= (x - pn->x[i]);
			x = pn->x[i];
		}
	}

	return pn;
}


static Polynom *ip_mean(TimeSeries *ts, IdxConv konv, int anteil)
{
	double *x, *y, y0;
	Polynom *p1, *p2;
	double a, b;
	int i;

	i = ts->dim + 1;
	x = memalloc(2 * i * sizeof(double));
	y = x + i;
	a = konv(ts->base.value) - 0.5;
	y0 = 0.;

	for (i = 0; i < ts->dim; i++)
	{
		x[i] = a;
		y[i] = y0;

		b = konv(ts->base.value + i + 1) - 0.5;

		if	(anteil)
		{
			y0 += ts->data[i];
		}
		else	y0 += ts->data[i] * (b - a);

		a = b;
	}

	x[ts->dim] = konv(ts->base.value + ts->dim) - 0.5;
	y[ts->dim] = y0;
	p1 = spline(ts->dim + 1, x, y);
	memfree(x);
	p2 = pndev(p1, 1);
	unref_pnom(p1);
	return p2;
}


static double *set_xy(TimeSeries *ts, IdxConv konv, int anteil)
{
	double *x, *y;
	double a, b;
	int i;

	if	(anteil)
	{
		x = memalloc(2 * ts->dim * sizeof(double));
		y = x + ts->dim;
	}
	else
	{
		x = memalloc(ts->dim * sizeof(double));
		y = NULL;
	}

	b = konv(ts->base.value + ts->dim) - 0.5;

	for (i = ts->dim - 1; i >= 0; i--)
	{
		a = konv(ts->base.value + i) - 0.5;
		x[i] = 0.5 * (a + b);
		
		if	(anteil)
			y[i] = ts->data[i] / (b - a);

		b = a;
	}

	return x;
}

static Polynom *ip_linear(TimeSeries *ts, IdxConv konv, int anteil)
{
	double *x, *y;
	Polynom *pn;

	x = set_xy(ts, konv, anteil);
	y = anteil ? x + ts->dim : ts->data;
	pn = linint(ts->dim, x, y);
	memfree(x);
	return pn;
}

static Polynom *ip_spline(TimeSeries *ts, IdxConv konv, int anteil)
{
	double *x, *y;
	Polynom *pn;

	x = set_xy(ts, konv, anteil);
	y = anteil ? x + ts->dim : ts->data;
	pn = spline(ts->dim, x, y);
	memfree(x);
	return pn;
}

static Polynom *ip_dspline(TimeSeries *ts, IdxConv konv, int anteil)
{
	double *x, *y;
	Polynom *p1, *p2;
	int i, j;

	x = set_xy(ts, konv, anteil);
	y = anteil ? x + ts->dim : ts->data;
	p1 = dspline(ts->dim, x, y, 0., 0.);
	p2 = pnalloc(p1->dim + 2, p1->deg);

	p2->x[0] = konv(ts->base.value) - 0.5;
	p2->c[0][0] = p1->c[0][0];

	for (j = 1; j <= p1->deg; j++)
		p2->c[0][j] = 0.;

	for (i = 0; i < p1->dim; i++)
	{
		p2->x[i+1] = p1->x[i];

		for (j = 0; j <= p1->deg; j++)
			p2->c[i+1][j] = p1->c[i][j];
	}

	i++;
	p2->x[i] = x[ts->dim - 1];
	p2->c[i][0] = pneval(p1, 0, p2->x[i]);

	for (j = 1; j <= p1->deg; j++)
		p2->c[i][j] = 0.;

	memfree(x);
	unref_pnom(p1);
	return p2;
}


/*	Auswertungsfunktionen
*/


static double e_first(Polynom *pn, double a, double b)
{
	return pneval(pn, 0, a);
}

static double e_last(Polynom *pn, double a, double b)
{
	return pneval(pn, 0, b);
}

static double e_center(Polynom *pn, double a, double b)
{
	return pneval(pn, 0, 0.5 * (a + b));
}


static double e_mean(Polynom *pn, double a, double b)
{
	return pnarea(pn, a, b) / (b - a);
}

static double e_sum(Polynom *pn, double a, double b)
{
	return pnarea(pn, a, b);
}


TimeSeries *ts_convert (const char *name, TimeSeries *ts,
	int type, const char *flags)
{
	int i, first, last;
	int mbase, anteil, mode;
	IdxConv iconv;
	Polynom *pn;
	double (*eval) (Polynom *pn, double a, double b);
	double x1, x2;

	if	(ts == NULL || ts->dim == 0 || ts->base.type == 0 || type == 0)
		return rd_refer(ts);

	mode = get_mode(flags);

/*	Indexbasis festlegen und Parameter abfragen
*/
	if	(mode & F_DMODE)
	{
		mbase = 0;
	}
	else	mbase = (MBASE(ts->base.type) && MBASE(type));

	iconv = geticonv(ts->base.type, mbase);

/*	Auswertungsfunktion bestimmen
*/
	anteil = 0;

	switch (mode & E_MASK)
	{
	case E_SUM:	eval = e_sum; anteil = 1; break;
	case E_FIRST:	eval = e_first; break;
	case E_LAST:	eval = e_last; break;
	case E_CENTER:	eval = e_center; break;
	default:	eval = e_mean; break;
	}

/*	Interpolationspolynom berechnen
*/
	switch (mode & IP_MASK)
	{
	case IP_LINEAR:	pn = ip_linear(ts, iconv, anteil); break;
	case IP_MEAN:	pn = ip_mean(ts, iconv, anteil); break;
	case IP_SPLINE:	pn = ip_spline(ts, iconv, anteil); break;
	case IP_DSPLINE:pn = ip_dspline(ts, iconv, anteil); break;
	default:	pn = ip_const(ts, iconv, anteil); break;
	}

/*	Zeitreihenbereich bestimmen
*/
	first = newbase(type, mbase, iconv(ts->base.value));
	last = newbase(type, mbase, iconv(ts->base.value + ts->dim));
	iconv = geticonv(type, mbase);
	ts = ts_create(name ? name : ts->name, tindex_create(type, first),
		last - first);

/*	Datenwerte berechnen
*/
	x2 = iconv(ts->base.value + ts->dim) - 0.5;

	for (i = ts->dim - 1; i >= 0; i--)
	{
		x1 = iconv(ts->base.value + i) - 0.5;
		ts->data[i] = eval(pn, x1, x2);
		x2 = x1;
	}

	unref_pnom(pn);
	return ts;
}
