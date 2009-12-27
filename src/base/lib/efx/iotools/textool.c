/*
Hilfsprogramme f�r TeX

$Copyright (C) 1994 Erich Fr�hst�ck
This file is part of EFEU.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; see the file COPYING.Library.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <EFEU/efio.h>


/*	String ausgeben
*/

int TeXputs(const char *str, io_t *io)
{
	int size;

	if	(str == NULL)	return 0;

	for (size = 0; *str != 0; str++)
		size += TeXputc(*str, io);

	return size;
}

/*	String unter Anf�hrung ausgeben
*/

int TeXquote(const char *str, io_t *io)
{
	int size;

	size = io_puts("\"`", io);
	size += TeXputs(str, io);
	size += io_puts("\"'", io);
	return size;
}


/*	Zeichen ausgeben
*/

int TeXputc(int c, io_t *io)
{
	if	(c == EOF)	return 0;

	io = io_count(io_refer(io));

	switch ((char) c)
	{
	case EOF:
	
		break;

	case '%': case '$': case '#': case '_':
	case '&': case '{': case '}':

		io_putc('\\', io);
		io_putc(c, io);
		break;

	case '^':
	case '~':

		io_putc('\\', io);
		io_putc(c, io);
		io_putc('~', io);
		/*
		io_printf(io, "{\\tt\\symbol{%d}}", c);
		*/
		break;

	case '|': case '<': case '>':

		io_putc('$', io);
		io_putc(c, io);
		io_putc('$', io);
		break;

	case '"':

		io_printf(io, "{\\tt\\symbol{%d}}", c);
		break;

	case '\\':

		io_puts("$\\backslash$", io);
		break;

	case '-':

		io_puts("{-}", io);
		break;

	case '*':

		io_puts("$\\ast$", io);
		break;

	case '\t': case '\n':
	case '�': case '�': case '�':
	case '�': case '�': case '�': case '�':

		io_putc(c, io);
		break;

	default:

		if	(c < ' ' || c >= 127)
		{
			io_puts("$\\bullet$", io);
		}
		else	io_putc(c, io);

		break;
	}

	return io_close(io);
}
