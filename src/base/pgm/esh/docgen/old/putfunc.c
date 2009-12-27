/*	TeX-Ausgabefunktionen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include "eisdoc.h"

/*	Zeichen TeX-kompatibel ausgeben
*/

void tex_putc(int c, io_t *out)
{
	if	(c == EOF || out == NULL || out->put == NULL)
		return;

	switch (c)
	{
	case '%': case '$': case '#': case '_':
	case '&': case '{': case '}':

		out->put('\\', out->data);
		out->put(c, out->data);
		break;

	case '^':
	case '~':

		out->put('\\', out->data);
		out->put(c, out->data);
		out->put('~', out->data);
		break;

	case '|': case '<': case '>':

		out->put('$', out->data);
		out->put(c, out->data);
		out->put('$', out->data);
		break;

	case '"':

		io_printf(out, "{\\tt\\symbol{%d}}", c);
		break;

	case '\\':

		io_puts("$\\backslash$", out);
		break;

	case '-':

		io_puts("{-}", out);
		break;

	case '*':

		io_puts("$\\ast$", out);
		break;

	case '\t': case '\n':
	case 'Ä': case 'Ö': case 'Ü':
	case 'ä': case 'ö': case 'ü': case 'ß':

		out->put(c, out->data);
		break;

	default:

		if	(c < ' ' || c >= 127)
		{
			io_puts("$\\bullet$", out);
		}
		else	out->put(c, out->data);

		break;
	}
}

void tex_puts(const char *str, io_t *io)
{
	if	(io && io->put && str)
	{
		register const uchar_t *p = (const uchar_t *) str;

		while (*p != 0)
			tex_putc(*p++, io->data);
	}
}
