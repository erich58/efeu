/*	Nullstellenberechnung von Polynomen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#include <pconfig.h>
#include <pnom.h>
#include <efutil.h>


/*	Resourcedefinitionen
*/

double x0 = -1.;	/* Intervallanfang */
double x1 = 1.;		/* Intervallende */
double c0 = 0.;		/* Parameter */

Var_t vardef[] = {
	{ "x0", &Type_double, &x0 },
	{ "x1", &Type_double, &x1 },
	{ "c0", &Type_double, &c0 },
};



/*	Auswertungsfunktion für regfalsi
*/

staic double funk (void *par, double x)
{
	return pneval((pnom_t *) p, 0, x);
	/*
	return x * x - c0;
	*/
}


/*	Hauptprogramm
*/

int main (int narg, char **arg)
{
	pnom_t *pn;
	double x;
	int i;

	libinit(arg[0]);
	SetupSolve();
	pconfig(NULL, vardef, tabsize(vardef));
	loadarg(&narg, arg);

	pn = pnalloc(1, narg - 2);
	pn->x[0] = 0.;

	for (i = 1; i < narg; i++)
		pn->c[0][i-1] = atof(arg[i]);

	x = regfalsi(funk, (void *) pn, x0, x1);
	/*
	x = regfalsi(funk, NULL, x0, x1);
	*/

	printf(getmsg(NULL, 1, "%lf\n"), x);
	return 0;
}
