/*
Kompakte Ein- Ausgabe von 64-Bit Ganzzahlvektoren

$Copyright (C) 2006 Erich Frühstück
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

/*
$Description

Die kompakte Darstellung von 64-Bit Ganzzahlwerten erlaubt eine dichte
Darstellung von Datenwerten unterschiedlicher Größenordnung.
Bei der Darstellung wird ein Bit als Fortsetzungsflag verwendet.
Damit werden zur Darstellung von 64-Bit Ganzzahlwerten bis zu
9 Byte benötigt (8 Byte mit je 1 Fortsetzungsbit und 7 Datenbits sowie
1 zusatzliches Byte mit 8 Datenbits).

Bei vorzeichenbehafteten Ganzzahlwerten enthält das erste Datenbyte
ein Vorzeichenflag. Der Absolutwert wird gepackt dargestellt.
Die Zahl 0x8000000000000000 wird als "negative null" dargestellt.

Die folgende Tabelle zeigt beispielhaft die Umkodierung:
---- verbatim
0	00000000
1	00000001
...
127	01111111
128	10000000 00000001
129	10000001 00000001
...
16383	11111111 01111111
16384	10000000 10000000 00000001
16385	10000001 10000000 00000001
...
----
*/

#define	SHIFT	7
#define	M_FLAG	(1 << SHIFT)
#define	M_DATA	(0xFF & ~M_FLAG)

#define	BIT_CONT	0x80	/* Fortsetzungsbit */
#define	BIT_SIG		0x40	/* Vorzeichenbit */
#define	MASK_6		0x3f	/* Maske für 6 Datenbits */
#define	MASK_7		0x7f	/* Maske für 7 Datenbits */

static int vput_residual (uint64_t val, IO *io)
{
	int n;

	for (n = 0; n < 7; n++)
	{
		if	(val > MASK_7)
		{
			io->put(BIT_CONT | (val & MASK_7), io);
			val >>= 7;
		}
		else	break;
	}

	io->put(val, io);
	return n + 1;
}

static int vget_residual (IO *io, uint64_t *val)
{
	int shift;
	int n;
	int c;

	*val = 0;
	shift = 0;
	n = 0;

	for (;;)
	{
		if	((c = io_getc(io)) == EOF)
		{
			io_error(io, "[ftools:21]", NULL);
			*val = 0;
			return 0;
		}
			
		n++;

		if	(n < 8 && (c & BIT_CONT))
		{
			*val += (uint64_t) (c & MASK_7) << shift;
			shift += 7;
		}
		else	break;
	}

	*val += (uint64_t) c << shift;
	return n;
}

/*
Die Funktion |$1| schreibt einen vorzeichenbehafteten 64-Bit Ganzzahlvektor
in kompakter Form in die Ausgabestruktur <io>.

$Diagnostic
Die Funktion |$1| liefert die Zahl der ausgegebenen Byte.
*/

size_t io_llwrite (IO *io, const int64_t *val, size_t dim)
{
	uint64_t uval;
	unsigned char byte;
	size_t n;

	if	(!(io && io->put))
		return 0;

	for (n = 0; dim-- > 0; n++, val++)
	{
		if	(*val == 0)
		{
			io->put(0, io);
			continue;
		}
		else if	(*val == -(*val))
		{
			io->put(BIT_SIG, io);
			continue;
		}

		if	(*val < 0)
		{
			byte = BIT_SIG;
			uval = -(*val);
		}
		else
		{
			byte = 0;
			uval = *val;
		}

		byte |= (uval & MASK_6);
		uval >>= 6;

		if	(uval)
		{
			io->put(byte | BIT_CONT, io);
			n += vput_residual(uval, io);
		}
		else	io->put(byte, io);
	}

	return n;
}

/*
Die Funktion |$1| schreibt einen vorzeichenlosen 64-Bit Ganzzahlvektor
in kompakter Form in die Ausgabestruktur <io>.

$Diagnostic
Die Funktion |$1| liefert die Zahl der ausgegebenen Byte.
*/

size_t io_ullwrite (IO *io, const uint64_t *val, size_t dim)
{
	size_t n;

	if	(!(io && io->put))
		return 0;

	for (n = 0; dim-- > 0; n++, val++)
	{
		if	(*val > MASK_7)
		{
			io->put(BIT_CONT | (*val & MASK_7), io);
			n += vput_residual(*val >> 7, io);
		}
		else	io->put(*val, io);
	}

	return n;
}

/*
Die Funktion |$1| liest einen kompakt gespeicherten,
vorzeichenbehafteten 64-Bit Ganzzahlvektor aus der Eingabestruktur <io>.

$Diagnostic
Keine
*/

size_t io_llread (IO *io, int64_t *val, size_t dim)
{
	int c;
	int sig;
	uint64_t uval;
	size_t n;

	for (n = 0; dim-- > 0; n++, val++)
	{
		if	((c = io_getc(io)) == EOF)
		{
			io_error(io, "[ftools:21]", NULL);
			break;
		}

		sig = (c & BIT_SIG);

		if	(c & BIT_CONT)
		{
			n += vget_residual(io, &uval);
			uval <<= 6;
		}
		else	uval = 0;

		uval += (c & MASK_6);

		if	(sig)
		{
			if	(!uval)
				uval = (uint64_t) 1 << 63;

			*val = -uval;
		}
		else	*val = uval;
	}

	return n;
}

/*
Die Funktion |$1| liest einen kompakt gespeicherten,
vorzeichenlosen 64-Bit Ganzzahlvektor aus der Eingabestruktur <io>.

$Diagnostic
Keine
*/

size_t io_ullread (IO *io, uint64_t *val, size_t dim)
{
	int c;
	size_t n;

	for (n = 0; dim-- > 0; n++, val++)
	{
		if	((c = io_getc(io)) == EOF)
		{
			io_error(io, "[ftools:21]", NULL);
			break;
		}
		else if	(c & BIT_CONT)
		{
			n += vget_residual(io, val);
			*val = (*val << 7) + (c & MASK_7);
		}
		else	*val = (c & MASK_7);
	}

	return n;
}
