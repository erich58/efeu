/*	Dynamisches Zeichenfeld editieren
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.6
*/

#include <EFEU/strbuf.h>

/*
Die Funktion |$1| löscht das Zeichen an der aktuellen Position.
*/

int sb_delete (strbuf_t *sb)
{
	int i, c, n;

	if	(sb->nfree >= sb->size)
		return EOF;

	sb->nfree++;
	n = sb->size - sb->nfree;
	c = sb->data[sb->pos];

	for (i = sb->pos; i < n; i++)
		sb->data[i] = sb->data[i + 1];

	if	(sb->pos > n)	sb->pos = n;

	return c;
}

/*
Die Funktion |$1| fügt das Zeichen <c> an der aktuellen Position ein.
*/

void sb_insert (int c, strbuf_t *sb)
{
	int i;

	if	(sb->pos >= sb->size - sb->nfree)
	{
		sb_put(c, sb);
		return;
	}

	if	(--sb->nfree < 0)
		sb_expand(sb);

	for (i = sb->size - sb->nfree; i > sb->pos; i--)
		sb->data[i] = sb->data[i-1];

	sb->data[sb->pos++] = c;
}

/*
$SeeAlso
\mref{strbuf(3)}, \mref{sb_getc(3)}, \mref{sb_putc(3)}.\br
*/
