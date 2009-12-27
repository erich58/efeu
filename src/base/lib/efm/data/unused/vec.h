/*	Vektorhilfsprogramme
	(c) 1998 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

$Header	<EFEU/$1>
*/

#ifndef EFEU_VEC_H
#define EFEU_VEC_H	1

#include <EFEU/memalloc.h>

/*	Speicherverwaltung für Datenvektoren
*/

void *vec_alloc (size_t size, size_t dim);
void vec_free (void *ptr, size_t size, size_t dim);
void vec_alloc_stat (void);

#define	vec_align_dim(n,b)	((b) * (((n) + (b) - 1) / (b)))
#define vec_need_realloc(n,s,b)	((n) > (s) || (n) + (b) <= (s))

extern size_t vec_alloc_request;	/* Zahl der Anforderungen */
extern size_t vec_alloc_total;		/* Angeforderter Speicherplatz */
extern size_t vec_alloc_free;		/* Freier Speicherplatz */
extern int vec_alloc_debug_flag;	/* Debugflag */


/*	Datenvektor löschen
*/

void vec_clean (clean_t clean, void *ptr, size_t size, size_t dim);

#endif	/* EFEU/vec.h */
