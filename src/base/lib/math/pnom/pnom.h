/*
Arbeiten mit stückweisen Polynomen

$Header <Math/$1>

$Copyright (C) 1992 Erich Frühstück
This file is part of Math.

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

#ifndef	Math_pnom_h
#define	Math_pnom_h

#define	PNOM	"pnom"

#include <EFEU/efmain.h>
#include <EFEU/object.h>
#include <EFEU/refdata.h>


/*	Polynomstruktur
*/

typedef struct {
	REFVAR;		/* Referenzvariablen */
	size_t dim;	/* Zahl der Stützstellen */
	size_t deg;	/* Grad des Polynoms */
	double *x;	/* Stützstellen */
	double **c;	/* Koeffizientenvektor */
} pnom_t;

extern reftype_t pnreftype;


/*	Basisfunktionen
*/

pnom_t *pnalloc (size_t dim, size_t deg);
pnom_t *pnconst (double x, double y);

#define ref_pnom(x)	rd_refer(x)
#define unref_pnom(x)	rd_deref(x)

pnom_t *pndev (pnom_t *pn, int dev);
pnom_t *pnint (pnom_t *pn, double x, double y);

double pneval (pnom_t *pn, int dev, double x);
double pnarea (pnom_t *pn, double a, double b);


/*	Polynomverknüpfungen
*/

size_t pnjoin (pnom_t **p, size_t n, void (*op) (double x,
	double **c, size_t *deg, size_t n));
pnom_t *pnlincmb (pnom_t *p, ...);
pnom_t *pnmult (double c, pnom_t *p1, pnom_t *p2);


/*	Ein-/Ausgabe von Polynomen
*/

pnom_t *pnload (const char *name);
void pnsave (pnom_t *pn, const char *name);

pnom_t *pnread (FILE *file);
int pnprint (io_t *file, pnom_t *pn, const char *fmt);


/*	Interpolationsfunktionen
*/

pnom_t *linint (size_t n, double *x, double *y);
pnom_t *spline (size_t n, double *x, double *y);
pnom_t *cspline (size_t n, double *x, double *y);
pnom_t *dspline (size_t n, double *x, double *y, double a, double b);


/*	Schnittstelle zu Befehlsinterpreter
*/

extern Type_t Type_pnom;

#define	pnom2Obj(x)	NewPtrObj(&Type_pnom, (x))
#define	Val_pnom(x)	((pnom_t **) x)[0]


/*	Initialisierung
*/

void SetupPnom (void);

#endif	/* Math/pnom.h */
