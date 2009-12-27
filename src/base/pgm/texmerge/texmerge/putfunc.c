/*	TeX-Ausgabefunktionen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include "texmerge.h"

static char out_buf[64];


/*	Zeichen TeX-kompatibel ausgeben
*/

void tex_putc(int c, Output_t *out)
{
	if	(out->put == NULL || c == EOF)	return;

	switch (c)
	{
	case '%': case '$': case '#': case '_':
	case '&': case '{': case '}':

		out->put('\\', out);
		out->put(c, out);
		break;

	case '^':
	case '~':

		out->put('\\', out);
		out->put(c, out);
		out->put('~', out);
		break;

	case '|': case '<': case '>':

		out->put('$', out);
		out->put(c, out);
		out->put('$', out);
		break;

	case '"':

		sprintf(out_buf, "{\\tt\\symbol{%d}}", c);
		out->puts(out_buf, out);
		break;

	case '\\':

		out->puts("$\\backslash$", out);
		break;

	case '-':

		out->puts("{-}", out);
		break;

	case '*':

		out->puts("$\\ast$", out);
		break;

	case '\t': case '\n':
	case 'Ä': case 'Ö': case 'Ü':
	case 'ä': case 'ö': case 'ü': case 'ß':

		out->put(c, out);
		break;

	default:

		if	(c < ' ' || c >= 127)
		{
			out->puts("$\\bullet$", out);
		}
		else	out->put(c, out);

		break;
	}
}


/*	Zeichen im Stringmodus ausgeben
*/

void str_putc(int c, Output_t *out)
{
	if	(out->put == NULL || c == EOF)	return;

	switch (c)
	{
	case '"': case '\\':

		out->put('\\', out);
		out->put(c, out);
		break;

	case '\t':

		out->put('\\', out);
		out->put('t', out);
		break;

	case '\n':
		
		out->put('\\', out);
		out->put('n', out);
		break;

	case 'Ä': case 'Ö': case 'Ü':
	case 'ä': case 'ö': case 'ü': case 'ß':

		out->put(c, out);
		break;

	default:

		c = (unsigned char) c;

		if	(c < ' ')
		{
			out->put('^', out);
			out->put(c + '@', out);
		}
		else if	(c >= 127)
		{
			sprintf(out_buf, "\\%03o", c);
			out->puts(out_buf, out);
		}
		else	out->put(c, out);

		break;
	}
}
