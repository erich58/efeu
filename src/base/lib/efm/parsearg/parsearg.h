/*
Prozessargumente analysieren und zerlegen

$Header	<EFEU/$1>

$Copyright (C) 1997 Erich Frühstück
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

#ifndef	EFEU_parsearg_h
#define	EFEU_parsearg_h	1

#include <EFEU/memalloc.h>
#include <EFEU/refdata.h>


typedef struct {
	char *name;	/* Argumentname */
	char *opt;	/* Optionsstring */
	char *arg;	/* Argument */
} assignarg_t;

extern char *argval (const char *arg);
extern assignarg_t *assignarg (const char *arg, char **ptr, const char *delim);

extern void skiparg (int *narg, char **arg, int n);

#endif	/* EFEU/parsearg.h */
