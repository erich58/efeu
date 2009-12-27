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

static ALLOCTAB(sb_tab, 32, sizeof(StrBuf));

static char *ref_ident (const void *ptr)
{
	return sb_strcpy((StrBuf *) ptr);
}

static void ref_clean (void *ptr)
{
	sb_free(ptr);
	del_data(&sb_tab, ptr);
}

RefType sb_reftype = REFTYPE_INIT("StrBuf", ref_ident, ref_clean);

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
:*:
The function |$1| sets the actual size of the string buffer to zero
without freeing any memory used by the buffer.
:de:
Die Funktion |$1| setzt die Größe des Zeichenfeldes auf 0 ohne
Speicherbereich freizugeben.
*/

void sb_clean (StrBuf *sb)
{
	if	(sb)
	{
		sb->pos = 0;
		sb->nfree = sb->size;
	}
}

/*
:*:
The function |$1| creates a new string buffer with blocksize <bsize>.
:de:
Die Funktion |$1| liefert ein neues Zeichenfeld mit Buffergröße <blksize>.
*/

StrBuf *sb_create (size_t blksize)
{
	StrBuf *buf = new_data(&sb_tab);
	sb_init(buf, blksize);
	rd_init(&sb_reftype, buf);
	return buf;
}

/*
:*:
The function |$1| expands the memory field used to store date in the
string buffer.
:de:
Die Funktion |$1| vergrößert das Zeichenfeld entsprechend der Blockgröße.
*/

void sb_expand (StrBuf *buf)
{
	if	(buf)
	{
		size_t n;

		if	(buf->blksize)	n = buf->blksize;
		else if	(buf->size)	n = buf->size;
		else			n = SB_SIZE;

		buf->size += n;
		buf->nfree += n;
		buf->data = lrealloc(buf->data, buf->size);
	}
}


/*
:*:
The function |$1| reads the next character from the string field <sb>.
If the end of previously written data is reached, |EOF| is returned.
:de:
Die Funktion |$1| liest ein Zeichen aus dem Zeichenfeld <sb>.
Wurde das Ende des Zeichenfeldes erreicht, liefert er EOF.
Sie ist Funktionsgleich mit dem Makro |sb_getc| erlaubt aber
einen Nullpointer als Argument. In diesem Fall liefert sie immer EOF.
*/

int sb_get (StrBuf *sb)
{
	return sb ? sb_getc(sb) : EOF;
}

/*
:*:
The function |$1| writes the character <c> in the string buffer <sb>
and returns the character value. If nessesary, the string buffer
is expandet. In opposite to the macro |sb_putc| the position in the
string buffer is always synchronized.
:de:
Die Funktion |$1| schreibt das Zeichen <c> in das Zeichenfeld <sb> und
liefert das geschriebene Zeichen als Rückgabewert. Das Zeichenfeld
wird bei Bedarf mit |sb_expand| vergrößert.
Gegenüber dem Makro |sb_putc| führt |$1| immer eine Synchronisation
der Zeichenfeldgröße mit der aktuellen Position durch.
*/

int sb_put (int c, StrBuf *sb)
{
	if	(c == EOF || sb == NULL)
		return EOF;

	if	(sb->pos >= sb->size)
		sb_expand(sb);

	c = sb->data[sb->pos++] = (unsigned char) c;
	sb->nfree = sb->size - sb->pos;
	return c;
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
The function |$1| writes the string <str> to the string buffer <sb>.
:de:
Die Funktion |$1| schreibt den String <str> in das Zeichenfeld <sb>.
*/

int sb_puts (const char *str, StrBuf *buf)
{
	if	(str && buf)
	{
		register int n;

		for (n = 0; *str != 0; n++, str++)
			sb_putc(*str, buf);

		return n;
	}
	else	return 0;
}

/*
:*:
The function |$1| writes the string <str> in reverse order
to the string buffer <sb>.
:de:
Die Funktion |$1| schreibt den String <str> in umgekehrter
Reihenfolge (das letzte Zeichen zuerst) in das Zeichenfeld <sb>.
*/

int sb_rputs (const char *str, StrBuf *buf)
{
	if	(str && buf)
	{
		register int n = strlen(str);
		register int k = n;

		while (k-- > 0)
			sb_putc(str[k], buf);

		return n;
	}
	else	return 0;
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

/*
:*:
The function |$1| returns the adresss of the string buffer at position
<pos> as string. The return value is only a valid string,
if a '\0'-character is written after the position <pos>.
:de:
Die Funktion |$1| liefert die Adresse des Zeichenfelds bei Position <pos>
als String. Der Rückgabewert ist nur dann ein gültiger String, wenn ein
0-Zeichen nach der Position <pos> geschrieben wurde.
*/

char *sb_str (StrBuf *buf, int pos)
{
	if	(buf && pos < buf->pos)
	{
		return (char *) buf->data + pos;
	}
	else	return NULL;
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
The function |$1| converts the string buffer to a string.
:de:
Die Funktion |$1| konvertiert einen Strungbuffer in einen String mit
0-Abschluß.
*/

char *sb2str (StrBuf *buf)
{
	if	(buf)
	{
		char *p = sb_strcpy(buf);
		rd_deref(buf);
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
:*:
The function |$1| converts the string buffer to a character field
without '\0'-termination.
:de:
Die Funktion |$1| konvertiert einen Strungbuffer in ein Zeichenfels
ohne 0-Abschluß.
*/

char *sb2mem (StrBuf *buf)
{
	if	(buf)
	{
		char *p = sb_memcpy(buf);
		rd_deref(buf);
		return p;
	}

	return NULL;
}

/*
$SeeAlso
\mref{strbuf(7)},
\mref{memalloc(7)}.
*/
