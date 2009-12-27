/*
Symboltabellen

$Copyright (C) 1999 Erich Frühstück
This file is part of EFEU.

EFEU is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

EFEU is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public
License along with EFEU; see the file COPYING.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef	DocSym_h
#define	DocSym_h	1

#include <EFEU/io.h>
#include <EFEU/Info.h>
#include <EFEU/strbuf.h>
#include <EFEU/stack.h>

#ifndef	MSG_DOC
#define	MSG_DOC	"Doc"
#endif


typedef struct {
	char *key;	/* Symbolkennung */
	char *fmt;	/* Symbolformat */
} DocSymEntry_t;

typedef struct {
	strbuf_t buf;	/* Stringbuffer */
	size_t dim;	/* Zahl der Einträge */
	DocSymEntry_t *tab;	/* Symbolvektor */
} DocSym_t;

DocSym_t *DocSym (const char *name);
int DocSym_print (io_t *io, DocSym_t *sym);
char *DocSym_get (DocSym_t *sym, const char *name);
void DocSym_free (DocSym_t *sym);

#endif	/* DocSym.h */
