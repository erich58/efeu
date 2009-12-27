/*
Arbeiten mit dynamischen Zeichenfeldern
*/

#include <EFEU/strbuf.h>

/*
Die Funktion |$1| kopiert <size> Byte von <mem> in das
Zeichenfeld <sb>.
Das Speicherfeld <mem> darf nicht auf das Datenfeld im Stringbuffers verweisen.
*/

size_t sb_putmem (StrBuf *sb, const void *mem, size_t size)
{
	if	(sb && mem && size)
	{
		memcpy(sb_expand(sb, size), mem, size);
		sb->pos += size;
		sb->nfree -= size;
		return size;
	}

	return 0;
}

/*
$SeeAlso
\mref{strbuf(7)}.
*/
