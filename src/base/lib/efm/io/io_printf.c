/*
Datenwerte formatiern

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
#include <EFEU/fmtkey.h>

int io_printf(IO *io, const char *fmt, ...)
{
	va_list args;
	int n;

	va_start(args, fmt);
	n = io_vprintf(io, fmt, args);
	va_end(args);
	return n;
}


int io_vprintf(IO *io, const char *fmt, va_list list)
{
	FmtKey key;
	int n;

	if	(fmt == NULL)	return 0;

	n = 0;

	while (*fmt != 0)
	{
		if	(*fmt != '%')
		{
			if	(io_putc(*fmt, io) == EOF)
				break;

			n++;
			fmt++;
			continue;
		}

		fmt++;
		fmt += fmtkey(fmt, &key);

		if	(key.flags & FMT_NEED_WIDTH)
		{
			key.width = va_arg(list, int);

			if	(key.width < 0)
			{
				key.flags ^= FMT_RIGHT;
				key.width = -key.width;
			}
		}

		if	(key.flags & FMT_NEED_PREC)
		{
			key.prec = va_arg(list, int);

			if	(key.prec < 0)
			{
				key.flags ^= FMT_NEGPREC;
				key.prec = -key.prec;
			}
		}

		switch (key.mode)
		{
		case 's':
		case 'S':
		
			n += fmt_str(io, &key, va_arg(list, char *));
			break;

		case 'c':
		case 'C':

			if	(key.flags & FMT_LONG)
			{
				n += fmt_ucs(io, &key, va_arg(list, int32_t));
			}
			else	n += fmt_char(io, &key, va_arg(list, int));

			break;

		case 'i':
		case 'd':
		case 'u':
		case 'b':
		case 'o':
		case 'x':
		case 'X':

			if	(key.flags & FMT_XLONG)
			{
				n += fmt_int64(io, &key, va_arg(list, int64_t));
			}
			else if	(key.flags & FMT_LONG)
			{
				n += fmt_int64(io, &key, va_arg(list, int32_t));
			}
			else	n += fmt_int64(io, &key, va_arg(list, int));

			break;

		case 'f':
		case 'e':
		case 'E':
		case 'g':
		case 'G':

			n += fmt_double(io, &key, va_arg(list, double));
			break;

		case 'p':
		case 'P':

			n += fmt_uint64(io, &key,
				(uint64_t) (size_t) va_arg(list, void *));
			break;

		case 'n':

			if	(key.flags & FMT_XLONG)
			{
				int64_t *ptr = va_arg(list, int64_t *);
				*ptr = n;
			}
			else if	(key.flags & FMT_LONG)
			{
				int32_t *ptr = va_arg(list, int32_t *);
				*ptr = n;
			}
			else if	(key.flags & FMT_BYTE)
			{
				int8_t *ptr = va_arg(list, int8_t *);
				*ptr = n;
			}
			else if	(key.flags & FMT_SHORT)
			{
				int16_t *ptr = va_arg(list, int16_t *);
				*ptr = n;
			}
			else
			{
				int *ptr = va_arg(list, int *);
				*ptr = n;
			}

			break;

		case '[':

			n += fmt_str(io, &key, key.list);
			memfree(key.list);
			break;

		default:

			if	(io_putc(key.mode, io) == EOF)
				return n;

			n++;
			break;
		}
	}

	return n;
}
