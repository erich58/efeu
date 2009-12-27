/*	Gro�e Datenmengen sortieren
	(c) 1995 Erich Fr�hst�ck
	A-1090 Wien, W�hringer Stra�e 64/6

$Header	<EFEU/$1>
*/

#ifndef EFEU_BIGSORT_H
#define EFEU_BIGSORT_H	1

#include <EFEU/ftools.h>
#include <EFEU/vecbuf.h>

#define	MSG_BIGSORT	"bigsort"

#define	BS_MAXMERGE	4	/* Maximazahl der mischbaren Dateien */

extern int BigSortDebug;


/*	Sortierstruktur
*/

typedef struct {
	size_t size;
	void *data;
} BigSortPtr_t;

typedef struct {
	size_t recl;	/* Satzl�nge  */
	size_t size;	/* Maximale Buffergr��e */
	size_t pos;	/* Position im Buffer */
	size_t dim;	/* Maximale Zahl der S�tze */
	size_t idx;	/* Aktueller Satzindex */
	char *data;	/* Zwischenspeicher */
	comp_t comp;	/* Vergleichsfunktion */
	vecbuf_t ftab;	/* Tabelle mit Zwischenfiles */
	BigSortPtr_t *ptr;	/* Pointervektor */
} BigSort_t;


extern BigSort_t *BigSort (size_t recl, size_t dim, size_t size, comp_t comp);
extern void BigSortReorg (BigSort_t *sort);
extern void BigSortClose (BigSort_t *sort);
extern void *BigSortNext (BigSort_t *sort, size_t recl);
extern void *BigSortData (BigSort_t *sort, size_t *size);

#endif	/* EFEU/bigsort.h */
