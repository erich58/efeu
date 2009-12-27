/*
String einlesen

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

#include <EFEU/io.h>


char *io_gets (char *buf, int n, io_t *io)
{
	int c;
	int k;

	k = 0;
	n--;

	while (k < n && (c = io_getc(io)) != EOF)
	{
		buf[k++] = (char) c;

		if	(c == '\n')	break;
	}

	if	(k > 0)
	{
		buf[k] = 0;
		return buf;
	}
	else	return NULL;
}
