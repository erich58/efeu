/*
Leerzeichen und Weiße Zeichen überlesen

$Copyright (C) 1999 Erich Frühstück
This file is part of EFEU.

EFEU is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

EFEU is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public
License along with EFEU; see the file COPYING.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <efeudoc.h>
#include <ctype.h>

int DocSkipSpace (IO *in, int flag)
{
	int c;

	do	c = io_skipcom(in, NULL, 0);
	while	(c == ' ' || c == '\t');

	if	(!(flag && c == '\n'))
		io_ungetc(c, in);

	return c;
}

int DocSkipWhite (IO *in)
{
	int c;

	do	c = io_skipcom(in, NULL, 1);
	while	(isspace(c));

	io_ungetc(c, in);
	return c;
}
