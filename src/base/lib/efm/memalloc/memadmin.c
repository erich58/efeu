/*
Speicherplatzadministration

$Copyright (C) 1999 Erich Frühstück
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


#include <EFEU/memalloc.h>
#include <EFEU/vecbuf.h>

void memclean (void **ptr)
{
	if	(ptr && *ptr)
	{
		memfree(*ptr);
		*ptr = NULL;
	}
}

void *memadmin (void *tg, const void *src, size_t size)
{
	void *rval;

	if	(tg && !src)
	{
		memfree(tg);
		return NULL;
	}

	rval = tg ? tg : memalloc(size);

	if	(src && src != tg)
		memcpy(rval, src, size);
	else if	(tg)
		memset(rval, 0, size);

	return rval;
}

void *admin_data (alloctab_t *tab, void *tg, const void *src)
{
	void *rval;

	if	(tg && !src)
	{
		del_data(tab, tg);
		return NULL;
	}

	if	(tab == NULL)	return NULL;

	rval = tg ? tg : new_data(tab);

	if	(src && src != tg)
		memcpy(rval, src, tab->elsize);
	else if	(tg)
		memset(rval, 0, tab->elsize);

	return rval;
}
