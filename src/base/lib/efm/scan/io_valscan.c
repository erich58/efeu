/*
:*:scan numeric values
:de:Zahlenwerte abfragen

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

#include <EFEU/procenv.h>
#include <EFEU/ioscan.h>
#include <EFEU/locale.h>
#include <ctype.h>


/*	Statische Buffer für Rückgabewerte
*/

static int scan_ibuf = 0;
static long scan_lbuf = 0;
static double scan_dbuf = 0;



/*	Test auf Tausendtrennstelle
*/

static int test_key (IO *io, int c, const char *key, int flag)
{
	int i;

	if	(key == NULL || c == 0 || key[0] != c)
		return 0;

	for (i = 1; key[i] != 0; i++)
	{
		c = io_getc(io);

		if	(c != key[i])
		{
			io_ungetc(c, io);

			while (--i > 0)
				io_ungetc(key[i], io);

			return 0;
		}
	}

	if	(flag)	return 1;

	c = io_getc(io);
	io_ungetc(c, io);

	if	(isdigit(c))	return 1;

	while (--i > 0)
		io_ungetc(key[i], io);

	return 0;
}

static int test_sep(IO *io, int c)
{
	if	(Locale.scan && Locale.scan->thousands_sep)
		return test_key(io, c, Locale.scan->thousands_sep, 0);

	return 0;
}


/*	Test auf Dezimalpunkt
*/

static int test_dp (IO *io, int c, int flag)
{
	if	(Locale.scan && Locale.scan->decimal_point)
		return test_key(io, c, Locale.scan->decimal_point, flag);

	if	(c == '.' || c == ',')
	{
		if	(flag)	return 1;

		c = io_getc(io);
		io_ungetc(c, io);

		if	(isdigit(c))	return 1;
	}

	return 0;
}



static unsigned long get_binval (IO *io)
{
	register unsigned long val = 0;

	for (;;)
	{
		register int c = io_getc(io);

		switch (c)
		{
		default:	io_ungetc(c, io); /* FALLTHROUGH */
		case EOF:	return val;
		case '0':	val <<= 1; break;
		case '1':	val = (val << 1) + 1; break;
		}
	}
}

static unsigned long get_hexval (IO *io)
{
	register unsigned long val = 0;

	for (;;)
	{
		register int c = io_getc(io);

		switch (c)
		{
		default:	io_ungetc(c, io); /* FALLTHROUGH */
		case EOF:	return val;
		case '0':	c = 0; break;
		case '1':	c = 1; break;
		case '2':	c = 2; break;
		case '3':	c = 3; break;
		case '4':	c = 4; break;
		case '5':	c = 5; break;
		case '6':	c = 6; break;
		case '7':	c = 7; break;
		case '8':	c = 8; break;
		case '9':	c = 9; break;
		case 'a': case 'A':	c = 10; break;
		case 'b': case 'B':	c = 11; break;
		case 'c': case 'C':	c = 12; break;
		case 'd': case 'D':	c = 13; break;
		case 'e': case 'E':	c = 14; break;
		case 'f': case 'F':	c = 15; break;
		}

		val <<= 4;
		val += c;
	}
}


static int make_integer (IO *io, void **ptr, long val, unsigned base)
{
	register unsigned type = SCAN_INT;
	register unsigned flags = 0;

	switch (base & (SCAN_INT | SCAN_LONG))
	{
	case SCAN_INT:	base = SCAN_INT; break;
	case SCAN_LONG:	base = SCAN_LONG; break;
	default:	base = 0; break;
	}
	
	for (;;)
	{
		register int c = io_getc(io);

		switch (c)
		{
		case 'u': case 'U':

			if	(flags & SCAN_UNSIGNED)
			{
				io_error(io, "[efm:51]", NULL);
				break;
			}
			else	flags |= SCAN_UNSIGNED;

			break;

		case 'l': case 'L':

			if	(type == SCAN_LONG)
			{
				io_error(io, "[efm:52]", NULL);
				break;
			}
			else	type = SCAN_LONG;

			break;

		default:

			io_ungetc(c, io);
			/* FALLTHROUGH */

		case EOF:
			if	(base)
				type = base;

			if	(ptr == NULL)
			{
				;
			}
			else if	(type == SCAN_INT)
			{
				scan_ibuf = val;
				*ptr = &scan_ibuf;
			}
			else
			{
				scan_lbuf = val;
				*ptr = &scan_lbuf;
			}

			return (type | flags);
		}
	}
}

/*	Test auf Exponent
*/

static int add_exp (IO *io, StrBuf *sb, int c)
{
	if	(c == 'e' || c == 'E')
	{
		int sign;
		int d;

		d = io_getc(io);

		if	(d == '+' || d == '-')
		{
			sign = d;
			d = io_getc(io);
		}
		else	sign = 0;

		if	(isdigit(d))
		{
			sb_putc(c, sb);
			c = d;

			if	(sign)
				sb_putc(sign, sb);

			while (isdigit(c))
			{
				sb_putc(c, sb);
				c = io_getc(io);
			}

			io_ungetc(c, io);
			return 1;
		}

		io_ungetc(d, io);

		if	(sign)
			io_ungetc(sign, io);
	}

	return 0;
}


static void add_fract (IO *io, StrBuf *sb)
{
	int c;

	while ((c = io_getc(io)) != EOF)
	{
		if	(isdigit(c))		sb_putc(c, sb);
		else if	(!test_sep(io, c))	break;
	}

	if	(!add_exp(io, sb, c))
		io_ungetc(c, io);
}


static int make_double (IO *io, void **ptr, StrBuf *sb, int sign)
{
	if	(ptr)
	{
		sb_putc(0, sb);
		scan_dbuf = strtod((char *) sb->data, NULL);

		if	(sign)	scan_dbuf = -scan_dbuf;

		*ptr = &scan_dbuf;
	}

	del_strbuf(sb);
	return SCAN_DOUBLE;
}


#define	GET_INT	1
#define	GET_DBL	2
#define	GET_ANY	3

int io_valscan (IO *in, unsigned flags, void **ptr)
{
	long lval;
	int sign;
	StrBuf *sb;
	int c;
	int valtype;
	int base;
	int mode;

/*	Verarbeitungsmodus bestimmen
*/
	mode = 0;

	if	(flags & SCAN_INT)	mode |= GET_INT;
	if	(flags & SCAN_LONG)	mode |= GET_INT;
	if	(flags & SCAN_DOUBLE)	mode |= GET_DBL;

	if	(mode == GET_DBL)
		flags &= ~SCAN_MODEMASK;

/*	Vorzeichen abfragen
*/
	c = io_eat(in, " \t");

	if	(c == '-' || c == '+')
	{
		sign = io_getc(in);
	}
	else	sign = 0;

	lval = (sign == '-' ? -1 : 1);

/*	Test auf Ganzzahlwert mit anderer Basis
*/
	c = io_getc(in);
	valtype = 0;

	if	(c == '0' && (flags & SCAN_MODEMASK))
	{
		c = io_getc(in);

		if	((flags & SCAN_HEXVAL) && (c == 'x' || c == 'X'))
		{
			lval *= get_hexval(in);
			flags &= (SCAN_INT | SCAN_LONG | SCAN_HEXVAL);
			return make_integer(in, ptr, lval, flags);
		}
		else if	((flags & SCAN_BINVAL) && (c == 'b' || c == 'B'))
		{
			lval *= get_binval(in);
			flags &= (SCAN_INT | SCAN_LONG | SCAN_BINVAL);
			return make_integer(in, ptr, lval, flags);
		}

		valtype = SCAN_INT;
		base = (flags & SCAN_OCTVAL) ? 8 : 10;
	}
	else	base = 10;

/*	Test auf Gleitkommawert ohne Vorkommastellen
*/
	if	(mode != GET_INT && test_dp(in, c, valtype))
	{
		sb = new_strbuf(0);
		sb_putc('0', sb);
		sb_putc('.', sb);
		add_fract(in, sb);
		return make_double(in, ptr, sb, sign == '-');
	}

/*	Test auf vorhandene Ziffern
*/
	if	(valtype == 0 && !isdigit(c))
	{
		io_ungetc(c, in);

		if	(sign)
			io_ungetc(sign, in);

		return 0;
	}

/*	Ganzzahlwert/Vorkommastellen zwischenspeichern
*/
	sb = new_strbuf(0);
	valtype = SCAN_INT;

	while (isdigit(c))
	{
		if	(c == '8' || c == '9')
			base = 10;

		sb_putc(c, sb);
		c = io_getc(in);

		while (test_sep(in, c))
			c = io_getc(in);
	}

/*	Test auf Gleitkommawert
*/
	if	(mode == GET_INT)
	{
		;
	}
	else if	(test_dp(in, c, 1))
	{
		sb_putc('.', sb);
		add_fract(in, sb);
		return make_double(in, ptr, sb, sign == '-');
	}
	else if	(add_exp(in, sb, c))
	{
		return make_double(in, ptr, sb, sign == '-');
	}
	else if	(mode == GET_DBL)
	{
		io_ungetc(c, in);
		return make_double(in, ptr, sb, sign == '-');
	}

	io_ungetc(c, in);
	sb_putc(0, sb);
	lval *= (long) strtoul((char *) sb->data, NULL, base);
	del_strbuf(sb);
	return make_integer(in, ptr, lval, flags);
}
