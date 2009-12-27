/*	Kopierhilfsfunktionen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/efmain.h>
#include <EFEU/efio.h>


/*	Einsetzen eines Wertes
*/

int iocpy_repl(io_t *in, io_t *out, int c, const char *arg, unsigned int flags)
{
	return io_puts(arg, out);
}


/*	Zeichen Markieren
*/

int iocpy_mark(io_t *in, io_t *out, int c, const char *arg, unsigned int flags)
{
	int n;

	n = io_puts(arg, out);
	io_putc(c, out);
	return n + 1;
}


/*	Namen kopieren
*/

int iocpy_name(io_t *in, io_t *out, int c, const char *arg, unsigned int flags)
{
	int n;

	io_putc(c, out);

	for (n = 1; (c = io_mgetc(in, 1)) != EOF; n++)
	{
		if	(!listcmp(arg, c))
		{
			io_ungetc(c, in);
			break;
		}

		io_putc(c, out);
	}

	return n;
}


/*	Escape - Zeichen
*/

int iocpy_esc(io_t *in, io_t *out, int c, const char *arg, unsigned int flags)
{
	int x;
	int n;

	x = io_getc(in);
	n = 0;

	if	(!listcmp(arg, x))
	{
		io_ungetc(x, in);
		return EOF;
	}

	if	(flags)	io_putc(c, out);
	
	io_putc(x, out);
	return flags ? 2 : 1;
}
