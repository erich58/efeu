/*	Dynamischer Vektorbuffer
	(c) 1996 Erich Fr�hst�ck
	A-1090 Wien, W�hringer Stra�e 64/6

	Version 0.6

$Header	<EFEU/$1>
$SeeAlso
\mref{vecbuf(3)}, \mref{vb_alloc(3)}, \mref{vb_create(3)},
\mref{vb_search(3)}, \mref{lmalloc(3)}.
*/

#ifndef	_EFEU_vecbuf_h
#define	_EFEU_vecbuf_h	1

#include <EFEU/types.h>

/*
Die Datenstruktur |$1| dient zur Verwaltung eines Datenfeldes.
Dabei ist |data| ein Pointer auf ein Speicherfeld, da� |size|
Elemente der Gr��e |elsize| enth�lt. Die Komponente |used|
gibt an, wieviele Elemente gerade ben�tzt werden. Falls
|blksize| verschieden von null ist, ist der Buffer dynamisch und
wird bei Bedarf um |blksize| Elemente vergr��ert.
*/

typedef struct {
	size_t blksize;	/* Blockgr��e zur Buffervergr��erung */
	size_t elsize;	/* Gr��e eines Datenelementes */
	size_t size;	/* Aktuelle Gr��e des Buffers */
	size_t used;	/* Zahl der ben�tzten Elemente */
	void *data;	/* Datenbuffer */
} vecbuf_t;


/*
Der Makro |$1| liefert die Strukturkomponenten eines Vektorbuffers
mit Blockgr��e <blk> und Datengr��e <size>. Er wird bei der Initialisierung
von Variablen verwendet, die einen Vektorbuffer als Komponente enthalten.
*/

#define	VB_DATA(blk, size)	{ blk, size, 0, 0, NULL }

/*
Der Makro |$1| deklariert den Vektorbuffer <name> und initialisiert
ihn mit der Blockgr��e <blk> und der Datengr��e <size>.

$Examples
Die nachfolgende Befehlszeile richtet einen statischen Vektorbuffer
mit Namen |buf| vom Type |int| ein:

---- verbatim
static VECBUF(buf, 1024, sizeof(int));
----
*/

#define	VECBUF(name, blk, size)	vecbuf_t name = VB_DATA(blk, size)

vecbuf_t *vb_create (size_t blk, size_t size);
void vb_destroy (vecbuf_t *vb);

void vb_init (vecbuf_t *buf, size_t blk, size_t size);
void *_vb_alloc (vecbuf_t *buf, size_t dim);
void *vb_alloc (vecbuf_t *buf, size_t dim);
void vb_free (vecbuf_t *buf);
void vb_clean (vecbuf_t *vb, clean_t clean);
void *vb_realloc (vecbuf_t *buf, size_t dim);
void *vb_next (vecbuf_t *buf);
void *vb_data (vecbuf_t *buf, size_t pos);
void *vb_insert (vecbuf_t *buf, size_t pos, size_t dim);
void *vb_delete (vecbuf_t *buf, size_t pos, size_t dim);
void *vb_load (vecbuf_t *buf, FILE *file, size_t dim);

void vb_qsort (vecbuf_t *buf, comp_t comp);
void *vb_bsearch (vecbuf_t *buf, const void *key, comp_t comp);

#define	VB_ENTER	0	/* Eintrag abfragen/erg�nzen */
#define	VB_REPLACE	1	/* Eintrag ersetzen/erg�nzen */
#define	VB_SEARCH	2	/* Eintrag abfragen */
#define	VB_DELETE	3	/* Eintrag aus Tabelle l�schen */

void *vb_search (vecbuf_t *buf, void *data, comp_t comp, int mode);

/*	Hilfsfunktionen
*/

size_t vsplit (void *base, size_t dim, size_t size, test_t test);

#endif	/* EFEU/vecbuf.h */
