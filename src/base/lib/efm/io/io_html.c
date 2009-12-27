/*
IO-Struktur für HTML-Ausgabe

$Copyright (C) 2000 Erich Frühstück
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

#define	CTRL_NL		0x1
#define	CTRL_CMD	0x2

typedef struct {
	IO *io;		/* Ausgabestruktur */
	int last;	/* Letztes ausgegebene Zeichen */
	int protect;	/* Schutzmodus */
} FILTER;

static int filter_put (int c, void *ptr)
{
	FILTER *filter = ptr;

	if	(filter->protect)
		return io_putc(c, filter->io);

	switch (c)
	{
	case '<':
		io_puts("&lt;", filter->io);
		break;
	case '>':
		io_puts("&gt;", filter->io);
		break;
	case '&':
		io_puts("&amp;", filter->io);
		break;
	case '"':
		io_puts("&quot;", filter->io);
		break;
	default:
		io_putc(c, filter->io);
		break;
	}

	filter->last = c;
	return c;
}

static int filter_ctrl (void *ptr, int req, va_list list)
{
	FILTER *filter = ptr;
	char *fmt;

	switch (req)
	{
	case IO_CLOSE:
		io_puts("</HTML></BODY>\n", filter->io);
		io_close(filter->io);
		memfree(filter);
		return io_close_stat;
	case CTRL_NL:
		if	(filter->last != '\n')
		{
			io_putc('\n', filter->io);
			filter->last = '\n';
		}
	case IO_PROTECT:

		if	(va_arg(list, int))	filter->protect++;
		else if	(filter->protect)	filter->protect--;

		return filter->protect;
	case CTRL_CMD:
		fmt = va_arg(list, char *);
		io_vxprintf(filter->io, fmt, list);
		return 0;
	default:
		break;
	}

	return io_vctrl(filter->io, req, list);
}


IO *io_html (IO *io)
{
	if	(io)
	{
		FILTER *filter;

		filter = memalloc(sizeof(FILTER));
		filter->io = io;
		filter->last = '\n';
		filter->protect = 0;
		io_puts("<HTML><BODY>\n", io);
		io = io_alloc();
		io->put = filter_put;
		io->ctrl = filter_ctrl;
		io->data = filter;
	}

	return io;
}
