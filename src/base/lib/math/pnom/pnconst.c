/*	Arbeiten mit st�ckweisen Polynomen: Konstante Funktion
	(c) 1991 Erich Fr�hst�ck
	A-1090 Wien, W�hringer Stra�e 64/6

	Version 2.0
*/

#include <Math/pnom.h>


pnom_t *pnconst(double x, double y)
{
	pnom_t *p;

	p = pnalloc(1, 0);
	p->x[0] = x;
	p->c[0][0] = y;
	return p;
}
