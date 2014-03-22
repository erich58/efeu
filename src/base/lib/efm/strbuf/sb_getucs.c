/*
Unicode-Zeichen einlesen
*/

#include <EFEU/strbuf.h>
#include <EFEU/mstring.h>

#define	U1(c)	((c & 0x80) == 0)	/* Test auf 1-Byte Sequenz */
#define	U2(c)	((c & 0xe0) == 0xc0)	/* Test auf 2-Byte Sequenz */
#define U3(c)	((c & 0xf0) == 0xe0)	/* Test auf 3-Byte Sequenz */
#define	U4(c)	((c & 0xf8) == 0xf0)	/* Test auf 4-Byte Sequenz */

#define	V2(c)	((c & 0x1f) << 6)	/* Startbits für 2-Byte Sequenz */
#define	V3(c)	((c & 0xf) << 12)	/* Startbits für 3-Byte Sequenz */
#define	V4(c)	((c & 0x7) << 18)	/* Startbits für 4-Byte Sequenz */

#define	UF(c)	((c & 0xc0) == 0x80)	/* Test auf Folgezeichen */
#define	VF(c)	(c & 0x3f)		/* Datenbits des Folgezeichens */

int32_t sb_getucs (StrBuf *sb)
{
	unsigned n;
	unsigned char *p;

	if	(!sb)	return EOF;

	n = sb->size - sb->nfree;

	if	(sb->pos >= n)	return EOF;

	n -= sb->pos;
	p = sb->data + sb->pos;
	sb->pos++;

	if	(U1(p[0]))
	{
		return p[0];
	}
	else if	(n > 1 && U2(p[0]) && UF(p[1]))
	{
		sb->pos += 1;
		return V2(p[0]) | VF(p[1]);
	}
	else if	(n > 2 && U3(p[0]) && UF(p[1]) && UF(p[2]))
	{
		sb->pos += 2;
		return V3(p[0]) | VF(p[1]) << 6 | VF(p[2]);
	}
	else if	(n > 3 && U4(p[0]) && UF(p[1]) && UF(p[2]) && UF(p[3]))
	{
		sb->pos += 3;
		return V4(p[0]) | VF(p[1]) << 12 | VF(p[2]) << 6 | VF(p[3]);
	}
	else	return latin9_to_ucs(p[0]);

	return 0;
}
