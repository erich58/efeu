/*	Beschreibungstexte fuer TeX Aufbereiten
	(c) 1993 Erich Fruehstueck
	A-1090 Wien, Waehringer Strasse 64/6
*/

#include "oestz.h"
#include <ctype.h>

static int TeX_konv (io_t *io, const char *p);
static int latin1_konv (io_t *io, const char *p);

static int TeX_konv(io_t *io, const char *p)
{
	switch (p[0])
	{
	case '[':	return io_puts("\"A", io);
	case '\\':	return io_puts("\"O", io);
	case ']':	return io_puts("\"U", io);
	case '{':	return io_puts("\"a", io);
	case '|':	return io_puts("\"o", io);
	case '}':	return io_puts("\"u", io);
	case '~':	return io_puts("\"s", io);
	case '^':	return io_puts("$\\hat{~}$", io);
	case '_':	return io_puts("\\_", io);
	case '%':	return io_puts("\\%", io);
	case '.':	return io_puts(isupper(p[1]) ? ". " : ".", io);
	case '-':	return io_puts(p[1] != 0 ? "--" : "-", io);
	default:	return io_nputc(*p, io, 1);
	}
}

static int latin1_konv(io_t *io, const char *p)
{
	switch (p[0])
	{
	case '[':	return io_putc('Ä', io);
	case '\\':	return io_putc('Ö', io);
	case ']':	return io_putc('Ü', io);
	case '{':	return io_putc('ä', io);
	case '|':	return io_putc('ö', io);
	case '}':	return io_putc('ü', io);
	case '~':	return io_putc('ß', io);
	case '.':	return io_puts(isupper(p[1]) ? ". " : ".", io);
	default:	return io_nputc(*p, io, 1);
	}
}


int oestz_TeXput(io_t *io, const char *p)
{
	int n;

	for (n = 0; *p != 0; p++)
	{
		if	(TeXmode)
		{ 
			n += TeX_konv(io, p);
		}
		else	n += latin1_konv(io, p);

		/*
		switch (*p)
		{
		case '[':	n += io_puts("\"A", io); break;
		case '\\':	n += io_puts("\"O", io); break;
		case ']':	n += io_puts("\"U", io); break;
		case '{':	n += io_puts("\"a", io); break;
		case '|':	n += io_puts("\"o", io); break;
		case '}':	n += io_puts("\"u", io); break;
		case '~':	n += io_puts("\"s", io); break;
		case '^':	n += io_puts("$\\hat{~}$", io); break;
		case '_':	n += io_puts("\\_", io); break;
		case '%':	n += io_puts("\\%", io); break;
		case '.':	n += io_puts(isupper(p[1]) ? ". " :
					".", io); break;
		case '-':	n += io_puts(p[1] != 0 ? "--" :
					"-", io); break;
		default:	n += io_nputc(*p, io, 1); break;
		}
		*/
	}

	return n;
}


char *oestz_TeXstr(const char *p)
{
	strbuf_t *sb;
	io_t *tmp;

	sb = new_strbuf(0);
	tmp = io_strbuf(sb);
	oestz_TeXput(tmp, p);
	io_close(tmp);
	return sb2str(sb);
}
