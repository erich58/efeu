/*
Werte aus Buffer abfragen

$Copyright (C) 1993 Erich Frühstück
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

#define FMT_11	"[db:11]$!: column $1: invalid digit $2 in value.\n"
#define FMT_12	"[db:12]$!: column $1: error in BCD-value $2 (HIGH)\n"
#define FMT_13	"[db:13]$!: column $1: error in BCD-value $2 (LOW)\n"
#define FMT_14	"[db:14]$!: column $1: error in BCD-value $2 (SIGNUM).\n"
#define FMT_15	"[db:15]$!: column $1: character $2 not alphanumeric.\n"


/*	Datenoffset
*/

unsigned char *db_offset (const unsigned char *buf, int pos, int len)
{
	return (unsigned char *) buf + pos - 1;
}

/*	Test auf Leerfeld
*/

int db_isblank(const unsigned char *buf, int pos, int len)
{
	for (buf += pos - 1; len-- > 0; buf++)
		if (*buf != SPACE) return 0;

	return 1;
}

int db_iskey(const unsigned char *buf, int key, int pos, int len)
{
	for (buf += pos - 1; len-- > 0; buf++)
		if (*buf != key) return 0;

	return 1;
}

/*	Test eines Feldes auf 0 oder Blank
*/

static int test_null(const unsigned char *buf, int len)
{
	for(; len-- > 0; buf++)
		if (*buf != 0 && *buf != SPACE) return 0;

	return 1;
}


/*	Zahlenwerte im Hexadezimalformat
*/

unsigned db_xval(const unsigned char *buf, int pos, int len)
{
	unsigned val;

	val = 0;
	pos--;

	while (len-- > 0)
	{
		val <<= 8;
		val += buf[pos++];
	}

	return val;
}


/*	Zahlenwerte im Zeichenformat
*/

unsigned db_cval(const unsigned char *buf, int pos, int len)
{
	unsigned val;

	val = 0;
	pos--;

	while (len-- > 0)
	{
		switch (buf[pos++])
		{
		case 0:
		case SPACE:
		case DIGIT_0:	val = 10 * val; break;
		case DIGIT_1:	val = 10 * val + 1; break;
		case DIGIT_2:	val = 10 * val + 2; break;
		case DIGIT_3:	val = 10 * val + 3; break;
		case DIGIT_4:	val = 10 * val + 4; break;
		case DIGIT_5:	val = 10 * val + 5; break;
		case DIGIT_6:	val = 10 * val + 6; break;
		case DIGIT_7:	val = 10 * val + 7; break;
		case DIGIT_8:	val = 10 * val + 8; break;
		case DIGIT_9:	val = 10 * val + 9; break;
		case LETTER_X:	val = 16 * val + 15; break;
		default:

			log_error(NULL, FMT_11, "dc", pos,
				db_char(buf, pos, 1));
			break;
		}
	}

	return val;
}

unsigned db_xcval (const unsigned char *buf, int pos, int len)
{
	return (buf[pos - 1] != 0xFF) ? db_cval(buf, pos, len) : 0;
}


static unsigned char cval_tab[10] = {
	DIGIT_0, DIGIT_1, DIGIT_2, DIGIT_3, DIGIT_4,
	DIGIT_5, DIGIT_6, DIGIT_7, DIGIT_8, DIGIT_9,
};

void set_cval (unsigned char *buf, int pos, int len, unsigned val)
{
	buf += pos - 1;

	while (len-- > 0)
	{
		buf[len] = cval_tab[val % 10];
		val /= 10;
	}
}

void set_scval (unsigned char *buf, int pos, int len, unsigned val)
{
	buf += pos - 1;

	while (len-- > 0)
	{
		if	(val)
		{
			buf[len] = cval_tab[val % 10];
			val /= 10;
		}
		else	buf[len] = SPACE;
	}
}

/*	Zahlenwerte in gepackter Form
*/

unsigned db_pval(const unsigned char *buf, int pos, int len)
{
	unsigned val;
	int c;

	val = 0;
	pos--;

	if	(test_null(buf + pos, len))
	{
		return 0;
	}

	while (len-- > 1)
	{
		val *= 10;

		if	((c = (buf[pos] >> 4)) > 9)
		{
			log_error(NULL, FMT_12, "dx", pos + 1, buf[pos]);
		}
		else	val += c;

		val *= 10;

		if	((c = (buf[pos] & 0xF)) > 9)
		{
			log_error(NULL, FMT_13, "dx", pos + 1, buf[pos]);
		}
		else	val += c;

		pos++;
	}

	val *= 10;

	if	((c = (buf[pos] >> 4)) > 9)
	{
		log_error(NULL, FMT_12, "dx", pos, buf[pos]);
	}
	else	val += c;

	switch (buf[pos] & 0xF)
	{
	case 0xD:	val = - (long) val; break;
	case 0xC:
	case 0xF:	break;
	default:	log_error(NULL, FMT_14, "dx", pos + 1, buf[pos]); break;
	}

	return val;
}

void set_pval (unsigned char *buf, int pos, int len, unsigned val)
{
	buf += pos - 1;
	len--;

	buf[len] = ((val % 10) << 4) + 0xC;

	while (len-- > 0)
	{
		val /= 10;
		buf[len] = (val % 10);
		val /= 10;
		buf[len] += ((val % 10) << 4);
	}
}

/*	Zahlenwerte in BCD-Verschlüsselung
*/

unsigned db_bcdval(const unsigned char *buf, int pos, int len)
{
	unsigned val;
	int c;

	val = 0;
	pos--;

	while (len-- > 0)
	{
		val *= 10;

		if	((c = (buf[pos] >> 4)) > 9)
		{
			log_error(NULL, FMT_12, "dx", pos + 1, buf[pos]);
		}
		else	val += c;

		val *= 10;

		if	((c = (buf[pos] & 0xF)) > 9)
		{
			log_error(NULL, FMT_13, "dx", pos + 1, buf[pos]);
		}
		else	val += c;

		pos++;
	}

	return val;
}

/*	Zeichenkette zur Basis 37
*/

unsigned db_a37l(const unsigned char *buf, int pos, int len)
{
	unsigned val;

	val = 0;
	pos--;

	while (len-- > 0)
	{
		val *= 37;

		switch (buf[pos++])
		{
		case 0:
		case SPACE:	break;
		case DIGIT_0:	val +=  1; break;
		case DIGIT_1:	val +=  2; break;
		case DIGIT_2:	val +=  3; break;
		case DIGIT_3:	val +=  4; break;
		case DIGIT_4:	val +=  5; break;
		case DIGIT_5:	val +=  6; break;
		case DIGIT_6:	val +=  7; break;
		case DIGIT_7:	val +=  8; break;
		case DIGIT_8:	val +=  9; break;
		case DIGIT_9:	val += 10; break;
		case LETTER_A:	val += 11; break;
		case LETTER_B:	val += 12; break;
		case LETTER_C:	val += 13; break;
		case LETTER_D:	val += 14; break;
		case LETTER_E:	val += 15; break;
		case LETTER_F:	val += 16; break;
		case LETTER_G:	val += 17; break;
		case LETTER_H:	val += 18; break;
		case LETTER_I:	val += 19; break;
		case LETTER_J:	val += 20; break;
		case LETTER_K:	val += 21; break;
		case LETTER_L:	val += 22; break;
		case LETTER_M:	val += 23; break;
		case LETTER_N:	val += 24; break;
		case LETTER_O:	val += 25; break;
		case LETTER_P:	val += 26; break;
		case LETTER_Q:	val += 27; break;
		case LETTER_R:	val += 28; break;
		case LETTER_S:	val += 29; break;
		case LETTER_T:	val += 30; break;
		case LETTER_U:	val += 31; break;
		case LETTER_V:	val += 32; break;
		case LETTER_W:	val += 33; break;
		case LETTER_X:	val += 34; break;
		case LETTER_Y:	val += 35; break;
		case LETTER_Z:	val += 36; break;
		default:

			log_error(NULL, FMT_15, "dc", pos, db_char(buf, pos, 1));
			break;
		}
	}

	return val;
}

unsigned db_char(const unsigned char *buf, int pos, int len)
{
	return ebcdic2ascii(buf[pos - 1]);
}

void set_char (unsigned char *buf, int pos, int len, unsigned val)
{
	buf[pos - 1] = ascii2ebcdic(val);
}

void set_blank (unsigned char *buf, int pos, int len)
{
	memset(buf + pos - 1, SPACE, len);
}

static StrBuf buf_str = SB_DATA(64);

char *db_str (const unsigned char *buf, int pos, int len)
{
	sb_clean(&buf_str);

	for (buf += pos - 1; len-- > 0; buf++)
		sb_putc(ebcdic2ascii(*buf), &buf_str);

	sb_putc(0, &buf_str);
	return (char *) buf_str.data;
}
