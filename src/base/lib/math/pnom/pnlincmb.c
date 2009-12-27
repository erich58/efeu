/*	Linearkombination von stückweisen Polynomen
	(c) 1992 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 2.0
*/

#include <EFEU/efmain.h>
#include <Math/pnom.h>


static void lc_sub (double c, double **a, size_t *deg, size_t n);

static pnom_t *lc_pn;	/* Polynompointer */
static size_t lc_dim;	/* Polynomdimension */
static double *lc_m;	/* Multiplikatoren */


/*	Polynome zusammenfassen
*/

static void lc_sub(double x, double **c, size_t *deg, size_t n)
{
	int i, j;

	for (j = 0; j <= lc_pn->deg; j++)
		lc_pn->c[lc_dim][j] = 0.;

	for (i = 0; i < n; i++)
	{
		for (j = 0; j <= deg[i]; j++)
		{
			lc_pn->c[lc_dim][j] += lc_m[i] * c[i][j];
		}
	}

	lc_pn->x[lc_dim++] = x;
}


/* VARARGS 1 */

pnom_t *pnlincmb(pnom_t *p, ...)
{
	pnom_t *x;
	pnom_t **pl;
	size_t n;
	size_t deg;
	va_list k;

/*	Bestimmung der Zahl der Polynome
*/
	va_start(k, p);

	for (n = 0, x = p; x != NULL; n++, x = va_arg(k, pnom_t *))
		(void) va_arg(k, double);

	va_end(k);

	if	(n == 0)	return NULL;

/*	Vektoren aufbauen
*/
	lc_m = ALLOC(n, double);
	pl = ALLOC(n, pnom_t *);
	va_start(k, p);
	deg = 0;

	for (n = 0, x = p; x != NULL; n++, x = va_arg(k, pnom_t *))
	{
		lc_m[n] = va_arg(k, double);
		pl[n] = x;

		if	(deg < x->deg)	deg = x->deg;
	}

	va_end(k);

/*	Polynom generieren
*/
	lc_dim = pnjoin(pl, n, NULL);
	lc_pn = pnalloc(lc_dim, deg);
	lc_dim = 0;
	(void) pnjoin(pl, n, lc_sub);
	FREE(pl);
	FREE(lc_m);
	return lc_pn;
}
