/*
Typenbaum ausgeben

$Copyright (C) 2001 Erich Frühstück
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

#include <EFEU/object.h>
#include <ctype.h>

typedef struct {
	io_t *io;
	char *pfx;
	Type_t **ptr;
	size_t dim;
} INFO;

extern xtab_t TypeTab;

static void subfunc (io_t *io, const char *pfx, int leaf, const Type_t *type);

static void next (INFO *info, int leaf, const Type_t *base)
{
	Type_t *type;

	while (info->dim-- > 0)
	{
		type = *(info->ptr++);

		if	(type->base == base)
		{
			next(info, 0, base);
			subfunc(info->io, info->pfx, leaf, type);
			break;
		}
	}
}

static void subfunc (io_t *io, const char *pfx, int leaf, const Type_t *type)
{
	INFO info;

	if	(pfx)
	{
		io_puts(pfx, io);
		io_puts(leaf ? " `- " : " +- ", io);
	}

	io_printf(io, "%s\n", type ? type->name : "NULL");

	info.io = io;
	info.ptr = (Type_t **) TypeTab.tab;
	info.dim = TypeTab.dim;
	info.pfx = pfx ? mstrpaste(NULL, pfx, leaf ? "    " : " |  ") : "";
	next(&info, 1, type);
	memfree(info.pfx);
}

void TypeTree (io_t *io, const Type_t *type)
{
	subfunc(io, NULL, 1, type);
}
