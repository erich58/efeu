/*
Fehlermeldung mit IO-Struktur

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

#include <EFEU/io.h>
#include <EFEU/ioctrl.h>
#include <EFEU/procenv.h>

void io_message(io_t *io, const char *name, int num, int narg, ...)
{
	char *id = io_ident(io);
	va_list list;
	va_start(list, narg);
	vmessage(id, name, num, narg, list);
	memfree(id);
	va_end(list);
}

void io_error(io_t *io, const char *name, int num, int narg, ...)
{
	char *id = io_ident(io);
	va_list list;
	va_start(list, narg);
	vmessage(id, name, num, narg, list);
	memfree(id);
	va_end(list);

	if	(io_ctrl(io, IO_ERROR) == EOF)
		exit(EXIT_FAILURE);
}

