/*	Datenvektor runden
	(c) 1995 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/efutil.h>

void roundvec(double *x, size_t dim, double val)
{
	double zp, zn;
	double rdiff;
	double *y;
	int np, nn;
	int ip, in;
	int i;

/*	Abweichungen bestimmen
*/
	zp = zn = 0.;

	for (i = 0; i < dim; i++)
	{
		if	(x[i] >= 0.)
		{
			zp += x[i];
		}
		else	zn -= x[i];
	}

	if	(zp + zn == 0.)	return;

	val = (val - zp + zn) / (zp + zn);

/*	Datenwerte justieren und in Ganzzahlwert und Nachkommastellen zerlegen.
	Rdiff enthällt die Summe der Nachkommastellen und ist immer ganzzahlig.
	Durch Aufrunden positiver Werte, bzw. abrunden negativer Werte muß
	rdiff auf 0 justiert werden. Np und nn enthält die Zahl der nach 4/5
	Rundung auf/abzurundenden Werte.
*/
	y = ALLOC(dim, double);
	rdiff = 0.;
	nn = np = 0;

	for (i = 0; i < dim; i++)
	{
		if	(x[i] >= 0.)
		{
			y[i] = x[i] + val * x[i];
			x[i] = (long) y[i];
			y[i] -= x[i];

			if	(y[i] > 0.5)	np++;
		}
		else
		{
			y[i] = x[i] - val * x[i];
			x[i] = - (long) (-y[i]);
			y[i] -= x[i];

			if	(y[i] < -0.5)	nn++;
		}

		rdiff += y[i];
	}

/*	Positive Werte aufrunden
*/
	while (rdiff > 0.9)
	{
		zp = 0.;
		ip = 0;

		for (i = 0; i < dim; i++)
			if (y[i] > zp) zp = y[ip = i];

		if	(np)	np--;

		x[ip]++;
		y[ip] = 0.;
		rdiff--;
	}

/*	Negative Werte abrunden
*/
	while (rdiff < -0.9)
	{
		zn = 0.;
		in = 0;

		for (i = 0; i < dim; i++)
			if (y[i] < zn) zn = y[in = i];

		if	(nn)	nn--;

		x[in]--;
		y[in] = 0.;
		rdiff++;
	}

/*	Rundungsfehler optimieren
*/
	while (nn && np)
	{
		zp = zn = 0.;
		ip = in = 0;

		for (i = 0; i < dim; i++)
		{
			if	(y[i] > zp)	zp = y[ip = i];
			else if	(y[i] < zn)	zn = y[in = i];
		}

		x[ip]++;
		x[in]--;
		y[ip] = y[in] = 0.;
		nn--;
		np--;
	}

	FREE(y);
}
