/*	Nullstellenberechnung mit Sekantenverfahren
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 2.0
*/

#include <Math/solve.h>
#include <EFEU/object.h>


double regfalsi (double (*f)(void *par, double x), void *par, double x0, double x1)
{
	double tol;	/* Fehlerintervall */
	double weight;	/* Gewicht des Sekantenanteils */
	double y, y0, y1; /* Funktionswerte */
	double x, z;	/* Hilfsvariablen */
	int i;		/* Hilfszähler */

	if	(x0 > x1)	x = x0, x0 = x1, x1 = x;

	if	((y0 = (*f)(par, x0)) == 0.)	return x0;
	if	((y1 = (*f)(par, x1)) == 0.)	return x1;

	if	(y0 * y1 >= 0.)
	{
		reg_set(1, msprintf(SolveFmt, x0));
		reg_set(2, msprintf(SolveFmt, x1));
		liberror(MSG_SOLVE, 2);
		return 0.5 * (x0 + x1);
	}

	tol = SolveEps * (x1 - x0);
	weight = 1.;

	for (i = 0; ; i++)
	{
		if	(SolveFlag)
		{
			reg_set(1, msprintf("%d", i));
			reg_set(2, msprintf(SolveFmt, x0));
			reg_set(3, msprintf(SolveFmt, x1));
			reg_set(4, msprintf("%g", weight));
			errmsg(MSG_SOLVE, 5);
		}

		x = x0 - y0 * (x1 - x0) / (y1 - y0);
		x = weight * x + (0.5 - 0.5 * weight) * (x0 + x1);
		y = (*f)(par, x);
		z = (x1 - x0);

		if	(z < tol || y == 0.)	return x;
		else if	(i >= SolveStep)	break;
		else if	(y * y0 > 0.)		x0 = x, y0 = y;
		else				x1 = x, y1 = y;

		z = (x1 - x0) / z;

		if	(z > 0.5)	weight = weight / 2.;
		else if	(z < 0.5)	weight = (1. + weight) / 2.;
	}

	errmsg(MSG_SOLVE, 3);
	return x;
}
