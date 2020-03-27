/*
Ausgabezeilen einrücken

$Copyright (C) 1998 Erich Frühstück
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

#include <EFEU/io.h>
#include <EFEU/ioctrl.h>
#include <EFEU/mstring.h>


typedef struct {
	IO *io;		/* Ausgabestruktur */
	int c;		/* Einrückungszeichen */
	int n;		/* Multiplikator */
	int depth;	/* Einrücktiefe */
	int newline;	/* Flag für neue Zeile */
} INDENT;

static int indent_put (int c, IO *io)
{
	INDENT *indent = io->data;

	if	(indent->newline && c != '\n')
	{
		io_nputc(indent->c, indent->io, indent->n * indent->depth);
		indent->newline = 0;
	}

	indent->newline = (c == '\n');
	return io_putc(c, indent->io);
}

static int indent_ctrl (IO *io, int req, va_list list)
{
	INDENT *indent = io->data;
	size_t stat;

	switch (req)
	{
	case IO_CLOSE:

		stat = io_close(indent->io);
		memfree(indent);
		break;

	case IO_SUBMODE:
		
		if	(va_arg(list, int))	indent->depth++;
		else if	(indent->depth > 0)	indent->depth--;

		return indent->depth;

	case IO_REWIND:

		if	(io_rewind(indent->io) == EOF)
			return EOF;

		indent->depth = 0;
		indent->newline = 1;
		stat = 0;
		break;

	case IO_IDENT:

		*va_arg(list, char **) = io_xident(indent->io, "indent(%*)");
		return 0;

	default:

		stat = io_vctrl(indent->io, req, list);
		break;
	}

	return stat;
}


IO *io_indent (IO *io, int c, int n)
{
	INDENT *indent = memalloc(sizeof(INDENT));
	indent->io = io;
	indent->c = c;
	indent->n = n;
	indent->depth = 0;
	indent->newline = 1;
	io = io_alloc();
	io->put = indent_put;
	io->ctrl = indent_ctrl;
	io->data = indent;
	return io;
}
