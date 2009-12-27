/*
Suchtabelle um Einträge erweitern

$Copyright (C) 1994 Erich Frühstück
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

#include <EFEU/efmain.h>


void xappend(xtab_t *tab, void *base, size_t nel, size_t width, int flag)

{
	char *ptr;	/* Hilfspointer */
	int i;		/* Hilfszähler */

	ptr = (char *) base;

	for (i = 0; i < nel; i++)
	{
		(void) xsearch(tab, (void *) ptr, flag);
		ptr += width;
	}
}
