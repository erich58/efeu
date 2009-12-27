/*	String formatiert ausgeben
	(c) 1998 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#include "eisdoc.h"


void FormatString(const char *str, io_t *out, const char *fmt)
{
	char varfmt[4];
	int key;

	key = fmt ? fmt[0] : 0;

	switch (key)
	{
	case 's':
		io_printf(out, "%#s", str);
		break;
	case 't':
		tex_puts(str, out);
		break;
	case ' ':
		io_puts(str, out);
		break;
	case 'v':
		out_newline(out);
		io_puts("\\begin{verbatim}\n", out);
		io_puts(str, out);
		out_newline(out);
		io_puts("\\end{verbatim}\n", out);
		break;
	case 'c':
		io_putc(str ? str[0] : 0, out);
		break;
	case 'i':
	case 'd':
	case 'o':
	case 'x':
	case 'X':
		sprintf(varfmt, "%%#%c", key);
		io_printf(out, varfmt, str ? atoi(str) : 0);
		break;
	case 'e':
	case 'E':
	case 'f':
	case 'F':
	case 'g':
	case 'G':
		sprintf(varfmt, "%%#%c", key);
		io_printf(out, varfmt, str ? atof(str) : 0.);
		break;
	default:
		io_puts(str, out);
		break;
	}
}
