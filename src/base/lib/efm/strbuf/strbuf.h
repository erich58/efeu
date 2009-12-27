/*
:*:dymamic string buffer
:de:Dynamisches Zeichenfeld (Stringbuffer)

$Header	<EFEU/$1>

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

#ifndef	EFEU_strbuf_h
#define	EFEU_strbuf_h	1

#include <EFEU/memalloc.h>
#include <EFEU/stdint.h>

/*
:*:
The data structure |$1| provides a easy way to construct character
field. It has an read/write interface similiar to streams and
allows to insert/delete characters at any position (edit functions).
The size of the character field is exanded on demand.
:de:
Mithilfe der Datenstruktur |$1| kann auf einfache Weise eine
Zeichenkette aufgebaut werden. Es gibt ähnliche Schreib- und
Lesefunktionen wie für Dateien. Zusätzlich können auch
Zeichen innerhalb der Zeichenkette eingefügt oder gelöscht werden
(Editierfunktion).
Der Speicherbedarf der Zeichenkette
wird bei Bedarf automatisch vergrößert.

:*:
The string buffer |$1| contains the character field <data> of
size <size>. The componend <nfree> shows how many characters
could be stored with out expansion of the character, while
<pos> gives the aktual position in the character field.
On expansion, the size of the character field groes with the
amount of the component <blksize>.
If the value is zero, a starting value is used and the size
is doubled on each expansion.
:de:
Das dynamische Zeichenfeld |$1| enthält einen Zeichenvektor <data>
der Größe <size>. Die Komponente <nfree> gibt die Zahl der
freien Zeichen an, während <pos> die aktuelle Position
angibt. Die Variable <blksize> bestimmt, um wieviel der
Zeichenvektor vergrößert werden soll, wenn nicht mehr genug Platz
ist. Ist <blksize> auf 0 gesetzt, wird mit einem Startwert
begonnen und anschließend die aktuelle Größe immer verdoppelt.
*/

typedef struct {
	unsigned char *data;	/* Datenbuffer */
	unsigned size;	/* Aktuelle Größe des Buffers */
	unsigned blksize;	/* Blockgröße zur Buffervergrößerung */
	int nfree;	/* Zahl der freien Zeichen */
	int pos;	/* Aktuelle Bufferposition */
} StrBuf;

/*
:*:
The macro |$1| initializes the components of a string buffer
with blocksize <blk>.
:de:
Der Makro |$1| initialisiert die Strukturkomponenten des
Zeichenfeldes. Als Argument wird die Blockgröße benötigt.
*/

#define	SB_DATA(blk)	{ NULL, 0, blk, 0, 0 }

/*
:*:
The Mmacro |$1| declares the string buffer variable <name> and initializes
it with blocksize <blk>.
:de:
Der Makro |$1| deklariert das Zeichenfeld <name> und initialisiert
es mit der Blockgröße <blk>.
*/

#define	STRBUF(name, blk)	StrBuf name = SB_DATA(blk)
#define	SB_DECL(name, blk)	StrBuf name = SB_DATA(blk)

StrBuf *sb_create (size_t blksize);
void sb_destroy (StrBuf *sb);

void sb_init (StrBuf *buf, size_t blksize);

void *sb_expand (StrBuf *buf, size_t size);
char *sb_nul (StrBuf *sb);
void sb_trunc (StrBuf *sb);
void sb_free (StrBuf *sb);
void *sb_getmem (StrBuf *sb);

#define sb_clean(sb)	sb_trunc(sb)

int sb_get (StrBuf *sb);
int sb_put (int c, StrBuf *sb);
int sb_xputc(int c, StrBuf *sb, const char *delim);
int sb_xputs(const char *str, StrBuf *sb, const char *delim);
int sb_puts (const char *str, StrBuf *sb);
int sb_putmb (const char *str, StrBuf *sb);
int sb_nputs (const char *str, size_t len, StrBuf *sb);
int sb_rputs (const char *str, StrBuf *sb);
int sb_putstr (const char *str, StrBuf *sb);

int sb_printf (StrBuf *sb, const char *fmt, ...);
int sb_vprintf (StrBuf *sb, const char *fmt, va_list list);
int sb_xprintf (StrBuf *sb, const char *fmt, ...);
int sb_vxprintf (StrBuf *sb, const char *fmt, va_list list);
int sb_mbprintf (StrBuf *sb, const char *fmt, ...);
int sb_vmbprintf (StrBuf *sb, const char *fmt, va_list list);

int sb_setpos (StrBuf *buf, int n);
size_t sb_putmem (StrBuf *sb, const void *mem, size_t size);
int sb_move (StrBuf *sb, int tg, int src, int length);
int sb_swap (StrBuf *sb, int pos, int length);

int sb_nputc (int c, StrBuf *sb, int n);
int sb_putucs (int32_t c, StrBuf *sb);
int32_t sb_getucs (StrBuf *sb);

void *sb_insert (StrBuf *sb, int c, size_t n);
void *sb_delete (StrBuf *sb, size_t n);

StrBuf *sb_acquire (void);
void sb_release (StrBuf *buf);
char *sb_cpyrelease (StrBuf *buf);

/*
:de:Der Makro |$1| liefert einen Stringpointer auf die aktuelle Position.
Achtung: Der String ist nicht notwendigerweise 0-terminiert.
Vergleiche dazu auch \mref{sbuf_nul}.
*/

#define	sb_ptr(sb)	((char *) (sb)->data + (sb)->pos)


/*
:*:
The macro |$1| returns the nummber of valid characters stored in
the string buffer.
:de:
Der Makro |$1| liefert die Zahl der gültigen Zeichen des Zeichenfeldes.
*/

#define	sb_size(sb)	((sb)->size - (sb)->nfree)

/*
:*:
The macro |$1| returns the actual position in the string buffer.
:de:
Der Makro |$1| liefert die aktuelle Position im Zeichenfeld.
*/

#define	sb_getpos(sb)	((sb)->pos)

/*
:*:
The macro |$1| sets the actual position to the beginn of the string buffer.
:de:
Der Makro |$1| positionert auf den Anfang des Zeichenfeld.
*/

#define	sb_begin(sb)	((sb)->pos = 0)

/*
:*:
The macro |$1| sets the actual position to the end of the string buffer.
:de:
Der Makro |$1| positionert auf das Ende des Zeichenfelds.
*/

#define	sb_end(sb)	((sb)->pos = (sb)->size - (sb)->nfree)

/*
:*:
The macro |$1| sets the number of valid characters to the actual position.
:de:
Der Makro |$1| setzt die Größe des Zeichenfeldes auf
die aktuelle Position.
*/

#define	sb_sync(sb)	((sb)->nfree = (sb)->size - (sb)->pos)

/*
:*:
The macro |$1| reads a character form the string buffer <sb>.
If the end of valid characters is reachd, |EOF| is returned.
:de:
Der Makro |$1| liest ein Zeichen aus dem Zeichenfeld <sb>.
Wurde das Ende des Zeichenfeldes erreicht, liefert er EOF.
*/

#define	sb_getc(sb)	((sb)->pos < sb_size(sb) ? \
	(int) (sb)->data[(sb)->pos++] : EOF)

/*
:*:
The macro |$1| writes the character <c> to string buffer <sb>
und returns the code of the character.
:de:
Der Makro |$1| schreibt das Zeichen <c>
in das Zeichenfeld <sb> und liefert das geschriebene Zeichen als
Rückgabewert. 
*/

#define	sb_putc(c, sb)	(--(sb)->nfree < 0 ? \
	sb_put(c, sb) : (int) ((sb)->data[(sb)->pos++] = (unsigned char) (c)))

/*
:*:
The macro |$1| worry about a correct allignment of the valid
characters to a whole-numbered multiple of <m>. If nessesary,
'\0'-characters are inserted at end of the character field.
:de:
Der Makro |$1| sorgt für eine Ausrichtung der Zeichenfeldgröße
auf ein ganzzahliges Vielfaches von <m>. Bei Bedarf werden
am Ende 0-Zeichen eingefügt.
*/

#define	sb_align(sb, m)	while ((sb_size(sb) % (m)) != 0) sb_putc(0, sb)

char *sb_nul (StrBuf *sb);
char *sb_strcpy (StrBuf *sb);
char *sb_memcpy (StrBuf *sb);
char *lexsortkey (const char *base, StrBuf *buf);
char *sb_read (int fd, StrBuf *sb);

/*
$Warning
:*:
The macros evaluates <sb> more than once and may produce side
effects if terms are inserted for <sb>.
:de:
Die Makros werten den Ausdruck <sb> mehrfach aus.
Damit es zu keinen Seiteneffekten kommt, sollten hier keine
komplexen Terme eingesetzt werden, insbesonders dürfen
Increment oder Decrementoperatoren nicht verwendet werden.
So ist das Resultat des Ausdruck |sb_getc(buf++)| undefiniert. 
Ein Nullpointer ist ebenfalls nicht zulässig.

:*:
On writing, it is assumed, that the position points to the end of the
character field. On reading or editing, the position is uncoupled
from the end of the character field. So writing after reading or editing
needs a synchronisation of the position either with |sb_clean|, |sb_sync|
or |sb_end|.
:de:
Beim Schreiben wird vorrausgesetzt, daß der Positionszeiger am Ende des
Zeichenfeldes steht. Beim Lesen oder Editieren wird der Positionszeiger
vom Ende des Zeichenfeldes abgekoppelt.
Soll nach einem Lese- oder Editiervorgang wieder geschrieben werden,
muß daher entweder |sb_clean| (Anfang) |sb_sync| (aktuelle Position) oder
|sb_end| (Ende) aufgerufen werden.

$SeeAlso
\mref{strbuf(3)}, \mref{sb_getc(3)}, \mref{sb_putc(3)}.
*/

#endif	/* EFEU/strbuf.h */
