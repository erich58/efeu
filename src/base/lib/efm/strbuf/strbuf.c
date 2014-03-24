/*
:*:	Dynamic string buffer
:de:	Arbeiten mit dynamischen Zeichenfeldern

$Copyright (C) 1996 Erich Frühstück
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

#include <EFEU/strbuf.h>

#define	SB_SIZE	1000

/*
:*:
The Function |$1| initializes the dynamic string with the blocksize <bsize>.
:de:
Die Funktion |$1| initialisiert den Stringbuffer <sb>
mit der Blockgröße <bsize>.
*/

void sb_init (StrBuf *sb, size_t bsize)
{
	if	(sb)
	{
		sb->data = NULL;
		sb->size = 0;
		sb->blksize = bsize;
		sb->nfree = 0;
		sb->pos = 0;
	}
}

/*
:*:
The function |$1| sets the actual size of the string buffer <sb> to zero
and frees the memory used by the string buffer <sb>.
:de:
Die Funktion |$1| setzt die Größe des Stringbuffers auf 0 und
gibt das vom Stringbuffer verwendete Speicherfeld frei.
*/

void sb_free (StrBuf *sb)
{
	if	(sb)
	{
		lfree(sb->data);
		sb->data = NULL;
		sb->size = 0;
		sb->nfree = 0;
		sb->pos = 0;
	}
}

/*
:de:Die Funktion |$1| liefert das aktuell im Stringbuffer verwendete
Speicherfeld. Dieses kann später mit |memfree| freigegeben werden. Der
Zustand des Stringbuffers ist gleich wie nach einem Aufruf von |sb_free|.
*/

void *sb_getmem (StrBuf *sb)
{
	if	(sb)
	{
		void *p = sb->data;
		sb->data = NULL;
		sb->size = 0;
		sb->nfree = 0;
		sb->pos = 0;
		memnotice(p);
		return p;
	}

	return NULL;
}

/*
:*:
The function |$1| sets the actual size of the string buffer to zero
without freeing any memory used by the buffer.
:de:
Die Funktion |$1| setzt die Größe des Zeichenfeldes auf 0 ohne
Speicherbereich freizugeben.
*/

void sb_trunc (StrBuf *sb)
{
	if	(sb)
	{
		sb->pos = 0;
		sb->nfree = sb->size;
	}
}


/*
:*:
The function |$1| expands the memory field used to store date in the
string buffer.
:de:
Die Funktion |$1| vergrößert das Zeichenfeld entsprechend der Blockgröße.
*/

void *sb_expand (StrBuf *buf, size_t size)
{
	if	(!buf)	return NULL;

	buf->nfree = buf->size - buf->pos;

	if	(buf->nfree < size)
	{
		void *data;

		size += buf->pos;

		if	(!buf->blksize)
			buf->blksize = SB_SIZE;

		size = buf->size + sizealign(size - buf->size, buf->blksize);

		if	(buf->size)
			buf->blksize = buf->size;
#ifdef	SB_REALLOC
		data = SB_REALLOC(buf->data, size);
#else
		data = lrealloc(buf->data, size);
#endif
		if	(data)
		{
			buf->data = data;
			buf->size = size;
			buf->nfree = buf->size - buf->pos;
		}
		else	return NULL;
	}

	return buf->data + buf->pos;
}


/*
:*:
The function |$1| sets the actual position of the string buffer <sb>
to <pos>.
:de:
Die Funktion |$1| setzt die aktuelle Position des Stringbuffers <sb>
auf <pos>.
*/

int sb_setpos (StrBuf *sb, int pos)
{
	if	(!sb)			return 0;
	else if	(pos <= 0)		sb->pos = 0;
	else if	(pos <= sb_size(sb))	sb->pos = pos;
	else				sb->pos = sb_size(sb);
	
	return sb->pos;
}


/*
:*:
The function |$1| writes the string <str> with terminating '\0'
to the string buffer <sb>.
:de:
Die Funktion |$1| schreibt den String <str> mit dem abschließenden
0-Zeichen in das Zeichenfeld <sb>.
*/

int sb_putstr (const char *str, StrBuf *buf)
{
	if	(buf)
	{
		int n = sb_puts(str, buf);
		sb_putc(0, buf);
		return n + 1;
	}
	else	return 0;
}

char *sb_nul (StrBuf *buf)
{
	if	(buf)
	{
		sb_putc(0, buf);
		buf->pos--;
		sb_sync(buf);
		return (char *) buf->data;
	}

	return NULL;
}


/*
:*:
The function |$1| returns a '\0'-terminated string copy of the characters
stored in the string buffer. If no characters are stored, a Nullpointer is
returned.
:de:
Die Funktion |$1| liefert eine Stringkopie (0-Abschluß) des Zeichenfeldes.
Falls kein Zeichen gespeichert wurde, liefert die Funktion einen Nullpointer.
*/

char *sb_strcpy (StrBuf *buf)
{
	register int n = buf ? sb_getpos(buf) : 0;

	if	(n != 0)
	{
		char *p = memalloc(n + 1);
		memcpy(p, buf->data, n);
		p[n] = 0;
		return p;
	}

	return NULL;
}


/*
:*:
The function |$1| returns ia copy of the characters stored in the string
without '\0'-termination.
:de:
Die Funktion |$1| liefert eine Kopie der Zeichen im Stringbuffer
ohne 0-Abschluß.
*/

char *sb_memcpy (StrBuf *buf)
{
	register int n = buf ? sb_getpos(buf) : 0;
	return n ? memcpy(memalloc(n), buf->data, n) : NULL;
}


/*
$SeeAlso
\mref{strbuf(7)},
\mref{memalloc(7)}.
*/
