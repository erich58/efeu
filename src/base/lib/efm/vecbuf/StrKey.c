/*
Tabelle mit Stringschlüssel

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

#include <EFEU/KeyTab.h>
#include <EFEU/mstring.h>

typedef struct {
	const char *name;
} StrKey_t;

static int StrKey_cmp (const void *a, const void *b)
{
	StrKey_t **ka = (StrKey_t **) a;
	StrKey_t **kb = (StrKey_t **) b;
	return mstrcmp((*ka)->name, (*kb)->name);
}

void *StrKey_add (vecbuf_t *buf, void *data)
{
	vb_search(buf, &data, StrKey_cmp, VB_REPLACE);
	return data;
}

void *StrKey_get (vecbuf_t *buf, const char *name, void *defval)
{
	StrKey_t key, *keyptr;
	void **resptr;

	if	(!buf)	return defval;

	key.name = name;
	keyptr = &key;
	resptr = vb_search(buf, &keyptr, StrKey_cmp, VB_SEARCH);
	return resptr ? *resptr : defval;
}

void StrKey_append (vecbuf_t *buf, void *base, size_t nel, size_t size)
{
	char *ptr;

	if	(buf == NULL || base == NULL || nel == 0)
		return;

	for (ptr = (char *) base; nel-- > 0; ptr += size)
		StrKey_add(buf, ptr);
}
