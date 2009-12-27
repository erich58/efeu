/*
Dynamisches Zeichenfeld (Stringbuffer)

$Header	<EFEU/$1>

$Copyright (C) 1996 Erich Fr�hst�ck
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

#ifndef	EFEU_strbuf_h
#define	EFEU_strbuf_h	1

#include <EFEU/memalloc.h>

/*
:de:
Mithilfe der Datenstruktur |$1| kann auf einfache Weise eine
Zeichenkette aufgebaut werden. Es gibt �hnliche Schreib- und
Lesefunktionen wie f�r Dateien. Zus�tzlich k�nnen auch
Zeichen innerhalb der Zeicheinkette eingef�gt oder gel�scht werden
(Editierfunktion).
Der Speicherbedarf der Zeichenkette
wird bei Bedarf automatisch vergr��ert.
\par
Das dynamische Zeichenfeld |$1| enth�lt einen Zeichenvektor <data>
der Gr��e <size>. Die Komponente <nfree> gibt die Zahl der
freien Zeichen an, w�hrend <pos> die aktuelle Position
angibt. Die Variable <blksize> bestimmt, um wieviel der
Zeichenvektor vergr��ert werden soll, wenn nicht mehr genug Platz
ist. Ist <blksize> auf 0 gesetzt, werden Standardvorgaben verwendet.
*/

typedef struct {
	unsigned char *data;	/* Datenbuffer */
	unsigned size;	/* Aktuelle Gr��e des Buffers */
	unsigned blksize;	/* Blockgr��e zur Buffervergr��erung */
	int nfree;	/* Zahl der freien Zeichen */
	int pos;	/* Aktuelle Bufferposition */
} StrBuf;

/*
:de:
Der Makro |$1| initialisiert die Strukturkomponenten des
Zeichenfeldes. Als Argument wird die Blockgr��e ben�tigt.
*/

#define	SB_DATA(blk)		{ NULL, 0, blk, 0, 0 }

/*
:de:
Der Makro |$1| deklariert das Zeichenfeld <name> und initialisiert
es mit der Blockgr��e <blk>.
*/

#define	STRBUF(name, blk)	StrBuf name = SB_DATA(blk)

extern StrBuf *new_strbuf (size_t blksize);
extern int del_strbuf (StrBuf *buf);
extern void sb_expand (StrBuf *buf);
extern void sb_clear (StrBuf *sb);

extern int sb_get (StrBuf *sb);
extern int sb_put (int c, StrBuf *sb);
extern int sb_puts (const char *str, StrBuf *sb);
extern int sb_rputs (const char *str, StrBuf *sb);
extern int sb_putstr (const char *str, StrBuf *sb);
extern int sb_printf (StrBuf *sb, const char *fmt, ...);
extern int sb_vprintf (StrBuf *sb, const char *fmt, va_list list);
extern int sb_setpos (StrBuf *buf, int n);

void sb_insert (int c, StrBuf *sb);
int sb_delete (StrBuf *sb);

/*
:de:
Der Makro |$1| liefert die aktuelle Gr��e des Zeichenfeldes.
*/

#define	sb_size(sb)	((sb)->size - (sb)->nfree)

/*
:de:
Der Makro |$1| liefert die aktuelle Position im Zeichenfeld.
*/

#define	sb_getpos(sb)	((sb)->pos)

/*
:de:
Der Makro |$1| positionert auf den Anfang des Zeichenfeld.
*/

#define	sb_begin(sb)	((sb)->pos = 0)

/*
:de:
Der Makro |$1| positionert auf das Ende des Zeichenfelds.
*/

#define	sb_end(sb)	((sb)->pos = (sb)->size - (sb)->nfree)

/*
:de:
Der Makro |$1| setzt die Gr��e des Zeichenfeldes auf
die aktuelle Position.
*/

#define	sb_sync(sb)	((sb)->nfree = (sb)->size - (sb)->pos)

/*
:de:
Der Makro |$1| liest ein Zeichen aus dem Zeichenfeld <sb>.
Wurde das Ende des Zeichenfeldes erreicht, liefert er EOF.
*/

#define	sb_getc(sb)	((sb)->pos < sb_size(sb) ? \
	(int) (sb)->data[(sb)->pos++] : EOF)

/*
:de:
Der Makro |$1| schreibt das Zeichen <c>
in das Zeichenfeld <sb> und liefert das geschriebene Zeichen als
R�ckgabewert. 
*/

#define	sb_putc(c, sb)	(--(sb)->nfree < 0 ? \
	sb_put(c, sb) : (int) ((sb)->data[(sb)->pos++] = (unsigned char) (c)))

/*
:de:
Der Makro |$1| sorgt f�r eine Ausrichtung der Zeichenfeldgr��e
auf ein ganzzahliges Vielfaches von <m>. Bei Bedarf werden
am Ende 0-Zeichen eingef�gt.
*/

#define	sb_align(sb, m)	while ((sb_size(sb) % (m)) != 0) sb_putc(0, sb)

extern char *sb2str (StrBuf *sb);
extern char *sb2mem (StrBuf *sb);
extern char *sb_strcpy (StrBuf *sb);
extern char *sb_memcpy (StrBuf *sb);
extern char *lexsortkey (const char *base, StrBuf *buf);

/*
:de:
$Warning
Die Makros werten den Ausdruck <sb> mehrfach aus.
Damit es zu keinen Seiteneffekten kommt, sollten hier keine
komplexen Terme eingesetzt werden, insbesonders d�rfen
Increment oder Decrementoperatoren nicht verwendet werden.
So ist das Resultat des Ausdruck |sb_getc(buf++)| undefiniert. 
Ein Nullpointer ist ebenfalls nicht zul�ssig.
\par
Beim Schreiben wird vorrausgesetzt, da� der Positionszeiger am Ende des
Zeichenfeldes steht. Beim Lesen oder Editieren wird der Positionszeiger
vom Ende des Zeichenfeldes abgekoppelt.
Soll nach einem Lese- oder Editiervorgang wieder geschrieben werden,
mu� daher entweder |sb_end| oder |sb_sync| aufgerufen werden.

$SeeAlso
\mref{strbuf(3)}, \mref{sb_getc(3)}, \mref{sb_putc(3)}.\br
*/

#endif	/* EFEU/strbuf.h */
