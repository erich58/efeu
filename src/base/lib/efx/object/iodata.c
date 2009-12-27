/*
Datenwerte ein/ausgeben

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

#include <EFEU/object.h>
#include <EFEU/stdtype.h>

size_t ReadData (const EfiType *type, void *data, IO *io)
{
	CleanData(type, data, 0);

	if	(type->read)
	{
		return type->read(type, data, io);
	}
	else if	(type->recl)
	{
		return io_dbread(io, data, type->recl, type->size, 1);
	}
	else	return 0;
}

size_t ReadVecData (const EfiType *type, size_t dim, void *data, IO *io)
{
	CleanVecData(type, dim, data, 0);

	if	(type->read)
	{
		size_t n = 0;

		while (dim > 0)
		{
			n += type->read(type, data, io);
			data = ((char *) data) + type->size;
			dim--;
		}

		return n;
	}
	else if	(type->recl)
	{
		return io_dbread(io, data, type->recl, type->size, dim);
	}
	else	return 0;
}


size_t WriteData (const EfiType *type, const void *data, IO *io)
{
	if	(type->write)
	{
		return type->write(type, data, io);
	}
	else if	(type->recl)
	{
		return io_dbwrite(io, data, type->recl, type->size, 1);
	}
	else	return 0;
}

size_t WriteVecData (const EfiType *type, size_t dim, const void *data, IO *io)
{
	if	(type->write)
	{
		size_t n = 0;

		while (dim > 0)
		{
			n += type->write(type, data, io);
			data = ((const char *) data) + type->size;
			dim--;
		}

		return n;
	}
	else if	(type->recl)
	{
		return io_dbwrite(io, data, type->recl, type->size, dim);
	}
	else	return 0;
}
