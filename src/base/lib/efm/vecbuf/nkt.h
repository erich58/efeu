/*
:*:Name key tables
:de:Namensschlüsseltabellen

$Header	<EFEU/$1>

$Copyright (C) 2001 Erich Frühstück
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

#ifndef	EFEU_nkt_h
#define	EFEU_nkt_h	1

#include <EFEU/vecbuf.h>
#include <EFEU/refdata.h>

typedef struct {
	const char *name;
	void *data;
} NameKeyEntry;

typedef struct {
	REFVAR;
	char *name;
	VecBuf tab;
	void (*clean) (void *data);
} NameKeyTab;

extern RefType nkt_reftype;

/*
Der Makro |$1| liefert die Initialisierungswerte für eine Schlüsseltabelle
mit Namen |name|, Blockgröße |bsize| und Löschfunktion |clean|.
*/

#define	NKT_DATA(name, bsize, clean)	\
{ REFDATA(&nkt_reftype), name, VB_DATA(bsize, sizeof(NameKeyEntry)), clean }

NameKeyTab *nkt_create (const char *name, size_t bsize,
	void (*clean) (void *data));

void nkt_clean (NameKeyTab *tab);
int nkt_insert (NameKeyTab *nkt, const char *name, void *data);
int nkt_delete (NameKeyTab *nkt, const char *name);
void *nkt_fetch (NameKeyTab *nkt, const char *name, void *defval);
int nkt_walk (NameKeyTab *nkt,
	int (*visit) (const char *name, void *data, void *par), void *par);
int nkt_rwalk (NameKeyTab *nkt,
	int (*visit) (const char *name, void *data, void *par), void *par);

/*
$SeeAlso
\mref{vecbuf(3)}, \mref{vb_alloc(3)}, \mref{vb_create(3)},
\mref{vb_search(3)}, \mref{lmalloc(3)}.
*/

#endif	/* EFEU/KeyTab.h */
