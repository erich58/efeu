/*
Arbeiten mit dynamischen Zeichenfeldern

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

#define	SB_SIZE	1024

static ALLOCTAB(sb_tab, 32, sizeof(strbuf_t));

/*
Die Funktion |$1| liefert ein neues Zeichenfeld
mit Buffergröße <blksize>.
*/

strbuf_t *new_strbuf (size_t blksize)
{
	strbuf_t *buf = new_data(&sb_tab);
	buf->blksize = blksize;
	return buf;
}

/*
Die Funktion |$1| gibt ein mit |new_strbuf| angefordertes Zeichenfeld
wieder frei.
*/

int del_strbuf (strbuf_t *buf)
{
	if	(buf)
	{
		lfree(buf->data);
		return (del_data(&sb_tab, buf) ? 0 : EOF);
	}
	else	return 0;
}


/*
Die Funktion |$1| vergrößert ein Zeichenfeld um die Blockgröße.
*/

void sb_expand (strbuf_t *buf)
{
	if	(buf)
	{
		uchar_t *p;
		size_t n;

		n = (buf->blksize ? buf->blksize : SB_SIZE);
		p = lmalloc((ulong_t) buf->size + n);
		memcpy(p, buf->data, buf->size);
		lfree(buf->data);
		buf->data = p;
		buf->size += n;
		buf->nfree += n;
	}
}


/*
Die Funktion |$1| setzt die Größe des Zeichenfeldes auf 0
und gibt den Speicher frei.
*/

void sb_clear (strbuf_t *sb)
{
	if	(sb)
	{
		sb->pos = 0;
		sb->nfree = sb->size;
	}
}

/*
Die Funktion |$1| liest ein Zeichen aus dem Zeichenfeld <sb>.
Wurde das Ende des Zeichenfeldes erreicht, liefert er EOF.
Sie ist Funktionsgleich mit dem Makro |sb_getc| erlaubt aber
einen Nullpointer als Argument. In diesem Fall liefert sie immer EOF.
*/

int sb_get (strbuf_t *sb)
{
	return sb ? sb_getc(sb) : EOF;
}

/*
Die Funktion |$1| schreibt das Zeichen <c> in das Zeichenfeld <sb> und
liefert das geschriebene Zeichen als Rückgabewert. Das Zeichenfeld
wird bei Bedarf mit |sb_expand| vergrößert.
Gegenüber dem Makro |sb_putc| führt |$1| immer eine Synchronisation
der Zeichenfeldgröße mit der aktuellen Position durch.
*/

int sb_put (int c, strbuf_t *sb)
{
	if	(!sb)	return 0;

	if	(sb->pos >= sb->size)
		sb_expand(sb);

	c = sb->data[sb->pos++] = (uchar_t) c;
	sb->nfree = sb->size - sb->pos;
	return c;
}


/*
Die Funktion |$1| setzt die aktuelle Position des Stringbuffers <sb>
auf <pos>.
*/

int sb_setpos (strbuf_t *sb, int pos)
{
	if	(!sb)			return 0;
	else if	(pos <= 0)		sb->pos = 0;
	else if	(pos <= sb_size(sb))	sb->pos = pos;
	else				sb->pos = sb_size(sb);
	
	return sb->pos;
}


/*
Die Funktion |$1| schreibt den String <str> in das Zeichenfeld <sb>.
*/

int sb_puts (const char *str, strbuf_t *buf)
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
Die Funktion |$1| schreibt den String <str> in umgekehrter
Reihenfolge (das letzte Zeichen zuerst) in das Zeichenfeld <sb>.
*/

int sb_rputs (const char *str, strbuf_t *buf)
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
Die Funktion |$1| schreibt den String <str> mit dem abschließenden
0-Zeichen in das Zeichenfeld <sb>.
*/

int sb_putstr (const char *str, strbuf_t *buf)
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
Die Funktion |$1| liefert eine Stringkopie (0-Abschluß) des Zeichenfeldes.
*/

char *sb_strcpy (strbuf_t *buf)
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
Die Funktion |$1| gibt das Zeichenfeld <sb> frei und liefert
eine Stringkopie (0-Abschluß).
*/

char *sb2str (strbuf_t *buf)
{
	if	(buf)
	{
		char *p = sb_strcpy(buf);
		del_strbuf(buf);
		return p;
	}

	return NULL;
}

/*
Die Funktion |$1| liefert eine Speicherkopie des Zeichenfeldes.
*/

char *sb_memcpy (strbuf_t *buf)
{
	register int n = buf ? sb_getpos(buf) : 0;
	return n ? memcpy(memalloc(n), buf->data, n) : NULL;
}

/*
Die Funktion |$1| gibt das Zeichenfeld <sb> frei und liefert
eine Speicherkopie.
*/

char *sb2mem (strbuf_t *buf)
{
	if	(buf)
	{
		char *p = sb_memcpy(buf);
		del_strbuf(buf);
		return p;
	}

	return NULL;
}
