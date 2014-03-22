/*
Hauptdefinitionen für efmain Programmbibliothek

$Header <EFEU/$1>

$Copyright (C) 1994 Erich Frühstück
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

#ifndef	EFEU_efmain_h
#define	EFEU_efmain_h	1

#include <EFEU/mstring.h>
#include <EFEU/memalloc.h>
#include <EFEU/strbuf.h>
#include <EFEU/label.h>

#define	MSG_EFMAIN	"efmain"


/*	Standardmakros und Funktionstypedefinitionen
*/

#ifndef	min
#define	min(a, b)	((a) < (b) ? (a) : (b))
#endif

#ifndef	max
#define	max(a, b)	((a) > (b) ? (a) : (b))
#endif

#ifndef	tabsize
#define	tabsize(x)	(sizeof(x) / sizeof(x[0]))
#endif


/*	Initialisieren / Aufräumen
*/

#include <EFEU/procenv.h>

void skiparg (int *narg, char **arg, int n);
char *argval (const char *arg);


/*	Stringhilfsfunktionen
*/

#include <EFEU/mstring.h>

size_t mstrsplit (const char *str, const char *delim, char ***ptr);
char *nextstr (char **ptr);
int xstrcmp(const char *a, const char *b);
int mscanf (const char *str, const char *fmt, ...);
void streval (const char *cmd);


#include <EFEU/ftools.h>

/*	Mustervergleich
*/

int listcmp (const char *list, int c);
int patcmp (const char *pat, const char *str, char **ptr);
int patselect (const char *name, char **list, size_t dim);


#endif	/* EFEU/efmain.h */
