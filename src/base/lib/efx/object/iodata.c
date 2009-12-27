/*
Datenwerte ein/ausgeben

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

#include <EFEU/object.h>
#include <EFEU/stdtype.h>

size_t IOData (const Type_t *t, iofunc_t f, void *p, void *d)
{
	if	(t->iodata)
	{
		return t->iodata(t, f, p, d, 1);
	}
	else	return f(p, d, t->recl, t->size, 1);
}

size_t IOVecData (const Type_t *t, iofunc_t f, void *p, size_t n, void *d)
{
	if	(t->iodata)
	{
		return t->iodata(t, f, p, d, n);
	}
	else	return f(p, d, t->recl, t->size, n);
}

size_t IOData_std (const Type_t *t, iofunc_t f, void *p, void *d, size_t n)
{
	return f(p, d, t->recl, t->size, n);
}

size_t IOData_str (const Type_t *t, iofunc_t f, void *p, void *d, size_t n)
{
	return mstr_iodata(f, p, d, n);
}
