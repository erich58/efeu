/*
Ein/Ausgabefunktionen

$Copyright (C) 1998 Erich Frühstück
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
#include <EFEU/ftools.h>

#if	REVBYTEORDER
#define	DATA(ptr,size)	(unsigned char *) ptr + size - 1
#define	SET(ptr,val)	*(ptr--) = val
#define	GET(ptr)	*(ptr--)
#else
#define	DATA(ptr,size)	(unsigned char *) ptr
#define	SET(ptr,val)	*(ptr++) = val
#define	GET(ptr)	*(ptr++)
#endif

/*	Datensätze lesen
*/

size_t io_dbread (IO *io, void *dptr, size_t recl, size_t size, size_t n)
{
	register size_t i, j;
	register int c;

	if	(io == NULL || io->stat != IO_STAT_OK)
	{
		io_error(io, "[ftools:21]", NULL);
		return 0;
	}

	if	(io->dbread && !io->nsave)
		return io->dbread(io, dptr, recl, size, n);

	for (i = 0; i < n; i++)
	{
		register unsigned char *data = DATA(dptr, size);

		for (j = 0; j < recl; j++)
		{
			if	((c = io_getc(io)) == EOF)
				io_error(io, "[ftools:21]", NULL);

			SET(data, c);
		}

		for (j = recl; j < size; j++)
			SET(data, 0);

		dptr = (char *) dptr + size;
	}

	return recl * n;
}


/*	Datensätze ausgeben
*/

size_t io_dbwrite (IO *io, const void *dptr, size_t recl, size_t size, size_t n)
{
	register size_t i, j;
	register int c;
	
	if	(io && io->dbwrite)
		return io->dbwrite(io, dptr, recl, size, n);

	for (i = 0; i < n; i++)
	{
		register unsigned char *data = DATA(dptr, size);

		for (j = 0; j < recl; j++)
		{
			c = GET(data);

			if	(io_putc(c, io) == EOF)
				io_error(io, "[ftools:22]", NULL);
		}

		dptr = (char *) dptr + size;
	}

	return recl * n;
}
