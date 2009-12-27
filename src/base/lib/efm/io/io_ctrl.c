/*
IO-Basiskontrolle

$Copyright (C) 1997 Erich Frühstück
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


/*	Kontrollfunktion
*/

int io_vctrl (IO *io, int req, va_list list)
{
	return (io && io->ctrl) ? io->ctrl(io, req, list) : EOF;
}

int io_ctrl (IO *io, int req, ...)
{
	if	(io && io->ctrl)
	{
		va_list list;
		int stat;

		va_start(list, req);
		stat = (*io->ctrl)(io, req, list);
		va_end(list);
		return stat;
	}
	else	return EOF;
}
