/*	Dynamische Stringoperationen
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.6

$Header	<EFEU/$1>
*/

#ifndef	EFEU_MSTRING_H
#define	EFEU_MSTRING_H	1

#include <EFEU/memalloc.h>

typedef char *mstr_t;

size_t mstrlen (const char *a);
int mstrcmp (const char *a, const char *b);
char *mstrchr (const char *a, const char *b);

char *mstrcat (const char *delim, const char *str1, ...);
char *mstrcpy (const char *str);
char *mstrncpy (const char *str, size_t len);
char *mstrpaste (const char *delim, const char *a, const char * b);
char *listcat (const char *delim, char **list, size_t dim);

char *mvsprintf (const char *fmt, va_list list);
char *msprintf (const char *fmt, ...);

size_t strsplit (const char *str, const char *delim, char ***ptr);
char *nextstr (char **ptr);

/*	Sonstige Stringshilfsfunktionen
*/

void *rmemcpy(void *dest, const void *src, size_t n);

#if	REVBYTEORDER
#define	MEMCPY	rmemcpy
#else
#define	MEMCPY	memcpy
#endif

#endif	/* EFEU/mstring.h */
