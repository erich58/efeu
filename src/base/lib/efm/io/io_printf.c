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
	intmax_t sval;
	uintmax_t uval;
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

			switch (key.size)
			{
			case FMTKEY_LONG:
				n += fmt_ucs(io, &key, va_arg(list, int32_t));
				break;
			default:
				n += fmt_char(io, &key, va_arg(list, int));
				break;
			}

			break;

		case 'i':
		case 'd':
			switch (key.size)
			{
			case FMTKEY_IMAX:
				sval = va_arg(list, intmax_t);
				break;
			case FMTKEY_PDIFF:
				sval = va_arg(list, ptrdiff_t);
				break;
			case FMTKEY_SIZE:
				sval = va_arg(list, ssize_t);
				break;
			case FMTKEY_XLONG:
				sval = va_arg(list, int64_t);
				break;
			case FMTKEY_LONG:
				sval = va_arg(list, int32_t);
				break;
			default:
				sval = va_arg(list, int);
				break;
			}

			n += fmt_intmax(io, &key, sval);
			break;


		case 'u':
		case 'b':
		case 'o':
		case 'x':
		case 'X':

			switch (key.size)
			{
			case FMTKEY_IMAX:
				uval = va_arg(list, uintmax_t);
				break;
			case FMTKEY_PDIFF:
				uval = va_arg(list, ptrdiff_t);
				break;
			case FMTKEY_SIZE:
				uval = va_arg(list, size_t);
				break;
			case FMTKEY_XLONG:
				uval = va_arg(list, uint64_t);
				break;
			case FMTKEY_LONG:
				uval = va_arg(list, uint32_t);
				break;
			default:
				uval = va_arg(list, unsigned);
				break;
			}

			n += fmt_uintmax(io, &key, uval);
			break;

		case 'f':
		case 'e':
		case 'E':
		case 'g':
		case 'G':
		case 'a':
		case 'A':

			n += fmt_double(io, &key, va_arg(list, double));
			break;

		case 'p':
		case 'P':

			n += fmt_uintmax(io, &key,
				(uintmax_t) (size_t) va_arg(list, void *));
			break;

		case 'n':

			switch (key.size)
			{
			case FMTKEY_IMAX:
				*va_arg(list, intmax_t *) = n;
				break;
			case FMTKEY_PDIFF:
				*va_arg(list, ptrdiff_t *) = n;
				break;
			case FMTKEY_SIZE:
				*va_arg(list, size_t *) = n;
				break;
			case FMTKEY_XLONG:
				*va_arg(list, int64_t *) = n;
				break;
			case FMTKEY_LONG:
				*va_arg(list, int32_t *) = n;
				break;
			case FMTKEY_SHORT:
				*va_arg(list, int16_t *) = n;
				break;
			case FMTKEY_BYTE:
				*va_arg(list, int8_t *) = n;
				break;
			default:
				*va_arg(list, int *) = n;
				break;
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
