/*
Ausgabefilter für LaTeX

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

#include <LaTeX.h>
#include <efeudoc.h>
#include <ctype.h>

#define	USE_TEXTCOMP	0

int LaTeX_indexmode = 0;

static void stex_filter (int c, IO *io)
{
	if	(c == EOF)	return;

	c = (unsigned char) c;

	switch (c)
	{
	case '!': case '@':

		if	(LaTeX_indexmode)	io_putc('"', io);

		io_putc(c, io);
		break;

	case '%': case '$': case '#': case '_':
	case '&': case '{': case '}':

		io_putc('\\', io);
		io_putc(c, io);
		break;

	case '^': case '~': case '\\':
	case '|': case '<': case '>': case '"':

		io_printf(io, "\\texttt{\\char%d}", c);
		break;

	case '-': 
		io_puts("{-}", io);
		break;

	case '\t': case '\n':
		io_putc(c, io);
		break;

#if	USE_TEXTCOMP
	case 0xa1:	io_puts("!", io); break;
	case 0xa7:	io_puts("{\\textsection}", io); break;
	case 0xa9:	io_puts("{\\textcopyright}", io); break;
	case 0xaa:	io_puts("{\\tt\\textordfeminine}", io); break;
	case 0xac:	io_puts("{\\textlnot}", io); break;
	case 0xad:	io_puts("{-}", io); break;
	case 0xaf:	io_puts("{\\textasciimacron}", io); break;
	case 0xb1:	io_puts("{\\textpm}", io); break;
	case 0xb2:	io_puts("{\\texttwosuperior}", io); break;
	case 0xb3:	io_puts("{\\textthreesuperior}", io); break;
	case 0xb5:	io_puts("{\\textmu}", io); break;
	case 0xb6:	io_puts("{\\textparagraph}", io); break;
	case 0xb9:	io_puts("{\\textonesuperior}", io); break;
	case 0xba:	io_puts("{\\tt\\textordmasculine}", io); break;
	case 0xbf:	io_puts("?", io); break;
	case 0xd0:	io_putc('D', io); break;
	case 0xd7:	io_puts("{\\texttimes}", io); break;
	case 0xde:	io_puts("T", io); break;
	case 0xf0:	io_putc('d', io); break;
	case 0xf7:	io_puts("{\\textdiv}", io); break;
	case 0xfe:	io_puts("t", io); break;
#endif

	default:

		if	(c < ' ' || (c >= 127 && c < 160))
		{
			io_puts("$\\bullet$", io);
		}
		else	io_putc(c, io);

		break;
	}
}


int LaTeX_plain (void *drv, int c)
{
	LaTeX *stex = drv;

	if	(c == ' ' && stex->last == '\n')
		io_puts("\\strut~", stex->out);
	else	stex_filter(c, stex->out);

	return c;
}

int LaTeX_putc (void *drv, int c)
{
	LaTeX *stex = drv;

	if	(stex->ignorespace && isspace(c))
		return c;

	stex->ignorespace = 0;
	stex_filter(c, stex->out);
	return c;
}


int LaTeX_xputc (void *drv, int c)
{
	LaTeX *stex = drv;

	if	(isspace(c))
	{
		if	(stex->ignorespace)	return 0;

		stex->ignorespace = 1;
		stex->space = c;
		return c;
	}

	if	(stex->space)
		io_putc(stex->space, stex->out);

	stex->ignorespace = 0;
	stex->space = 0;
	stex_filter(c, stex->out);
	return c;
}

void LaTeX_puts (LaTeX *stex, const char *str)
{
	if	(str)
	{
		while (*str != 0)
			stex_filter(*(str++), stex->out);
	}
}

void LaTeX_newline (LaTeX *stex)
{
	if	(stex->last != '\n')
		io_putc('\n', stex->out);

	stex->last = '\n';
}

void LaTeX_rem (void *drv, const char *cmd)
{
	LaTeX *stex = drv;
	io_puts("% ", stex->out);

	if	(cmd)
	{
		for (; *cmd != 0; cmd++)
		{
			io_putc(*cmd, stex->out);

			if	(*cmd == '\n')	io_puts("% ", stex->out);
		}
	}

	io_putc('\n', stex->out);
}
