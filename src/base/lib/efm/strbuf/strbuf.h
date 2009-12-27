/*	Dynamisches Zeichenfeld (Stringbuffer)
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.6

$Header	<EFEU/$1>
$SeeAlso
\mref{strbuf(3)}, \mref{sb_getc(3)}, \mref{sb_putc(3)}.\br
*/

#ifndef	EFEU_strbuf_h
#define	EFEU_strbuf_h	1

#include <EFEU/memalloc.h>

/*
Mithilfe der Datenstruktur |$1| kann auf einfache Weise eine
Zeichenkette aufgebaut werden. Es gibt ähnliche Schreib- und
Lesefunktionen wie für Dateien. Zusätzlich können auch
Zeichen innerhalb der Zeicheinkette eingefügt oder geloscht werden
(Editierfunktion).
Der Speicherbedarf der Zeichenkette
wird bei Bedarf automatisch vergrößert.

Das dynamische Zeichenfeld |$1| enthält einen Zeichenvektor <data>
der Größe <size>. Die Komponente <nfree> gibt die Zahl der
freien Zeichen an, während <pos> die aktuelle Position
angibt. Die Variable <blksize> bestimmt, um wieviel der
Zeichenvektor vergrößert werden soll, wenn nicht mehr genug Platz
ist. Ist <blksize> auf 0 gesetzt, werden Standardvorgaben verwendet.
*/

typedef struct {
	uchar_t *data;	/* Datenbuffer */
	unsigned size;	/* Aktuelle Größe des Buffers */
	unsigned blksize;	/* Blockgröße zur Buffervergrößerung */
	int nfree;	/* Zahl der freien Zeichen */
	int pos;	/* Aktuelle Bufferposition */
} strbuf_t;

/*
Der Makro |$1| initialisiert die Strukturkomponenten des
Zeichenfeldes. Als Argument wird die Blockgröße benötigt.
*/

#define	SB_DATA(blk)		{ NULL, 0, blk, 0, 0 }

/*
Der Makro |$1| deklariert das Zeichenfeld <name> und initialisiert
es mit der Blockgröße <blk>.
*/

#define	STRBUF(name, blk)	strbuf_t name = SB_DATA(blk)

extern strbuf_t *new_strbuf (size_t blksize);
extern int del_strbuf (strbuf_t *buf);
extern void sb_expand (strbuf_t *buf);
extern void sb_clear (strbuf_t *sb);

extern int sb_get (strbuf_t *sb);
extern int sb_put (int c, strbuf_t *sb);
extern int sb_puts (const char *str, strbuf_t *sb);
extern int sb_rputs (const char *str, strbuf_t *sb);
extern int sb_putstr (const char *str, strbuf_t *sb);
extern int sb_printf (strbuf_t *sb, const char *fmt, ...);
extern int sb_vprintf (strbuf_t *sb, const char *fmt, va_list list);
extern int sb_setpos (strbuf_t *buf, int n);

void sb_insert (int c, strbuf_t *sb);
int sb_delete (strbuf_t *sb);

/*	Der Makro |$1| liefert die aktuelle Größe des Zeichenfeldes.
*/

#define	sb_size(sb)	((sb)->size - (sb)->nfree)

/*	Der Makro |$1| liefert die aktuelle Position im Zeichenfeld.
*/

#define	sb_getpos(sb)	((sb)->pos)

/*	Der Makro |$1| positionert auf den Anfang des Zeichenfeld.
*/

#define	sb_begin(sb)	((sb)->pos = 0)

/*	Der Makro |$1| positionert auf das Ende des Zeichenfelds.
*/

#define	sb_end(sb)	((sb)->pos = (sb)->size - (sb)->nfree)

/*	Der Makro |$1| setzt die Größe des Zeichenfeldes auf
	die aktuelle Position.
*/

#define	sb_sync(sb)	((sb)->nfree = (sb)->size - (sb)->pos)

/*
Der Makro |$1| liest ein Zeichen aus dem Zeichenfeld <sb>.
Wurde das Ende des Zeichenfeldes erreicht, liefert er EOF.
*/

#define	sb_getc(sb)	((sb)->pos < sb_size(sb) ? \
	(int) (sb)->data[(sb)->pos++] : EOF)

/*
Der Makro |$1| schreibt das Zeichen <c>
in das Zeichenfeld <sb> und liefert das geschriebene Zeichen als
Rückgabewert. 
*/

#define	sb_putc(c, sb)	(--(sb)->nfree < 0 ? \
	sb_put(c, sb) : (int) ((sb)->data[(sb)->pos++] = (uchar_t) (c)))

/*
Der Makro |$1| sorgt für eine Ausrichtung der Zeichenfeldgröße
auf ein ganzzahliges Vielfaches von <m>. Bei Bedarf werden
am Ende 0-Zeichen eingefügt.
*/

#define	sb_align(sb, m)	while ((sb_size(sb) % (m)) != 0) sb_putc(0, sb)

extern char *sb2str (strbuf_t *sb);
extern char *sb2mem (strbuf_t *sb);
extern char *sb_strcpy (strbuf_t *sb);
extern char *sb_memcpy (strbuf_t *sb);
extern char *lexsortkey (const char *base, strbuf_t *buf);

/*
$Warning
Die Makros werten den Ausdruck <sb> mehrfach aus.
Damit es zu keinen Seiteneffekten kommt, sollten hier keine
komplexe Terme eingesetzt werden, insbesonders dürfen
Increment oder Decrementoperatoren nicht verwendet werden.
So ist das Resultat des Ausdruck |sb_getc(buf++)| undefiniert. 
Ein Nullpointer ist ebenfalls nicht zulässig.

Beim Schreiben wird vorrausgesetzt, daß der Positionszeiger am Ende des
Zeichenfeldes steht. Beim Lesen oder Editieren wird der Positionszeiger
vom Ende des Zeichenfeldes abgekoppelt.
Soll nach einem Lese- oder Editiervorgang wieder geschrieben werden,
muß daher entweder |sb_end| oder |sb_sync| aufgerufen werden.
*/

#endif	/* EFEU/strbuf.h */
