/*	Prozessargumente analysieren und zerlegen
	(c) 1997 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.6

$Header	<EFEU/$1>
*/

#ifndef	EFEU_parsearg_h
#define	EFEU_parsearg_h	1

#include <EFEU/memalloc.h>


typedef struct {
	char *name;	/* Argumentname */
	char *opt;	/* Optionsstring */
	char *arg;	/* Argument */
} assignarg_t;

char *argval (const char *arg);
assignarg_t *assignarg(const char *arg);

void skiparg (int *narg, char **arg, int n);

#endif	/* EFEU/parsearg.h */
