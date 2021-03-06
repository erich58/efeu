/*
Ausgabefilter für mroff

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

#include <mroff.h>
#include <efeudoc.h>
#include <ctype.h>

static int mr_ignore (ManRoff *mr, int c)
{
	int flag = (c == '\n' && mr->nlignore);
	mr->nlignore = flag;
	return flag;
}

static int mr_filter (int c, IO *io)
{
	if	(c == '\\')
		io_putc(c, io);

	io_putc(c, io);
	return c;
}


int mroff_protect (void *drv, int c)
{
	ManRoff *mr = drv;

	if	(isspace(c) && mr->last != '\n')
	{
		mr->space = 1;
		return ' ';
	}

	if	(mr->space)
	{
		io_putc(' ', mr->out);
		mr->space = 0;
	}

	switch (c)
	{
	case '\\':	io_puts("\\\\\\\\", mr->out); break;
	default:	io_putc(c, mr->out); break;
	}

	return c;
}

void mroff_cbeg (ManRoff *mr, const char *pfx)
{
	if	(pfx)
	{
		io_puts(pfx, mr->out);
		io_putc(' ', mr->out);
	}

	mr->last = '\n';
	mr->space = 0;
	mr->copy = 1;
	mr->put = mroff_protect;
}

void mroff_cend (ManRoff *mr, int flag)
{
	mr->put = mroff_putc;
	mr->copy = 0;

	if	(flag)
	{
		io_putc('\n', mr->out);
		mr->last = '\n';
		mr->nlignore = 1;
	}
}

void mroff_string (ManRoff *mr, const char *str)
{
	if	(str == NULL)	return;

	for (; *str != 0; str++)
		mr->put(mr, *str);
}

int mroff_plain (void *drv, int c)
{
	ManRoff *mr = drv;

	if	(mr_ignore(mr, c))
		return '\n';

	if	(mr->last == '\n')
	{
		switch (c)
		{
		case ' ':
			io_puts("\\ ", mr->out);
			return '\n';
		case '.':
		case '\'':
			io_puts("\\&", mr->out);
			break;
		default:
			break;
		}
	}

	return mr_filter(c, mr->out);
}

int mroff_putc (void *drv, int c)
{
	ManRoff *mr = drv;

	if	(mr_ignore(mr, c))
		return '\n';

	if	(mr->last == '\n')
	{
		switch (c)
		{
		case ' ':
			return '\n';
		case '.':
		case '\'':
			io_puts("\\&", mr->out);
			break;
		default:
			break;
		}
	}

	return mr_filter(c, mr->out);
}

void mroff_newline (ManRoff *mr)
{
	if	(mr->last != '\n')
		io_putc('\n', mr->out);

	mr->nlignore = 0;
	mr->last = '\n';
}

void mroff_rem (void *drv, const char *cmd)
{
	ManRoff *mr = drv;

	if	(mr->last == '\n')
		io_putc('.', mr->out);

	io_puts("\\\" ", mr->out);

	if	(cmd)
	{
		for (; *cmd != 0; cmd++)
		{
			io_putc(*cmd, mr->out);

			if	(*cmd == '\n')	io_puts(".\\\" ", mr->out);
		}
	}

	io_putc('\n', mr->out);
	mr->last = '\n';
	mr->nlignore = 1;
}

void mroff_cmdline (ManRoff *mr, const char *line)
{
	mroff_newline(mr);
	mroff_cmdend(mr, line);
}

void mroff_cmdend (ManRoff *mr, const char *line)
{
	io_puts(line, mr->out);
	io_putc('\n', mr->out);
	mr->last = '\n';
	mr->nlignore = 1;
}

