/*	Lineare Interpolation mit Polynomstruktur
	(c) 1991 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 2.0

Gegeben sind n+1 Stützpunkte x[i], i=0,..,n mit den zugehörigen
Funktionswerten y[i]. Gesucht ist eine lineare Interpolation der
Funktion in den n Intervallen.
*/

#include <Math/pnom.h>


#define	DIFF(x, i) (x[i+1] - x[i])	/* Differenzoperator */


pnom_t *linint(size_t n, double *x, double *y)
{
	pnom_t *p;	/* Polynomstruktur */
	int i;		/* Hilfszähler */

/*	Spezialfälle
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
