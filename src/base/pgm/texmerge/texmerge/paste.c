/*	Filenamen generieren
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#include "texmerge.h"

char *PasteString (const char *delim, const char *s1, const char *s2)
{
	Buffer_t *buf;

	buf = CreateBuffer();
	buf_puts(s1, buf);

	if	(s1 && s2)
		buf_puts(delim, buf);

	buf_puts(s2, buf);
	return CloseBuffer(buf);
}
