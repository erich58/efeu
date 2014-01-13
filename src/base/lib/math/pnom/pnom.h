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
} Polynom;

extern RefType pnreftype;


/*	Basisfunktionen
*/

Polynom *pnalloc (size_t dim, size_t deg);
Polynom *pnconst (double x, double y);

#define ref_pnom(x)	rd_refer(x)
#define unref_pnom(x)	rd_deref(x)

Polynom *pndev (Polynom *pn, int dev);
Polynom *pnint (Polynom *pn, double x, double y);

double pneval (Polynom *pn, int dev, double x);
double pnarea (Polynom *pn, double a, double b);


/*	Polynomverknüpfungen
*/

size_t pnjoin (Polynom **p, size_t n, void (*op) (double x,
	double **c, size_t *deg, size_t n));
Polynom *pnlincmb (Polynom *p, ...);
Polynom *pnmult (double c, Polynom *p1, Polynom *p2);


/*	Ein-/Ausgabe von Polynomen
*/

Polynom *pnload (const char *name);
void pnsave (Polynom *pn, const char *name);

Polynom *pnread (FILE *file);
int pnprint (IO *file, Polynom *pn, const char *fmt);


/*	Interpolationsfunktionen
*/

Polynom *linint (size_t n, double *x, double *y);
Polynom *spline (size_t n, double *x, double *y);
Polynom *cspline (size_t n, double *x, double *y);
Polynom *dspline (size_t n, double *x, double *y, double a, double b);


/*	Schnittstelle zu Befehlsinterpreter
*/

extern EfiType Type_pnom;

#define	pnom2Obj(x)	NewPtrObj(&Type_pnom, (x))
#define	Val_pnom(x)	((Polynom **) x)[0]


/*	Initialisierung
*/

void SetupPnom (void);

#endif	/* Math/pnom.h */
