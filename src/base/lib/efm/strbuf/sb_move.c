/*
Datenblock im Stringbuffer verschieben
*/

#include <EFEU/strbuf.h>

#define	TSIZE	512	/* Zwischenbuffergröße */

static size_t do_move (unsigned char *dest, unsigned char *src, size_t size);

/*
Die Funktion |$1| verschiebt <length> Byte von der Position <src> zur
Position <dest> nach vorne. Die Zielposition muss kleiner als die
Ausgangsposition sein.  Falls <length> den Wert 0 hat, wird alles im
Zeichenbuffer ab der Position <src> nach <dest> geschoben.  Die Größe
des Buffers wird dabei nicht verändert.  Die Funktion liefert die Zahl
der verschoben Zeichen.
*/

int sb_move (StrBuf *sb, int dest, int src, int length)
{
	int n;

	if	(!sb)	return 0;

	if	(dest >= src)	return 0;

	n = sb_size(sb);

	if	(src >= n)	return 0;

	if	(length == 0 || length > n)
		length = n;

	n = 0;

	while (length > TSIZE)
	{
		n += do_move(sb->data + dest, sb->data + src, TSIZE);
		dest += TSIZE;
		src += TSIZE;
		length -= TSIZE;
	}

	n += do_move(sb->data + dest, sb->data + src, length);
	return n;
}

static size_t do_move (unsigned char *dest, unsigned char *src, size_t size)
{
	unsigned char buf[TSIZE];
	memcpy(buf, src, size);
	memmove(dest + size, dest, src - dest);
	memcpy(dest, buf, size);
	return size;
}

/*
$SeeAlso
\mref{strbuf(3)},
\mref{strbuf(7)}.
*/
