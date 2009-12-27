/*
String zwischenspeichern

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
#include <EFEU/efio.h>


size_t io_savestr(IO *tmp, const char *str)
{
	size_t n;

	if	(str == NULL)
	{
		io_putc(0, tmp);
		return 0;
	}

	io_putc(1, tmp);
	n = io_puts(str, tmp);
	io_putc(0, tmp);
	return n + 1;
}


char *io_loadstr(IO *tmp, char **ptr)
{
	char *p;
	int c;

	if	((c = io_getc(tmp)) <= 0)
	{
		return NULL;
	}

	p = *ptr;

	do
	{
		c = io_getc(tmp);
		**ptr = (char) c;
		(*ptr)++;
	}
	while	(c > 0);

	return p;
}
