/*
Datenstruktur

$Copyright (C) 2000 Erich Frühstück
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

#include "src2doc.h"
#include <EFEU/strbuf.h>
#include <EFEU/efio.h>

static char *SecHead[] = {
	"\\Description\n",
	"\\Examples\n",
	"\\SeeAlso\n",
	"\\Diagnostics\n",
	"\\Notes\n",
	"\\Warnings\n",
	"\\Errors\n",
	"\\Copyright\nCopyright\n",
};

void SrcData_init (SrcData_t *data, io_t *ein, io_t *aus)
{
	int i;

	memset(data, 0, sizeof(SrcData_t));
	data->buf = new_strbuf(0);
	data->synopsis = new_strbuf(0);

	for (i = 0; i < BUF_DIM; i++)
		data->tab[i] = new_strbuf(0);

	data->ein = ein;
	data->aus = aus;
}


void copy_protect (const char *str, io_t *io)
{
	if	(!str || !io)	return;

	for (; *str; str++)
	{
		switch (*str)
		{
		case '<':
		case '>':
		case '\\':
		case '|':
		case '[':
		case '*':
		case '#':
		case '$':
		case '&':
			io_putc('\\', io);
			break;
		case '/':
			if	(str[1] == '/' || str[1] == '*')
				io_putc('\\', io);
			break;
		default:
			break;
		}

		io_putc(*str, io);
	}
}

void SrcData_clean (SrcData_t *data)
{
	int i;

	io_printf(data->aus, "\\Name\n%s -- ", data->var[VAR_NAME]);
	io_printf(data->aus, "%s\n", data->var[VAR_TITLE]);

	if	(data->var[VAR_HEAD] || sb_getpos(data->synopsis))
	{
		io_puts("\\Synopsis\n\\code\n", data->aus);

		if	(data->var[VAR_HEAD])
		{
			io_puts("\\#include ", data->aus);
			copy_protect(data->var[VAR_HEAD], data->aus);
			io_puts("\n\n", data->aus);
		}

		if	(sb_getpos(data->synopsis))
		{
			sb_putc(0, data->synopsis);
			io_puts((char *) data->synopsis->data, data->aus);
		}

		io_puts("\\end\n", data->aus);
	}

	for (i = 0; i < BUF_DIM; i++)
	{
		if	(sb_getpos(data->tab[i]))
		{
			io_puts(SecHead[i], data->aus);
			sb_putc(0, data->tab[i]);
			io_puts((char *) data->tab[i]->data, data->aus);
		}

		del_strbuf(data->tab[i]);
	}

	del_strbuf(data->buf);
	del_strbuf(data->synopsis);

	if	(data->var[VAR_COPYRIGHT])
		io_printf(data->aus, "\\Copyright\nCopyright %s\n",
			data->var[VAR_COPYRIGHT]);

	for (i = 0; i < VAR_DIM; i++)
		memfree(data->var[i]);
}
