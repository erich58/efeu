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

#define	INFO_NAME	"filter"

#define	INFO_HEAD	\
	":*:data base filter" \
	":de:Datenbankfilter"

static NameKeyTab FilterTab = NKT_DATA("EDBFilterTab", 60, NULL);

void AddEDBFilter (EDBFilter *def)
{
	nkt_insert(&FilterTab, def->name, def);
}

EDBFilter *GetEDBFilter (const char *name)
{
	SetupEDBFilter();
	return nkt_fetch(&FilterTab, name, NULL);
}

void PrintEDBFilter (IO *out, EDBFilter *fdef, int flag)
{
	char *fmt;
	IO *desc;
	int c;

	fmt = GetFormat(fdef->desc);
	desc = langfilter(io_cstr(fmt), NULL);

	io_puts(fdef->name, out);

	if	(flag)
	{
		io_puts(fdef->syntax, out);
		io_puts("\n\t", out);
	}
	else	io_putc('\t', out);

	while  ((c = io_getc(desc)) != EOF)
	{
		if	(c == '$')
		{
			switch (c = io_getc(desc))
			{
			case '1':	io_puts(fdef->name, out); break;
			case '2':	io_puts(fdef->par, out); break;
			default:	io_putc(c, out); break;
			}
		}
		else if	(c == '\n')
		{
			if	(flag)
				io_puts("\n\t", out);
			else	break;
		}
		else	io_putc(c, out);
	}

	io_close(desc);
	io_putc('\n', out);
}

static int list_flag = 0;

static int do_list (const char *name, void *data, void *par)
{
	PrintEDBFilter(par, data, list_flag);
	return 0;
}

void ListEDBFilter (IO *out, int flag)
{
	SetupEDBFilter();
	list_flag = flag;
	nkt_walk(&FilterTab, do_list, out);
}

static int add_info (const char *name, void *data, void *par)
{
	EDBFilter *fdef = data;
	InfoNode *info = par;
	AddInfo(info, fdef->name, GetFormat(fdef->desc), NULL, NULL);
	return 0;
}

static void load_info (InfoNode *info)
{
	nkt_walk(&FilterTab, add_info, info);
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
