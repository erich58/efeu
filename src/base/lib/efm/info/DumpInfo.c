/*
Informationsdatenbank ausgeben

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

#include <EFEU/Info.h>
#include <EFEU/ioctrl.h>
#include <EFEU/parsub.h>
#include <ctype.h>

#define	CTRL_NL		0x1
#define	CTRL_KEY	0x2
#define	CTRL_LABEL	0x3
#define	CTRL_TEXT	0x4

typedef struct {
	IO *io;		/* Ausgabestruktur */
	int (*put) (int c, IO *io);	/* Ausgabefunktion */
	int last;	/* Letztes ausgegebene Zeichen */
} FILTER;

static int filter_put (int c, void *ptr)
{
	FILTER *filter = ptr;

	if	(c == '$' || (c == INFO_KEY && filter->last == '\n'))
		io_putc(c, filter->io);

	filter->put(c, filter->io);
	filter->last = c;
	return c;
}

static int key_put (int c, IO *io)
{
	switch (c)
	{
	case '\n':	c = 'n';
	case '\t':	c = 't';
	case '\f':	c = 'f';
	case '\b':	c = 'b';
	case '<':
	case '|':
	case ':':
	case '\\':	io_putc('\\', io); break;
	}

	return io_putc(c, io);
}

static int label_put (int c, IO *io)
{
	switch (c)
	{
	case '\n':
	case '<':
	case '|':
	case '\\':	io_putc('\\', io); break;
	case '$':	io_putc('$', io); break;
	}

	return io_putc(c, io);
}

static int filter_ctrl (void *ptr, int req, va_list list)
{
	FILTER *filter = ptr;

	switch (req)
	{
	case IO_CLOSE:
		return 0;
	case CTRL_NL:
		if	(filter->last != '\n')
		{
			io_putc('\n', filter->io);
			filter->last = '\n';
		}

		return 0;
	case CTRL_KEY:
		io_putc(INFO_KEY, filter->io);
		filter->put = key_put;
		return 0;
	case CTRL_LABEL:
		io_putc(':', filter->io);
		filter->put = label_put;
		return 0;
	case CTRL_TEXT:
		io_putc('\n', filter->io);
		filter->last = '\n';
		filter->put = io_putc;
		return 0;
	default:
		break;
	}

	return io_vctrl(filter->io, req, list);
}

static void dump_info (IO *io, InfoNode *base, InfoNode *info)
{
	if	(info->load)
		info->load(info);

	io_ctrl(io, CTRL_NL);
	io_ctrl(io, CTRL_KEY);
	InfoName(io, base, info);
	io_ctrl(io, CTRL_LABEL);

	if	(info->label)
	{
		io_putc(' ', io);
		io_psubarg(io, info->label, "ns", info->name);
	}

	io_ctrl(io, CTRL_TEXT);

	if	(!info->func)
	{
		io_psubarg(io, info->par, "nss", info->name, info->label);
	}
	else	info->func(io, info);

	io_ctrl(io, CTRL_NL);

	if	(info->list)
	{
		int i = info->list->used;
		InfoNode **ip = info->list->data;

		for (; i > 0; i--, ip++)
			dump_info(io, base, *ip);
	}
}


void DumpInfo (IO *io, InfoNode *base, const char *name)
{
	InfoNode *info = GetInfo(base, name);

	if	(info)
	{
		FILTER filter;

		filter.io = io;
		filter.put = io_putc;
		filter.last = '\n';
		io = io_alloc();
		io->put = filter_put;
		io->ctrl = filter_ctrl;
		io->data = &filter;
		dump_info(io, name ? info : base, info);
		io_close(io);
	}
}
