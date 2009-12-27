/*
Umwandeln eines Wertes in Ziffern der Basis 37

$Copyright (C) 1994 Erich Fr�hst�ck
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

#include <EFEU/efutil.h>

static char *digit = " 0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static char buf[8];	/* Statischer Buffer */

char *l37a (unsigned x)
{
	char *p;

	p = buf + 7;
	*p = 0;

	while (x != 0)
	{
		*(--p) = digit[x % 37];
		x /= 37;
	}

	return p;
}
