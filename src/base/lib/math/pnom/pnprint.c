/*	Ausgabe einer Polynomstruktur
	(c) 1991 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 2.0
*/

#include <Math/pnom.h>

int pnprint(io_t *io, pnom_t *p, const char *fmt)
{
	int n, i, j;

	if	(p == NULL)
	{
		return io_puts("0 0\n", io);
	}

	n = io_printf(io, "%d %d", p->dim, p->deg);

	for (i = 0; i < p->dim; ++i)
	{
		n += io_nputc('\n', io, 1);
		n += io_printf(io, fmt, p->x[i]);

		for (j = 0; j <= p->deg; ++j)
		{
			n += io_nputc(' ', io, 1);
			n += io_printf(io, fmt, p->c[i][j]);
		}
	}

	n += io_nputc('\n', io, 1);
	return n;
}
