/*
Ausgabefilter für HTML-Unixdoc

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

#include <HTML.h>
#include <efeudoc.h>
#include <ctype.h>

static int html_filter(int c, io_t *io)
{
	switch (c)
	{
	case '<':	io_puts("&lt;", io); break;
	case '>':	io_puts("&gt;", io); break;
	case '&':	io_puts("&amp;", io); break;
	case '%':	io_puts("&percnt;", io); break;
	case '"':	io_puts("&quot;", io); break;
	default:	io_putc(c, io); break;
	}

	return c;
}

void HTML_puts (HTML_t *html, const char *str)
{
	if	(str)
	{
		while (*str != 0)
			html_filter(*(str++), html->out);
	}
}


int HTML_plain (HTML_t *html, int c)
{
	html_filter(c, html->out);
	return c;
}

int HTML_putc (HTML_t *html, int c)
{
	if	(isspace(c) && html->last == '\n')
		return '\n';

	html_filter(c, html->out);
	return c;
}

void HTML_newline(HTML_t *html, int n)
{
	if	(html->last != '\n')
		io_putc('\n', html->out);

	html->last = '\n';

	while (n-- > 0)
		io_putc('\n', html->out);
}

void HTML_rem (HTML_t *html, const char *cmd)
{
	io_puts("<!-- ", html->out);
	HTML_puts(html, cmd);
	io_puts(" -->", html->out);

	if	(html->last == '\n')
		io_putc('\n', html->out);
}

void HTML_sym (HTML_t *html, const char *name)
{
	char *sym = DocSym_get(html->symtab, name);

	if	(sym)	io_puts(sym, html->out);
	else		io_printf(html->out, "&%s;", name);
}

