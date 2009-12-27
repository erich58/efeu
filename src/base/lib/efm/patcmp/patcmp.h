/*	Mustervergleich
	(c) 1997 Erich Fr�hst�ck
	A-1090 Wien, W�hringer Stra�e 64/6

	Version 0.6

$Header	<EFEU/$1>
*/

#ifndef	EFEU_PATCMP_H
#define	EFEU_PATCMP_H	1

#include <EFEU/config.h>

int listcmp (const char *list, int c);
int patcmp (const char *pat, const char *str, char **ptr);
int patselect (const char *name, char **list, size_t dim);
int lexcmp (const char *a, const char *b);

#endif	/* EFEU/patcmp.h */
