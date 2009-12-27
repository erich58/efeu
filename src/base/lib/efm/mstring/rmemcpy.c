/*	String in reverser Ordnung kopieren
	(c) 1997 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/mstring.h>

void *rmemcpy(void *tp, const void *sp, size_t size)
{
	register uchar_t *tg = tp;
	register const uchar_t *src = sp;

	src += size - 1;

	while (size-- > 0)
		*(tg++) = *(src--);

	return tp;
}
