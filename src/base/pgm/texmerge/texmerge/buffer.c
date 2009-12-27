/*	Dynamischer Zeichenbuffer
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.6
*/

#include "texmerge.h"

#define	BUF_SIZE	128


/*	Zeichenbuffer generieren/freigeben
*/

Buffer_t *CreateBuffer (void)
{
	Buffer_t *buf;

	buf = AllocData("BUFFER", sizeof(Buffer_t));
	buf->data = NULL;
	buf->size = 0;
	buf->pos = 0;
	return buf;
}


/*	String aus Stringbuffer extrahieren
*/

char *CloseBuffer(Buffer_t *buf)
{
	char *data;

	if	(buf->pos != 0)
	{
		buf_putc(0, buf);
		data = buf->data;
	}
	else	data = NULL;

	FreeData("BUFFER", buf);
	return data;
}

/*	Zeichen in Buffer schreiben
*/

void buf_putc(int c, Buffer_t *buf)
{
	if	(buf == NULL || c == EOF)	return;

	if	(buf->pos >= buf->size)
	{
		size_t size;
		char *data;

		size = buf->size + BUF_SIZE;
		data = AllocData("STR", size);

		if	(buf->size)
		{
			memcpy(data, buf->data, buf->size);
			FreeData("STR", buf->data);
		}

		buf->size = size;
		buf->data = data;
	}

	buf->data[buf->pos++] = c;
}


/*	String in Stringbuffer schreiben
*/

void buf_puts(const char *str, Buffer_t *buf)
{
	if	(str == NULL || buf == NULL)
		return;

	for (; *str != 0; str++)
		buf_putc(*str, buf);
}
