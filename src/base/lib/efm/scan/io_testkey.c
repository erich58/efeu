/*
:*:check keyword
:de:Schlüsselword testen

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

#include <EFEU/ioscan.h>
#include <ctype.h>

#define	ENDNAME(c)	(c != '_' && !isalnum(c))


int io_testkey (IO *io, const char *key)
{
	int i;
	int c;

	if	(key == NULL)	return 0;

	for (i = 0; ; i++)
	{
		c = io_getc(io);

		if	(key[i] == 0 || c != key[i])	break;
	}

	io_ungetc(c, io);

	if	(key[i] == 0 && ENDNAME(c))	return 1;

	while (i-- > 0)
		io_ungetc(key[i], io);

	return 0;
}
