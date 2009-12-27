/*
Nullstellenbestimmung mit Bisektionsverfahren

$Copyright (C) 1994 Erich Frühstück
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

#include <Math/solve.h>
#include <EFEU/object.h>


double bisection (int (*f)(void *par, double x), void *par, double x0, double x1)
{
	double tol;	/* Fehlerintervall */
	int y, y0, y1;	/* Funktionswerte */
	double x, z;	/* Hilfsvariablen */
	int i;		/* Hilfszähler */

	if	(x0 > x1)	x = x0, x0 = x1, x1 = x;

	if	((y0 = (*f)(par, x0)) == 0)	return x0;
	if	((y1 = (*f)(par, x1)) == 0)	return x1;

	if	(y0 * y1 > 0)
	{
		dbg_error(NULL, "[solve:2]", "mm", msprintf(SolveFmt, x0),
			msprintf(SolveFmt, x1));
	}

	tol = SolveEps * (x1 - x0);

	for (i = 0; i < SolveStep; ++i)
	{
		if	(SolveFlag)
		{
			dbg_note(NULL, "[solve:4]", "dmm", i,
				msprintf(SolveFmt, x0),
				msprintf(SolveFmt, x1));
		}

		x = .5 * (x0 + x1);
		y = (*f)(par, x);
		z = x1 - x0;

		if	(z < tol || y == 0)	return x;
		else if	(y * y0 > 0)		x0 = x, y0 = y;
		else				x1 = x, y1 = y;
	}

	dbg_note(NULL, "[solve:3]", NULL);
	return 0.;
}
