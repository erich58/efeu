/*
Standardschlüsseltabellen

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

#ifndef	EFEU_KeyTab_h
#define	EFEU_KeyTab_h	1

#include <EFEU/vecbuf.h>

#define	KEYTAB(name,bs)	vecbuf_t name = VB_DATA(bs, sizeof(void **))

#define	KeyTab(bsize)	vb_create(bsize, sizeof(void **))

void *StrKey_add (vecbuf_t *buf, void *data);
void *StrKey_get (vecbuf_t *buf, const char *name, void *defval);
void StrKey_append (vecbuf_t *buf, void *base, size_t nel, size_t size);

/*
$SeeAlso
\mref{vecbuf(3)}, \mref{vb_alloc(3)}, \mref{vb_create(3)},
\mref{vb_search(3)}, \mref{lmalloc(3)}.
*/

#endif	/* EFEU/KeyTab.h */
