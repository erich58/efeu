/*	Zeichen überlesen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/io.h>
#include <EFEU/iocpy.h>
#include <EFEU/patcmp.h>


static void c_skip (io_t *io);

int io_eat(io_t *io, const char *wmark)
{
	int c;

	while ((c = io_mgetc(io, 1)) != EOF)
	{
		if	(c == '/' && iocpy_cskip(io, NULL, c,
				NULL, 1) != EOF)
		{
			continue;
		}
		else if	(!listcmp(wmark, c))
		{
			io_ungetc(c, io);
			break;
		}
	}

	return c;
}


/*	Lesen bis Ende eines C - Style Kommentars
*/

static void c_skip(io_t *io)
{
	int c;

	while ((c = io_getc(io)) != EOF)
	{
		while (c == '*')
		{
			c = io_getc(io);

			if	(c == '/')
			{
				return;
			}
		}
	}
}


/*	C, C++ Kommentare überlesen
*/

int iocpy_cskip(io_t *in, io_t *out, int c, const char *arg, unsigned int flags)
{
	int x;

	x = io_getc(in);

	if	(x == '*')
	{
		io_protect(in, 1);
		c_skip(in);
		io_protect(in, 0);
		return 0;
	}
	else if	(x == '/' && flags)
	{
		return iocpy_skip(in, out, x, "\n", 1);
	}
	else
	{
		io_ungetc(x, in);
		return EOF;
	}
}


/*	Zeichen überlesen
*/

int iocpy_skip(io_t *in, io_t *out, int c, const char *arg, unsigned int flags)
{
	int x;

	while ((x = io_getc(in)) != EOF)
		if (listcmp(arg, x)) break;

/*	Bei Flag wird Abschlusszeichen zurückgeschrieben
*/
	if	(flags)	io_ungetc(x, in);

	return 0;
}
