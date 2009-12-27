/*	Normalverteilte Zufallszahlen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/


#include <EFEU/efutil.h>
#include <EFEU/rand48.h>
#include <math.h>

#ifndef	M_PI
#define M_PI        3.14159265358979323846264338327950288
#endif


double NormalRand48(Rand48_t *rand, int dim)
{
	double x;
	int i, n;

	if	(dim <= 0)
	{
		x = sqrt(-2. * log(Rand48(rand)));
		x *= cos(M_PI * 2. * Rand48(rand));
		return x;
	}

	n = 3 * dim * dim;
	x = 0.;

	for (i = 0; i < n; i++)
		x += Rand48(rand);

	return (2. * x - (double) n) / (double) dim;
}
