/*
Splineinterpolation mit Polynomstruktur

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

#include <Math/pnom.h>	    /* Polynomstruktur */

/*
Gegeben sind n+1 Stützpunkte x[i], i=0,..,n mit den zugehörigen
Funktionswerten y[i]. Gesucht ist eine Interpolation der Funktion
in den n Intervallen mit stückweisen Polynomen 3. Grades.
Achtung: Der Aufrufparameter n gibt die Zahl der Stützstellen an
und muss daher um 1 reduziert werden, um der obigen Bedeutung zu
entsprechen.
In allen Programmen wird p als Name der Polynomstruktur verwendet.
Die Koeffizienten werden durch Makros repräsentiert um eine
bessere Lesbarkeit der Routinen zu erreichen. Bei der Spline-
interpolation werden einzelne Koeffizientenvektoren für Zwischen-
ergebnisse benötigt. Um die Übersichtlichkeit zu bewahren,
werden sie über Makros unter anderen Namen angesprochen.
Die Splineinterpolation wird in 3 Varianten angeboten:
a)      2. Ableitung an den Randpunkten ist 0,
b)      Funktion ist periodisch,
c)      1. Ableitung an den Randpunkten ist vorgegeben.
*/

static void koef (pnom_t *p, size_t n, double *x, double *y, double m);
static pnom_t *init (size_t n, double *x, double *y);

/*	Differenzoperator
*/

#define	DIFF(x, i)	(x[i+1] - x[i])

/*	Zugriffmakros für Koeffizientenvektoren
*/

#define	X0(i)	p->x[i]		/* Stützstellen */

#define	C0(i)	p->c[i][0]	/* konstanter Term */
#define	C1(i)	p->c[i][1]	/* linearer Term */
#define	C2(i)	p->c[i][2]	/* quadratischer Term */
#define	C3(i)	p->c[i][3]	/* kubischer Term */

#define	DX(i)	p->x[i]		/* Intervallänge */
#define	DY(i)	p->c[i][1]	/* Zuwachsrate */

#define	M(i)	p->c[i][2]	/* Momente der Splinefunktion */
#define	D(i)	p->c[i][2]	/* Hilfsvektor */
#define	A(i)	p->c[i][3]	/* Hilfsvektor */
#define	B(i)	p->c[i][0]	/* Hilfsvektor */


/*	Allgemeine Bemerkungen zu den Splineinterpolationen


Ist M(i) die 2. Ableitung der gesuchten Funktion an der Stelle
x[i], so errechnen sich die Koeffizienten nach den Formeln:

C0(i) = y[i],

		2 * M(i) + M(i+1)
C1(i) = DY(i) - ----------------- * DX(i),
			 6

C2(i) = M(i) / 2,

	M(i+1) - M(i)
C3(i) = -------------,
	  6 * DX(i)

wobei zusätzlich DY(i) = (y[i+1] - y[i]) / (x[i+1] - x[i])
und DX(i) = x[i+1] - x[i] definiert wurde.

In allen 3 Varianten der Splineinterpolation ergeben sich für
i=1,..n-1 die Gleichungen:

B(i) * M(i-1) + M(i) + A(i) * M(i+1) = D(i),

mit

       1        DX(i)
A(i) = - * ---------------,
       2   DX(i) + DX(i-1)

       1       DX(i-1)       1
B(i) = - * --------------- = - - A(i)
       2   DX(i) + DX(i-1)   2

und

	   DY(i) - DY(i-1)
D(i) = 3 * ---------------.
	   DX(i) + DX(i-1)

*/


/*	Generierung einer Polynomstruktur und Initialisierung
	der Werte für DX, DY, A, B und D.
*/

static void do_init(pnom_t *p, double *x, double *y)
{
	double z;	/* Hilfsvariable */
	int i;		/* Hilfszähler */

	for (i = 0; i < p->dim; ++i)
	{
		DX(i) = DIFF(x, i);
		DY(i) = DIFF(y, i) / DX(i);
	}

	for (i = 1; i < p->dim; ++i)
	{
		z = .5 / (DX(i) + DX(i-1));
		D(i) = 6. * z * (DY(i) - DY(i-1));
		A(i) = z * DX(i);
		B(i) = .5 - A(i);
	}
}

static pnom_t *init(size_t n, double *x, double *y)
{
	pnom_t *p = pnalloc(n, 3);
	do_init(p, x, y);
	return p;
}


/*	Abspeichern der Koeffizienten

Achtung: M und C2 haben dieselbe Speicheradresse, ebenso
DY und C1, bzw. DX und X0. Da der Vektor M nur n Werte fassen kann,
muss das n-te Moment explizit übergeben werden.
*/

static void koef(pnom_t *p, size_t n, double *x, double *y, double m)
{
	double z;	/* Hilfsvariable */
	int i;		/* Hilfszähler */

	for (i = 0; i < n; ++i)
	{
		z = (i == n-1 ? m : M(i+1));
		C1(i) -= (2. * M(i) + z) * DX(i) / 6.;
		C3(i) = (z - M(i)) / (6. * DX(i));
		C0(i) = y[i];
		C2(i) *= .5;
		X0(i) = x[i];
	}
}


/*	Splineinterpolation - Standard

Die Momente an der Stelle x[0] und x[n] sind 0.
*/

pnom_t *spline(size_t n, double *x, double *y)
{
	pnom_t *p;	/* Polynomstruktur */
	double z;	/* Hilfsvariable */
	int i;		/* Hilfszähler */

/*	Sonderfälle behandeln
*/
	if	(n == 0)	return NULL;
	if	(n == 1)	return pnconst(x[0], y[0]);

/*	Gleichungssystem initialisieren
*/
	n--;
	p = init(n, x, y);
	D(0) = A(0) = 0.;

/*	Gleichungssystem auf Diagonalform bringen
*/
	for (i = 1; i < n; ++i)
	{
		z = 1. / (1. - B(i) * A(i-1));
		D(i) -= B(i) * D(i-1);
		A(i) *= z;
		D(i) *= z;
	}

/*	Berechnung der Momente: Beachte, dass die Vektoren M und D
	auf die gleiche Speicheradresse verweisen. Wegen M(n) = 0,
	gilt M(n-1) = D(n-1).
*/
	for (i = n - 2; i >= 0; --i)
		M(i) -= A(i) * M(i+1);

	koef(p, n, x, y, 0.);
	return p;
}


/*	Splineinterpolation - periodische Funktion

Im Fall einer periodischen Funktion ergibt sich die zusätzliche
Gleichung:

M(0) + A(0) * M(1) + B(0) * M(n-1) = D(0),

mit

       1       DX(0)
A(0) = - * ---------------,
       2   DX(0) + DX(n-1)

B(0) = 1/2 - A(0)

und

	   DY(0) - DY(n-1)
D(0) = 3 * ---------------.
	   DX(0) + DX(n-1)

Achtung: Das Gleichungssystem ist nicht Tridiagonal, wie bei den
anderen Splineinterpolationen. Die Auflösung ist daher komplexer.
*/

pnom_t *cspline(size_t n, double *x, double *y)
{
	pnom_t *p;	/* Polynomstruktur */
	double z;	/* Hilfsvariable */
	int i;		/* Hilfszähler */

/*	Sonderfälle behandeln
*/
	if	(n == 0)	return NULL;
	if	(n == 1)	return pnconst(x[0], y[0]);

/*	Gleichungssystem initialisieren
*/
	n--;
	p = init(n, x, y);

	z = .5 / (DX(0) + DX(n-1));
	D(0) = 6. * z * (DY(0) - DY(n-1));
	A(0) = z * DX(0);
	B(0) = .5 - A(0);

/*	Umformen der ersten n-1 Gleichungen
*/
	for (i = 1; i < n - 1; ++i)
	{
		z = 1. / (1. - B(i) * A(i-1));
		D(i) -= B(i) * D(i-1);
		B(i) = -z * B(i) *B(i-1);
		A(i) *= z;
		D(i) *= z;
	}

/*	Umformen der letzten Gleichung.
*/
	z = 1. / (1. - B(n-1) * (A(n-2) + B(n-2)));
	D(n-1) -= B(n-1) * D(n-2);
	D(n-1) *= z;
	A(n-1) *= z;

	for (i = 0; i < n - 1; ++i)
	{
		z = 1. / (1. - A(n-1) * B(i));
		D(n-1) -= A(n-1) * D(i);
		A(n-1) *= -z * A(i);
		D(n-1) *= z;
	}

/*	Berechnung der Momente: Beachte, dass die Vektoren M und D
	auf die gleiche Speicheradresse verweisen.
*/
	M(n-1) /= 1. + A(n-1);

	for (i = n - 2; i >= 0; --i)
		M(i) -= B(i) * M(n-1) + A(i) * M(i+1);

	koef(p, n, x, y, M(0));
	return p;
}


/*	Splineinterpolation - Ableitung an den Randpunkten

Gegenüber der einfachen Splineinterpolation ergeben sich zwei
zusätzliche Gleichungen der Form:

M(0) + A(0) * M(1) = D(0), und

B(n) * M(n-1) + M(n) = D(n), mit den Koeffizienten

A(0) = .5,

	   DY(0) - y'(x[0])
D(0) = 3 * ---------------,
		DX(0)

B(n) = .5, und

	   y'(x[n]) - DY(n-1)
D(n) = 3 * ------------------.
		 DX(n-1)

Achtung: Die Vektoren können nur n-1 Werte fassen. Die letzte
Gleichung wird daher gesondert behandelt und M(n) in der Variablen
m zwischengespeichert.
*/

pnom_t *dspline(size_t n, double *x, double *y, double a, double b)
{
	pnom_t *p;	/* Polynomstruktur */
	double z;	/* Hilfsvariable */
	double m;	/* Moment an der Stelle x[n] */
	int i;		/* Hilfszähler */

/*	Sonderfälle behandeln
*/
	if	(n == 0)	return NULL;

	if	(n == 1)
	{
		p = pnalloc(1, 1);
		X0(0) = x[0];
		C0(0) = y[0];
		C1(0) = (a + b) / 2.;
		return p;
	}

/*	Gleichungssystem initialisieren
*/
	n--;
	p = init(n, x, y);
	A(0) = .5;
	D(0) = 3. * (DY(0) - a) / DX(0);

/*	Gleichungssystem auf Diagonalform bringen
*/
	for (i = 1; i < n; ++i)
	{
		z = 1. / (1. - B(i) * A(i-1));
		D(i) -= B(i) * D(i-1);
		A(i) *= z;
		D(i) *= z;
	}

/*	Berechnung der Momente: Beachte, daß die Vektoren M und D
	auf die gleiche Speicheradresse verweisen.
*/
	m = 3. * (b - DY(n-1)) / DX(n-1) - .5 * D(n-1);
	m /= 1. - .5 * A(n-1);
	M(n-1) -= A(n-1) * m;

	for (i = n - 2; i >= 0; --i)
		M(i) -= A(i) * M(i+1);

	koef(p, n, x, y, m);
	return p;
}
