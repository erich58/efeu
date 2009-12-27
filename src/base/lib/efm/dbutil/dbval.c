/*	Werte aus Buffer abfragen
	(c) 1993 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/ebcdic.h>
#include <EFEU/dbutil.h>
#include <EFEU/procenv.h>
#include <EFEU/strbuf.h>


static void val_error(int pos, int num, const uchar_t *ptr)
{
	char buf[32];

	sprintf(buf, "%d", num);
	message(buf, MSG_DB, num, 1,
		ptr ? msprintf("%#c", db_char(ptr, pos, 1)) : NULL);
	procexit(EXIT_FAILURE);
}

/*	Test auf Leerfeld
*/

int db_isblank(const uchar_t *buf, int pos, int len)
{
	for (buf += pos - 1; len-- > 0; buf++)
		if (*buf != SPACE) return 0;

	return 1;
}

int db_iskey(const uchar_t *buf, int key, int pos, int len)
{
	for (buf += pos - 1; len-- > 0; buf++)
		if (*buf != key) return 0;

	return 1;
}

/*	Test eines Feldes auf 0 oder Blank
*/

static int test_null(const uchar_t *buf, int len)
{
	for(; len-- > 0; buf++)
		if (*buf != 0 && *buf != SPACE) return 0;

	return 1;
}


/*	Zahlenwerte im Hexadezimalformat
*/

unsigned db_xval(const uchar_t *buf, int pos, int len)
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

unsigned db_cval(const uchar_t *buf, int pos, int len)
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

			val_error(pos, 11, buf);
			break;
		}
	}

	return val;
}

unsigned db_xcval (const uchar_t *buf, int pos, int len)
{
	return (buf[pos - 1] != 0xFF) ? db_cval(buf, pos, len) : 0;
}



/*	Zahlenwerte in gepackter Form
*/

unsigned db_pval(const uchar_t *buf, int pos, int len)
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
			val_error(pos + 1, 12, NULL);
		}
		else	val += c;

		val *= 10;

		if	((c = (buf[pos] & 0xF)) > 9)
		{
			val_error(pos + 1, 13, NULL);
		}
		else	val += c;

		pos++;
	}

	val *= 10;

	if	((c = (buf[pos] >> 4)) > 9)
	{
		val_error(pos, 12, NULL);
	}
	else	val += c;

	switch (buf[pos] & 0xF)
	{
	case 0xD:	val = - (long) val; break;
	case 0xC:
	case 0xF:	break;
	default:	val_error(pos + 1, 14, NULL); break;
	}

	return val;
}

/*	Zahlenwerte in BCD-Verschlüsselung
*/

unsigned db_bcdval(const uchar_t *buf, int pos, int len)
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
			val_error(pos + 1, 12, NULL);
		}
		else	val += c;

		val *= 10;

		if	((c = (buf[pos] & 0xF)) > 9)
		{
			val_error(pos + 1, 13, NULL);
		}
		else	val += c;

		pos++;
	}

	return val;
}

/*	Zeichenkette zur Basis 37
*/

unsigned db_a37l(const uchar_t *buf, int pos, int len)
{
	unsigned val;

	val = 0;
	pos--;

	while (len-- > 0)
	{
		val *= 37;

		switch (buf[pos++])
		{
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

			val_error(pos, 15, buf);
			break;
		}
	}

	return val;
}

unsigned db_char(const uchar_t *buf, int pos, int len)
{
	return ebcdic2ascii(buf[pos - 1]);
}

static strbuf_t buf_str = SB_DATA(64);

char *db_str (const uchar_t *buf, int pos, int len)
{
	sb_clear(&buf_str);

	for (buf += pos - 1; len-- > 0; buf++)
		sb_putc(ebcdic2ascii(*buf), &buf_str);

	sb_putc(0, &buf_str);
	return (char *) buf_str.data;
}
