/*
Variablentabelle ausgeben

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

#include <EFEU/object.h>
#include <ctype.h>

#define	MAX_POS	79

int ShowVarTab(io_t *io, const char *pfx, VarTab_t *vtab)
{
	int i, n, k, pos;
	char *name;

	if	(vtab == NULL)	return io_puts("NULL", io);

	if	(pfx)		n = io_printf(io, "%s", pfx);
	else if	(vtab->name)	n = io_printf(io, "%s", vtab->name);
	else			n = io_printf(io, "%#p", vtab);

	n += io_puts(" = {", io);
	pos = MAX_POS;

	for (i = 0; i < vtab->tab.dim; i++)
	{
		name = ((Var_t *) vtab->tab.tab[i])->name;

		if	(name == NULL)	continue;

		if	(pos + strlen(name) > MAX_POS)
		{
			n += io_puts("\n\t", io);
			pos = 8;
			k = 0;
		}
		else	k = io_puts(" ", io);

		k += io_puts(name, io);
		pos += k;
		n += k;
	}

	n += io_puts("\n}", io);
	return n;
}
