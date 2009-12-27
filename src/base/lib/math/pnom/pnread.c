/*	Lesen von Polynomdaten aus einem File
	(c) 1991 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 2.0
*/

#include <EFEU/efmain.h>
#include <Math/pnom.h>


pnom_t *pnread(FILE *file)
{
	pnom_t *p;
	int n, k;
	int i, j;

	if	(fscanf(file, "%d%d", &n, &k) != 2)
	{
		liberror(PNOM, 11);
	}

	p = pnalloc((size_t) n, (size_t) k);

	for (i = 0; i < n; ++i)
	{
		if	(fscanf(file, "%lf", p->x + i) != 1)
		{
			liberror(PNOM, 11);
		}

		for (j = 0; j <= k; ++j)
		{
			if	(fscanf(file, "%lf", p->c[i] + j) != 1)
			{
				liberror(PNOM, 11);
			}
		}
	}

	return p;
}
