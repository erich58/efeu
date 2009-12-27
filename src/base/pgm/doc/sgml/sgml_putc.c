/*
Ausgabefilter für SGML-Unixdoc

$Copyright (C) 1999 Erich Frühstück
This file is part of EFEU.

EFEU is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

EFEU is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public
License along with EFEU; see the file COPYING.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
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
	case '~':	io_puts("&tilde;", io); break;
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

