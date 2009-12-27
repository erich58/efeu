/*
Auflisten von Zählerdefinitionen

$Copyright (C) 1995 Erich Frühstück
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

#include <EFEU/mdmat.h>
#include <EFEU/mdcount.h>

#define	LBL_COUNT \
	":*:counter types" \
	":de:Zählerdefinitionen"

void md_showcnt(IO *io, MdCountPar *tab)
{
	MdCount **cnt;
	size_t n;

	for (cnt = tab->vtab.data, n = tab->vtab.used; n-- > 0; cnt++)
	{
		io_puts((*cnt)->name, io);
		io_putc('\t', io);
		/*
		io_puts((*cnt)->type, io);
		io_putc('\t', io);
		*/
		io_puts((*cnt)->desc, io);
		io_putc('\n', io);
	}
}


static void print_count(IO *io, InfoNode *info)
{
	MdCount *cnt = info->par;
	/*
	ListType(io, mdtype(cnt->type));
	*/
	io_puts(cnt->type, io);
	io_putc('\n', io);
}

void MdCountInfo (InfoNode *info, MdCountPar *tab)
{
	InfoNode *root;
	MdCount **cnt;
	size_t n;

	root = AddInfo(info, "count", LBL_COUNT, NULL, NULL);

	for (cnt = tab->vtab.data, n = tab->vtab.used; n-- > 0; cnt++)
		AddInfo(root, (*cnt)->name, (*cnt)->desc, print_count, *cnt);
}
