/*	EDB-Filterdefinitionen

$Copyright (C) 2004 Erich Frühstück
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

#include <EFEU/EDB.h>
#include <EFEU/nkt.h>
#include <EFEU/Resource.h>
#include <EFEU/EDBFilter.h>

#define	INFO_NAME	"filter"

#define	INFO_HEAD	\
	":*:data base filter" \
	":de:Datenbankfilter"

EfiPar EfiPar_EDBFilter = { "EDBFilter", "EDB data filter" };

void EDBFilter_info (IO *out, const void *data)
{
	const EDBFilter *fdef;
	char *fmt;
	IO *desc;
	int c;

	fdef = data;
	fmt = GetFormat(fdef->epc_label);
	desc = langfilter(io_cstr(fmt), NULL);

	io_puts(fdef->epc_name, out);
	io_puts(fdef->syntax, out);
	io_puts("\n\t", out);

	while  ((c = io_getc(desc)) != EOF)
	{
		if	(c == '$')
		{
			switch (c = io_getc(desc))
			{
			case '1':	io_puts(fdef->epc_name, out); break;
			case '2':	io_puts(fdef->par, out); break;
			default:	io_putc(c, out); break;
			}
		}
		else if	(c == '\n')
		{
			io_puts("\n\t", out);
		}
		else	io_putc(c, out);
	}

	io_close(desc);
	io_putc('\n', out);
}

static void print_info (IO *io, InfoNode *info)
{
	EDBFilter_info(io, info->par);
}

static void add_info (EfiParClass *entry, void *data)
{
	AddInfo(data, entry->epc_name, GetFormat(entry->epc_label),
		print_info, entry);
}

static void load_info (InfoNode *info)
{
	EfiParWalk(NULL, &EfiPar_EDBFilter, add_info, info);
}

void EDBFilterInfo (InfoNode *info)
{
	static int setup_done = 0;

	if	(setup_done)	return;

	setup_done = 1;
	SetupEDBFilter();
	info = AddInfo(info, INFO_NAME, INFO_HEAD, NULL, NULL);
	info->setup = load_info;
}
