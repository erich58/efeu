/*
ASCII-Konvertierungsfunktionen

$Copyright (C) 2001 Erich Frühstück
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

#include <EFEU/ebcdic.h>
#include <EFEU/dbutil.h>
#include <EFEU/procenv.h>
#include <EFEU/strbuf.h>
#include <EFEU/mstring.h>
#include <EFEU/Debug.h>

#define FMT_21	"[db:11]$!: column $1: invalid digit $2 in value.\n"
#define FMT_25	"[db:15]$!: column $1: character $2 not alphanumeric.\n"


int txt_isblank (const char *buf, int pos, int len)
{
	for (buf += pos - 1; len-- > 0; buf++)
		if (*buf != ' ') return 0;

	return 1;
}

/*	Zahlenwerte im Zeichenformat
*/

unsigned txt_unsigned (const char *buf, int pos, int len)
{
	unsigned val;

	val = 0;
	pos--;

	while (len-- > 0)
	{
		switch (buf[pos++])
		{
		case   0:	/* FALLTHROUGH */
		case ' ':	/* FALLTHROUGH */
		case '0':	val = 10 * val; break;
		case '1':	val = 10 * val + 1; break;
		case '2':	val = 10 * val + 2; break;
		case '3':	val = 10 * val + 3; break;
		case '4':	val = 10 * val + 4; break;
		case '5':	val = 10 * val + 5; break;
		case '6':	val = 10 * val + 6; break;
		case '7':	val = 10 * val + 7; break;
		case '8':	val = 10 * val + 8; break;
		case '9':	val = 10 * val + 9; break;
		case '.':	/* FALLTHROUGH */
		case ',':	break;
		default:
			dbg_error(NULL, FMT_21, "dc", pos,
				txt_char(buf, pos, 1));
			break;
		}
	}

	return val;
}


/*	Zeichenkette zur Basis 37
*/

unsigned txt_base37 (const char *buf, int pos, int len)
{
	unsigned val;

	val = 0;
	pos--;

	while (len-- > 0)
	{
		val *= 37;

		switch (buf[pos++])
		{
		case ' ':		break;
		case '0':		val +=  1; break;
		case '1':		val +=  2; break;
		case '2':		val +=  3; break;
		case '3':		val +=  4; break;
		case '4':		val +=  5; break;
		case '5':		val +=  6; break;
		case '6':		val +=  7; break;
		case '7':		val +=  8; break;
		case '8':		val +=  9; break;
		case '9':		val += 10; break;
		case 'A': case 'a':	val += 11; break;
		case 'B': case 'b':	val += 12; break;
		case 'C': case 'c':	val += 13; break;
		case 'D': case 'd':	val += 14; break;
		case 'E': case 'e':	val += 15; break;
		case 'F': case 'f':	val += 16; break;
		case 'G': case 'g':	val += 17; break;
		case 'H': case 'h':	val += 18; break;
		case 'I': case 'i':	val += 19; break;
		case 'J': case 'j':	val += 20; break;
		case 'K': case 'k':	val += 21; break;
		case 'L': case 'l':	val += 22; break;
		case 'M': case 'm':	val += 23; break;
		case 'N': case 'n':	val += 24; break;
		case 'O': case 'o':	val += 25; break;
		case 'P': case 'p':	val += 26; break;
		case 'Q': case 'q':	val += 27; break;
		case 'R': case 'r':	val += 28; break;
		case 'S': case 's':	val += 29; break;
		case 'T': case 't':	val += 30; break;
		case 'U': case 'u':	val += 31; break;
		case 'V': case 'v':	val += 32; break;
		case 'W': case 'w':	val += 33; break;
		case 'X': case 'x':	val += 34; break;
		case 'Y': case 'y':	val += 35; break;
		case 'Z': case 'z':	val += 36; break;
		default:

			dbg_error(NULL, FMT_25, "dc", pos,
				txt_char(buf, pos, 1));
			break;
		}
	}

	return val;
}

static StrBuf buf_str = SB_DATA(64);

char *txt_str (const char *buf, int pos, int len)
{
	sb_clear(&buf_str);

	for (buf += pos - 1; len-- > 0; buf++)
		sb_putc(*buf, &buf_str);

	sb_putc(0, &buf_str);
	return (char *) buf_str.data;
}

char txt_char (const char *buf, int pos, int len)
{
	return buf[pos - 1];
}
