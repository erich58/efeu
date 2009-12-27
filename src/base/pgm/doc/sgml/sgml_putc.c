/*	Ausgabefilter für SGML-Unixdoc
	(c) 1999 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <SGML.h>
#include <efeudoc.h>
#include <ctype.h>

static int sgml_filter(int c, io_t *io)
{
	switch (c)
	{
	case '<':	io_puts("&lt;", io); break;
	case '>':	io_puts("&gt;", io); break;
	case '&':	io_puts("&amp;", io); break;
	case '$':	io_puts("&dollar;", io); break;
	case '#':	io_puts("&num;", io); break;
	case '%':	io_puts("&percnt;", io); break;
	case '"':	io_puts("&dquot;", io); break;
	case '~':	io_puts("&tilde;>", io); break;
	default:	io_putc(c, io); break;
	}

	return c;
}

void SGML_puts (SGML_t *sgml, const char *str)
{
	if	(str)
	{
		while (*str != 0)
			sgml_filter(*(str++), sgml->out);
	}
}


int SGML_plain (SGML_t *sgml, int c)
{
	sgml_filter(c, sgml->out);
	return c;
}

int SGML_putc (SGML_t *sgml, int c)
{
	if	(isspace(c) && sgml->last == '\n')
		return '\n';

	sgml_filter(c, sgml->out);
	return c;
}


void SGML_newline(SGML_t *sgml, int n)
{
	if	(sgml->last != '\n')
		io_putc('\n', sgml->out);

	sgml->last = '\n';

	while (n-- > 0)
		io_putc('\n', sgml->out);
}

void SGML_rem (SGML_t *sgml, const char *cmd)
{
	io_puts("<!-- ", sgml->out);
	SGML_puts(sgml, cmd);
	io_puts(" -->", sgml->out);

	if	(sgml->last == '\n')
		io_putc('\n', sgml->out);
}

void SGML_sym (SGML_t *sgml, const char *name)
{
	char *sym = DocSym_get(sgml->symtab, name);

	if	(sym)	io_puts(sym, sgml->out);
	else		io_printf(sgml->out, "&%s;", name);
}

