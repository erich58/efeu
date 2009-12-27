/*	Multiplikation von stückweisen Polynomen
	(c) 1992 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 2.0
*/


#include <Math/pnom.h>


static void mul_sub (double c, double **a, size_t *deg, size_t n);

static pnom_t *mul_pn;	/* Polynompointer */
static size_t mul_dim;	/* Polynomdimension */
static double mul_m;	/* Multiplikator */


/*	Koeffizienten berechnen
*/

static void mul_sub(double x, double **c, size_t *deg, size_t n)
{
	int i, imax;
	int j0, j, j1;

	imax = deg[0] + deg[1];

	for (i = 0; i <= imax; i++)
	{
		j0 = (i > deg[1] ? i - deg[1] : 0);
		j1 = (i < deg[0] ? i : deg[0]);
		mul_pn->c[mul_dim][i] = 0.;

		for (j = j0; j <= j1; j++)
			mul_pn->c[mul_dim][i] += c[0][j] * c[1][i-j];

		mul_pn->c[mul_dim][i] *= mul_m;
	}

	while (i <= mul_pn->deg)
		mul_pn->c[mul_dim][i++] = 0.;

	mul_pn->x[mul_dim++] = x;
}


/*	Polynome multiplizieren
*/

pnom_t *pnmult(double c, pnom_t *p1, pnom_t *p2)
{
	pnom_t *p[2];
	size_t deg;

	if	(c == 0. || p1 == NULL || p2 == NULL)
	{
		return NULL;
	}

	mul_m = c;
	p[0] = p1;
	p[1] = p2;
	deg = p1->deg + p2->deg;
	mul_dim = pnjoin(p, 2, NULL);
	mul_pn = pnalloc(mul_dim, deg);
	mul_dim = 0;
	(void) pnjoin(p, 2, mul_sub);
	return mul_pn;
}
