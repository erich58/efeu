/*
Beschreibungstext ausgeben

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
#include <EFEU/mstring.h>
#include <EFEU/parsub.h>
#include <ctype.h>


void PrintInfo (io_t *io, InfoNode_t *base, const char *name)
{
	InfoNode_t *info = GetInfo(base, name);

	if	(info)
	{
		if	(info->load)
			info->load(info);

		if	(info->label)
		{
			io_psub(io, info->label);
			io_putc('\n', io);
		}

		if	(!info->func)
		{
			reg_cpy(1, info->name);
			reg_cpy(2, info->label);
			io_psub(io, info->par);
		}
		else	info->func(io, info);

		if	(info->list)
		{
			int i = info->list->used;
			InfoNode_t **ip = info->list->data;

			io_putc('\n', io);

			while (i > 0)
			{
				InfoName(io, info, *ip);

				if	((*ip)->label)
				{
					io_putc('\t', io);
					io_psub(io, (*ip)->label);
				}

				io_putc('\n', io);
				i--;
				ip++;
			}
		}
	}
}
