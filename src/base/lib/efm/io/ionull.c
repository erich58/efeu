/*
Dummy-Ein/Ausgabe

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

static int null_put (int c, IO *io)
{
	return c;
}

static int null_ctrl (IO *io, int c, va_list list)
{
	switch (c)
	{
	case IO_REWIND:	return 0;
	case IO_IDENT:	*va_arg(list, char **) = io->data; return 0;
	default:	return EOF;
	}
}

static IO ios_null = STD_IODATA (NULL, null_put, null_ctrl, "<null>");

IO *ionull = &ios_null;
