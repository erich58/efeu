/*	Datenwerte formatiern
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/io.h>
#include <EFEU/fmtkey.h>

int io_printf(io_t *io, const char *fmt, ...)
{
	va_list args;
	int n;

	va_start(args, fmt);
	n = io_vprintf(io, fmt, args);
	va_end(args);
	return n;
}


int io_vprintf(io_t *io, const char *fmt, va_list list)
{
	fmtkey_t key;
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

			n += fmt_char(io, &key, va_arg(list, int));
			break;

		case 'i':
		case 'd':
		case 'u':
		case 'b':
		case 'o':
		case 'x':
		case 'X':

			if	(key.flags & FMT_LONG)
			{
				n += fmt_long(io, &key, va_arg(list, long));
			}
			else	n += fmt_long(io, &key, va_arg(list, int));

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

			n += fmt_long(io, &key, (long) va_arg(list, void *));
			break;

		case 'n':

			if	(key.flags & FMT_LONG)
			{
				va_arg(list, long *)[0] = n;
			}
			else if	(key.flags & FMT_SHORT)
			{
				va_arg(list, short *)[0] = n;
			}
			else	va_arg(list, int *)[0] = n;

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
