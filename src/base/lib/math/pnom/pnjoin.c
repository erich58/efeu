/*
Kombination von Polynomstrukturen

$Copyright (C) 1992 Erich Fr�hst�ck
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


#include <EFEU/efmain.h>
#include <Math/pnom.h>


static void mkkoef (pnom_t **p, size_t n);
static void setkoef (double x, pnom_t **p, size_t n);
static void trans (double a, double *c, size_t deg);

static size_t *join_ind = NULL;
static size_t *join_deg = NULL;
static double **join_koef = NULL;


/*	Translation der St�tzstelle um den Wert a
*/

static void trans(double a, double *c, size_t k)
{
	int i, j;
	double x;

	if	(a == 0.)	return;

/*	Umrechnen der Koeffizienten
*/
	for (i = 0; i <= k; i++)
	{
		x = 1.;

		for (j = 1; j <= k - i; j++)
		{
			x *= a * (i + j) / (double) j;
			c[i] += x * c[i + j];
		}
	}
}


/*	Speicherplatz f�r Koeffizientenmatrix generieren
*/

static void mkkoef(pnom_t **p, size_t n)
{
	size_t size;
	double *ptr;
	int i;

	size = 0;

	for (i = 0; i < n; i++)
	{
		join_deg[i] = (p[i] == NULL ? 0 : p[i]->deg);
		size += 1 + join_deg[i];
	}

	join_koef = (double **) memalloc(n * sizeof(double *)
		+ size * sizeof(double));
	ptr = (double *) (join_koef + n);

	for (i = 0; i < n; i++)
	{
		join_koef[i] = ptr;
		ptr += 1 + join_deg[i];
	}
}


/*	Koeffizientenmatrix initialisieren
*/

static void setkoef(double x, pnom_t **p, size_t n)
{
	int i, j;

	for (i = 0; i < n; i++)
	{
	/*	Nullpolynom
	*/
		if	(p[i] == NULL)
		{
			join_deg[i] = 0;
			join_koef[i][0] = 0.;
			continue;
		}

		join_deg[i] = p[i]->deg;

	/*	Koeffizienten umspeichern
	*/
		for (j = 0; j <= join_deg[i]; j++)
			join_koef[i][j] = p[i]->c[join_ind[i]][j];

	/*	Polynomgrad anpassen
	*/
		while (join_deg[i] > 0 && join_koef[i][join_deg[i]] == 0.)
			join_deg[i]--;

	/*	St�tzstelle Verschieben
	*/
		trans(x - p[i]->x[join_ind[i]], join_koef[i], join_deg[i]);
	}
}


/*	Polynome verkn�pfen
*/

size_t pnjoin(pnom_t **p, size_t n, void (*op) (double, double **, size_t *, size_t))
{
	double x = 0.;	/* Wert der St�tzstelle */
	size_t dim;	/* Z�hler f�r St�tzstellen */
	int i, k;	/* Hilfsz�hler */

/*	Die Initialisierung von x mit 0. ist nicht notwendig,
	sie dient nur zur Unterdr�ckung einer Kompilerwarnung.
*/
	if	(n == 0)	return 0;

/*	Initialisieren der Z�hler und bestimmen des Minimums
	der St�tzstellen. Mit k verden die Polynome
	verschieden vom Nullpolynom gez�hlt.
*/
	dim = 0;
	join_ind = memalloc(n * sizeof(size_t));
	join_deg = NULL;
	join_koef = NULL;

	for (i = k = 0; i < n; i++)
	{
		join_ind[i] = 0;

		if	(p[i] == NULL)
			continue;

		if	(k++ == 0 || x > p[i]->x[0])
			x = p[i]->x[0];
	}

/*	Falls kein Polynom verschieden vom Nullpolynom,
	Abbruch der Verarbeitung mit R�ckgabewert 0.
*/
	if	(k == 0)
	{
		memfree(join_ind);
		return dim;
	}

/*	Falls op definiert ist, Speicherplatz f�r
	Gradvektor und Koeffizientenmatrix generieren.
*/
	if	(op != NULL)
	{
		join_deg = memalloc(n * sizeof(size_t));
		mkkoef(p, n);
	}

/*	Schrittweise Abarbeitung der St�tzstellen
*/
	for (;;)
	{
		dim++;

	/*	Falls op verschieden von Null ist, werden die
		Koeffizienten berechnet und op aufgerufen.
	*/
		if	(op != NULL)
		{
			setkoef(x, p, n);
			(*op)(x, join_koef, join_deg, n);
		}

	/*	Erh�hen der Indizes f�r alle Polynome mit
		St�tzstellen <= x und ermitteln des neuen
		Minimums. Mit k werden die Polynome gez�hlt,
		deren St�tzstellenbereich noch nicht
		ausgesch�pft ist.
	*/
		for (i = k = 0; i < n; i++)
		{
			if	(p[i] == NULL)
				continue;

			if	(p[i]->x[join_ind[i]] <= x)
			{
				if	(join_ind[i] < p[i]->dim - 1)
					join_ind[i]++;
				else	continue;
			}

			if	(k++ == 0 || x > p[i]->x[join_ind[i]])
				x = p[i]->x[join_ind[i]];
		}

	/*	St�tzstellenbereich aller Polynome ist ausgesch�pft
	*/
		if	(k == 0)	break;

	/*	Reduktion der Indizies f�r alle Polynome mit
		St�tzstelle > x und Index > 0.
	*/
		for (i = 0; i < n; i++)
		{
			if	(p[i] == NULL)
				continue;

			if	(join_ind[i] > 0 && p[i]->x[join_ind[i]] > x)
				join_ind[i]--;
		}
	}

	memfree(join_ind);
	memfree(join_deg);
	memfree(join_koef);
	return dim;
}
