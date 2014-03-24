/*
Dynamischer Vektorbuffer

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

#ifndef	EFEU_vecbuf_h
#define	EFEU_vecbuf_h	1

#include <EFEU/config.h>

/*
Die Datenstruktur |$1| dient zur Verwaltung eines Datenfeldes.
Dabei ist |data| ein Pointer auf ein Speicherfeld, daß |size|
Elemente der Größe |elsize| enthält. Die Komponente |used|
gibt an, wieviele Elemente gerade benützt werden. Falls
|blksize| verschieden von null ist, ist der Buffer dynamisch und
wird bei Bedarf um |blksize| Elemente vergrößert.
*/

typedef struct {
	size_t blksize;	/* Blockgröße zur Buffervergrößerung */
	size_t elsize;	/* Größe eines Datenelementes */
	size_t size;	/* Aktuelle Größe des Buffers */
	size_t used;	/* Zahl der benützten Elemente */
	void *data;	/* Datenbuffer */
} VecBuf;


/*
Der Makro |$1| liefert die Strukturkomponenten eines Vektorbuffers
mit Blockgröße <blk> und Datengröße <size>. Er wird bei der Initialisierung
von Variablen verwendet, die einen Vektorbuffer als Komponente enthalten.
*/

#define	VB_DATA(blk, size)	{ blk, size, 0, 0, NULL }

/*
Der Makro |$1| deklariert den Vektorbuffer <name> und initialisiert
ihn mit der Blockgröße <blk> und der Datengröße <size>.

$Examples
Die nachfolgende Befehlszeile richtet einen statischen Vektorbuffer
mit Namen |buf| vom Type |int| ein:

---- verbatim
static VECBUF(buf, 1024, sizeof(int));
----
*/

#define	VECBUF(name, blk, size)	VecBuf name = VB_DATA(blk, size)

VecBuf *vb_create (size_t blk, size_t size);
void vb_destroy (VecBuf *vb);

void vb_init (VecBuf *buf, size_t blk, size_t size);
void *_vb_alloc (VecBuf *buf, size_t dim);
void *vb_alloc (VecBuf *buf, size_t dim);
void vb_free (VecBuf *buf);
void vb_clean (VecBuf *vb, void (*clean) (void *ptr));
void *vb_realloc (VecBuf *buf, size_t dim);
void *vb_next (VecBuf *buf);
void *vb_data (VecBuf *buf, size_t pos);
void *vb_copy (VecBuf *buf, void *data, size_t dim);
void *vb_append (VecBuf *buf, void *data, size_t dim);
void *vb_fetch (VecBuf *buf, void *data);
void *vb_insert (VecBuf *buf, size_t pos, size_t dim);
void *vb_delete (VecBuf *buf, size_t pos, size_t dim);
void *vb_load (VecBuf *buf, FILE *file, size_t dim);

void vb_qsort (VecBuf *buf, int (*comp) (const void *a, const void *b));
void vb_uniq (VecBuf *buf, int (*comp) (const void *a, const void *b));
void *vb_bsearch (VecBuf *buf, const void *key,
	int (*comp) (const void *a, const void *b));

#define	VB_ENTER	0	/* Eintrag abfragen/ergänzen */
#define	VB_REPLACE	1	/* Eintrag ersetzen/ergänzen */
#define	VB_SEARCH	2	/* Eintrag abfragen */
#define	VB_DELETE	3	/* Eintrag aus Tabelle löschen */

void *vb_search (VecBuf *buf, void *data,
	int (*comp) (const void *a, const void *b), int mode);

/*	Hilfsfunktionen
*/

size_t vsplit (void *base, size_t dim, size_t size,
	int (*test) (const void *par));
size_t svsplit (void *base, size_t dim, size_t size,
	int (*test) (const void *par));
size_t vuniq (void *base, size_t dim, size_t size,
	int (*comp) (const void *a, const void *b));

/*	Indexbereich bestimmen
*/

typedef struct {
	size_t start;
	size_t end;
} VecMatch;

VecMatch vmatch (void *key, void *base, size_t dim, size_t size,
	int (*comp) (const void *a, const void *b));

/*
$SeeAlso
\mref{vecbuf(3)}, \mref{vb_alloc(3)}, \mref{vb_create(3)},
\mref{vb_copy(3)}, \mref{vb_search(3)}, \mref{lmalloc(3)}.
*/

#endif	/* EFEU/vecbuf.h */
