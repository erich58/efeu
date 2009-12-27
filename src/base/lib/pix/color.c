/*	Farbwerte
	(c) 1995 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/oldpixmap.h>

int ColorValue(double x)
{
	if	(x <= 0.)	return 0;
	else if	(x >= 1.)	return 255;
	else			return (int) (255. * x + 0.5);
}

Color_t SetColor(double red, double green, double blue)
{
	Color_t color;

	color.red = ColorValue(red);
	color.green = ColorValue(green);
	color.blue = ColorValue(blue);
	return color;
}


/*	Die am nähesten liegende Farbe bestimmen
*/

static int dist (int a, int b)
{
	if	(a < b)	return b - a;
	else if	(a > b)	return a - b;
	else		return 0;
}


int GetColor(Color_t color, Color_t *tab, size_t dim)
{
	int i, n, d, last;

	d = 255 + 255 + 255;
	last = 0;

	for (i = 0; i < dim; i++)
	{
		n = dist(color.red, tab[i].red) + dist(color.green, tab[i].green)
			+ dist(color.blue, tab[i].blue);

		if	(n < d)
		{
			last = i;
			d = n;
		}
	}

	return last;
}
