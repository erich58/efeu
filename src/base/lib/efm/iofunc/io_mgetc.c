/*
Filter für Fortsetzungszeilen

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


#include <EFEU/io.h>
#include <EFEU/ioctrl.h>


int io_mgetc (IO *io, int flag)
{
	register int c, sflag;

	if	(io == NULL || io->stat != IO_STAT_OK)
		return EOF;

	sflag = io->nsave;
	c = io_getc(io);

	while (c == '\\')
	{
		if	(io_peek(io) != '\n')
			return '\\';

		io_getc(io);
		io_linemark(io);

		if	(sflag)
			return '\n';

		c = io_getc(io);

		if	(!flag)
			continue;

		while (c == ' ' || c == '\t')
			c = io_getc(io);
	}

	return c;
}
