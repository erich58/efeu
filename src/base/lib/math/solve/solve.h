/*
Nullstellenberechnung

$Header <Math/$1>

$Copyright (C) 1994 Erich Frühstück
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

#ifndef	MATH_solve_h
#define	MATH_solve_h	1

#include <EFEU/efmain.h>
#include <EFEU/efio.h>


/*	Nullstellenberechnung
*/

#ifndef	MSG_SOLVE
#define	MSG_SOLVE	"solve"	/* Name für Fehlermeldungen */
#endif


double regfalsi (double (*f)(void *par, double x),
	void *fpar, double x0, double x1);
double bisection (int (*f)(void *par, double x),
	void *fpar, double x0, double x1);

void SetupSolve (void);	/* Parameter für Nullstellenberechnung */

extern int SolveFlag;	/* Protokollflag */
extern char *SolveFmt;	/* Ausgabeformat */
extern double SolveEps;	/* Genauigkeit */
extern int SolveStep;	/* Max. Stufenzahl */

#endif	/* MATH/solve.h */
