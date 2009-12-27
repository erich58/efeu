/*	Klammerausdruck kopieren
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/efmain.h>
#include <EFEU/efio.h>
#include <EFEU/efutil.h>


static int f_get(io_t *io)
{
	return io_egetc(io, NULL);
}

static int f_mget(io_t *io)
{
	return io_mgetc(io, 1);
}

int iocpy_brace(io_t *in, io_t *out, int c, const char *arg, unsigned int flags)
{
	int (*get) (io_t *io);
	int n;

	n = io_nputc(c, out, 1);
	get = (flags ? f_get : f_mget);

	while ((c = get(in)) != EOF)
	{
		if	(listcmp(arg, c))
		{
			n += io_nputc(c, out, 1);
			break;
		}

		switch (c)
		{
		case '(':	n += iocpy_brace(in, out, c, ")", flags); break;
		case '{':	n += iocpy_brace(in, out, c, "}", flags); break;
		case '[':	n += iocpy_brace(in, out, c, "]", flags); break;
		case '"':	n += iocpy_str(in, out, c, "\"", 1); break;
		case '\'':	n += iocpy_str(in, out, c, "'", 1); break;
		default:	n += io_nputc(c, out, 1); break;
		}
	}

	return n;
}
