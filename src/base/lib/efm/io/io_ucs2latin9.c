/*
IO-Filter mit Unicode-Latin9 Konvertierung

$Copyright (C) 2009 Erich Frühstück
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
#include <EFEU/mstring.h>

static int conv_get (IO *io)
{
	return ucs_to_latin9 (io_getucs(io->data));
}


IO *io_ucs2latin9 (IO *base)
{
	if	(base)
	{
		IO *io = io_alloc();
		io->get = conv_get;
		io->data = base;
		return io;
	}

	return NULL;
}
