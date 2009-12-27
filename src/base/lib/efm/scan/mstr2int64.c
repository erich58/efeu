/*
:*:convert string to 64-bit integer
:de:Zeichenkette in 64-Bit Ganzzahlwert konvertieren

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

#include <EFEU/ioscan.h>
#include <ctype.h>

static int overflow = 0;

static uint64_t do_conv (const char *str, char **ptr, int base, uint64_t mval)
{
	uint64_t x, y;
	int c;

	if	(base == 0)
	{
		if	(str[0] == '0')
		{
			str++;

			switch (*str)
			{
			case 'x': case 'X':	str++; base = 16; break;
			case 'b': case 'B':	str++; base = 2; break;
			default:		base = 8; break;
			}
		}
		else	base = 10;
	}

	overflow = 0;

	for (x = 0; *str; str++)
	{
		switch (*str)
		{
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
		case 'g': case 'G':	c = 16; break;
		case 'h': case 'H':	c = 17; break;
		case 'i': case 'I':	c = 18; break;
		case 'j': case 'J':	c = 19; break;
		case 'k': case 'K':	c = 20; break;
		case 'l': case 'L':	c = 21; break;
		case 'm': case 'M':	c = 22; break;
		case 'n': case 'N':	c = 23; break;
		case 'o': case 'O':	c = 24; break;
		case 'p': case 'P':	c = 25; break;
		case 'q': case 'Q':	c = 26; break;
		case 'r': case 'R':	c = 27; break;
		case 's': case 'S':	c = 28; break;
		case 't': case 'T':	c = 29; break;
		case 'u': case 'U':	c = 30; break;
		case 'v': case 'V':	c = 31; break;
		case 'w': case 'W':	c = 32; break;
		case 'x': case 'X':	c = 33; break;
		case 'y': case 'Y':	c = 34; break;
		case 'z': case 'Z':	c = 35; break;
		default:		c = base; break;
		}

		if	(c >= base)	break;
		if	(overflow)	continue;

		y = x * base + c;

		if	(y < x)
		{
			x = mval;
			overflow = 1;
		}
		else	x = y;
	}

	if	(x > mval)
	{
		x = mval;
		overflow = 1;
	}

	*ptr = (char *) str;
	return x;
}

int64_t mstr2int64 (const char *str, char **ptr, int base)
{
	int64_t x = 0;
	char *p = (char *) str;

	if	(str)
	{
		char *p2;

		while (isspace((unsigned char) *str))
			str++;

		if	(*str == '-')
		{
			str++;
			x = - do_conv(str, &p2, base, (uint64_t) 1 << 63);
		}
		else if	(*str == '+')
		{
			str++;
			x = do_conv(str, &p2, base, ~ (uint64_t) 0 >> 1);
		}
		else	x = do_conv(str, &p2, base, ~ (uint64_t) 0 >> 1);

		if	(p2 != str)
			p = p2;
	}

	if	(ptr)	*ptr = p;

	return x;
}

uint64_t mstr2uint64 (const char *str, char **ptr, int base)
{
	uint64_t m = ~ (uint64_t) 0;
	uint64_t x = 0;
	char *p = (char *) str;

	if	(str)
	{
		char *p2;

		while (isspace((unsigned char) *str))
			str++;

		if	(*str == '-')
		{
			str++;
			x = do_conv(str, &p2, base, m);
			x = overflow ? m : -x;
		}
		else if	(*str == '+')
		{
			str++;
			x = do_conv(str, &p2, base, m);
		}
		else	x = do_conv(str, &p2, base, m);

		if	(p2 != str)
			p = p2;
	}

	if	(ptr)	*ptr = p;

	return x;
}
