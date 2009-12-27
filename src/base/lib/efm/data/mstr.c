/*
Zeichenketten

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

#include <EFEU/data.h>

static size_t do_io (iofunc_t func, void *ptr, mstr_t *str, int mode)
{
	size_t size, recl;
	ushort_t len;

	size = *str ? strlen(*str) + 1 : 0;
	len = size;

	if	(len < size)
	{
		size = len;
	}

	recl = func(ptr, &len, mode ? sizeof(ushort_t) : 0, sizeof(ushort_t), 1);

	if	(len != size)
	{
		memfree(*str);
		*str = memalloc(len);
	}

	if	(mode)
		recl += func(ptr, *str, 1, 1, len);

	return recl;
}

#if	0
size_t mstr_ioskip (iofunc_t func, void *ptr, mstr_t *str, size_t n)
{
	register size_t recl;

	for (recl = 0; n > 0; n--, str++)
		recl += do_io (func, ptr, str, 0);

	return recl;
}
#endif

size_t mstr_iodata (iofunc_t func, void *ptr, mstr_t *str, size_t n)
{
	register size_t recl;

	for (recl = 0; n > 0; n--, str++)
		recl += do_io (func, ptr, str, 1);

	return recl;
}

void mstr_clrdata (mstr_t *str, size_t n)
{
	for (; n > 0; n--, str++)
	{
		memfree(*str);
		*str = NULL;
	}
}
