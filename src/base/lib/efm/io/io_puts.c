/*
Zeichen/String ausgeben

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


/*	Zeichen Ausgeben
*/

int io_putc (int c, IO *io)
{
	if	(io && io->put)
	{
		int stat = io->put(c, io->data);

		if	(stat == EOF)
			io->stat |= IO_STAT_ERR;

		return stat;
	}
	else	return 0;
}


/*	Zeichen wiederholt ausgeben
*/

int io_nputc (int c, IO *io, int n)
{
	if	(n > 0 && io && io->put)
	{
		register int i;

		for (i = 0; i < n; i++)
		{
			if	(io->put(c, io->data) == EOF)
			{
				io->stat |= IO_STAT_ERR;
				break;
			}
		}

		return i;
	}
	else	return 0;
}


/*	String ausgeben
*/

int io_puts (const char *str, IO *io)
{
	if	(io && io->put && str)
	{
		const unsigned char *p;
		int n;

		p = (const unsigned char *) str;

		for (n = 0; *p != 0; p++, n++)
		{
			if	(io->put(*p, io->data) == EOF)
			{
				io->stat |= IO_STAT_ERR;
				break;
			}
		}

		return n;
	}
	else	return 0;
}

/*	String mit Zeilenvorschub ausgeben
*/

int io_nlputs(const char *str, IO *io)
{
	if	(io && io->put && str)
	{
		const unsigned char *p;
		int n, last;

		p = (const unsigned char *) str;
		last = '\n';

		for (n = 0; *p != 0; last = *(p++), n++)
		{
			if	(io->put(*p, io->data) == EOF)
			{
				io->stat |= IO_STAT_ERR;
				return n;
			}
		}

		if	(last != '\n')
		{
			if	(io->put('\n', io->data) == EOF)
			{
				io->stat |= IO_STAT_ERR;
			}
			else	n++;
		}

		return n;
	}
	else	return 0;
}

/*	IO-Struktur umkopieren
*/

int io_copy (IO *in, IO *out)
{
	int c, n;

	for (n = 0; (c = io_getc(in)) != EOF; n++)
		if (io_putc(c, out) == EOF) break;

	return n;
}
