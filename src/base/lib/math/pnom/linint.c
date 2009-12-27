/*
Lineare Interpolation mit Polynomstruktur

$Copyright (C) 1991 Erich Fr�hst�ck
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

/*
Gegeben sind n+1 St�tzpunkte x[i], i=0,..,n mit den zugeh�rigen
Funktionswerten y[i]. Gesucht ist eine lineare Interpolation der
Funktion in den n Intervallen.
*/

#define	DIFF(x, i) (x[i+1] - x[i])	/* Differenzoperator */


pnom_t *linint(size_t n, double *x, double *y)
{
	pnom_t *p;	/* Polynomstruktur */
	int i;		/* Hilfsz�hler */

/*	Spezialf�lle
*/
	if	(n == 0)	return NULL;
	if	(n == 1)	return pnconst(x[0], y[0]);

/*	Speicherplatz generieren
*/
	p = pnalloc(n - 1, 1);

/*	Koeffizienten berechnen
*/
	for (i = 0; i < p->dim; i++)
	{
		p->x[i] = x[i];
		p->c[i][0] = y[i];
		p->c[i][1] = DIFF(y, i) / DIFF(x, i);
	}

	return p;
}
