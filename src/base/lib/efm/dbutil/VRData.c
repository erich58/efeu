/*
Datenbanken mit variabler Satzlänge

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

#include <EFEU/VRData.h>

#define	VRBLKSIZE	1024

size_t VRData_read (io_t *io, VRData_t *data, size_t n)
{
	size_t x = 0;

	while (n-- > 0)
	{
		data->recl = io_getval(io, 2);

		if	(data->recl < 4)
		{
			io_error(io, MSG_VRDATA, 3, 0);
			break;
		}
			
		if	(io_getval(io, 2) != 0)
		{
			io_error(io, MSG_VRDATA, 4, 0);
			break; 
		}

		data->recl -= 4;

		if	(data->size < data->recl)
		{
			data->size = sizealign(data->recl, VRBLKSIZE);
			memfree(data->buf);
			data->buf = lmalloc(data->size);
		}

		x += io_read(io, data->buf, data->recl);
		data++;
	}

	return x;
}

size_t VRData_write (io_t *io, VRData_t *data, size_t n)
{
	size_t x = 0;

	while (n-- > 0)
	{
		io_putval(data->recl, io, 2);
		io_putval(0, io, 2);
		x += io_write(io, data->buf, data->recl);
		data++;
	}

	return x;
}

void VRData_clear (VRData_t *data, size_t n)
{
	while (n-- > 0)
	{
		lfree(data->buf);
		data->buf = NULL;
		data->recl = data->size = 0;
		data++;
	}
}
