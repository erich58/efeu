/*	Nullstellenbestimmung mit Bisektionsverfahren
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 2.0
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
		reg_set(1, msprintf(SolveFmt, x0));
		reg_set(2, msprintf(SolveFmt, x1));
		liberror(MSG_SOLVE, 2);
	}

	tol = SolveEps * (x1 - x0);

	for (i = 0; i < SolveStep; ++i)
	{
		if	(SolveFlag)
		{
			reg_set(1, msprintf("%d", i));
			reg_set(2, msprintf(SolveFmt, x0));
			reg_set(3, msprintf(SolveFmt, x1));
			errmsg(MSG_SOLVE, 4);
		}

		x = .5 * (x0 + x1);
		y = (*f)(par, x);
		z = x1 - x0;

		if	(z < tol || y == 0)	return x;
		else if	(y * y0 > 0)		x0 = x, y0 = y;
		else				x1 = x, y1 = y;
	}

	errmsg(MSG_SOLVE, 3);
	return 0.;
}
