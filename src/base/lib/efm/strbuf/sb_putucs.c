/*
Unicode-Zeichen ausgeben
*/

#include <EFEU/strbuf.h>

int sb_putucs (int32_t c, StrBuf *sb)
{
	if	(c < 0 || sb == NULL)
	{
		return 0;
	}
	else if	(c <= 0x0000007F)
	{
		sb_putc(c, sb);
		return 1;
	}
	else if	(c <= 0x000007FF)
	{
		sb_putc(0xc0 | ((c >> 6) & 0x1f), sb);
		sb_putc(0x80 | (c & 0x3f), sb);
		return 2;
	}
	else if	(c <= 0x0000FFFF)
	{
		sb_putc(0xe0 | ((c >> 12) & 0x0f), sb);
		sb_putc(0x80 | ((c >> 6) & 0x3f), sb);
		sb_putc(0x80 | (c & 0x3f), sb);
		return 3;
	}
	else if	(c <= 0x001FFFFF)
	{
		sb_putc(0xf0 | ((c >> 18) & 0x07), sb);
		sb_putc(0x80 | ((c >> 12) & 0x3f), sb);
		sb_putc(0x80 | ((c >> 6) & 0x3f), sb);
		sb_putc(0x80 | (c & 0x3f), sb);
		return 4;
	}
	else 	return 0;
}
