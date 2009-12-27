/*	String mit Begrenzern und ohne Transformation umkopieren
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/efmain.h>
#include <EFEU/efio.h>

int iocpy_str(io_t *in, io_t *out, int c, const char *arg, unsigned int flags)
{
	int n, escape;

	if	(flags & 1)
	{
		io_putc(c, out);
		n = 1;
	}
	else	n = 0;

	escape = 0;
	io_protect(in, 1);

	while ((c = io_mgetc(in, 0)) != EOF)
	{
		if	(escape)
		{
			escape = 0;
		}
		else if	(listcmp(arg, c))
		{
			if	(flags & 2)
			{
				int d;

				d = io_mgetc(in, 0);

				if	(d == c)
				{
					io_putc(c, out);
					n++;
					continue;
				}
				else	io_ungetc(d, in);
			}

			if	(flags & 1)
			{
				io_putc(c, out);
				n++;
			}

			io_protect(in, 0);
			return n;
		}
		else	escape = (c == '\\');

		io_putc(c, out);
		n++;
	}

	io_protect(in, 0);
	return n;
}
