/*
Standardsuchtabellen

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

#include <EFEU/efmain.h>

nkey_t nkey_buf = { 0 };
skey_t skey_buf = { NULL };

typedef struct {
	int num;
	const void *ptr;
} NENTRY;


typedef struct {
	const char *name;
	const void *ptr;
} SENTRY;


static ALLOCTAB(skeytab, 32, sizeof(SENTRY));
static ALLOCTAB(nkeytab, 32, sizeof(NENTRY));

#define	NKEY(x)	(((const nkey_t *) (x))->num)
#define	SKEY(x)	(((const skey_t *) (x))->name)


/*	Vergleichsfunktion
*/

int skey_cmp(const void *a, const void *b)
{
	return mstrcmp(SKEY(a), SKEY(b));
}


/*	Pointer abfragen
*/

void *skey_get(xtab_t *tab, const char *name, const void *def)
{
	SENTRY *entry;

	entry = (SENTRY *) skey_find(tab, name);
	return (void *) (entry && entry->ptr ? entry->ptr : def);
}


/*	Wert setzen
*/

void *skey_set(xtab_t *tab, const char *name, const void *ptr)
{
	SENTRY *x, *y;

	if	(tab == NULL)	return NULL;

	x = (SENTRY *) new_data(&skeytab);
	x->name = mstrcpy(name);
	x->ptr = ptr;
	y = (SENTRY *) xsearch(tab, (void *) x, XS_ENTER);

	if	(y != x)
	{
		ptr = y->ptr;
		y->ptr = x->ptr;
		memfree((char *) x->name);
		del_data(&skeytab, x);
		return (void *) ptr;
	}
	else	return NULL;
}


/*	Vergleichsfunktion
*/

int nkey_cmp(const void *a, const void *b)
{
	if	(NKEY(a) < NKEY(b))	return -1;
	else if	(NKEY(a) > NKEY(b))	return 1;
	else				return 0;
}


/*	Wert abfragen
*/

void *nkey_get(xtab_t *tab, int num, const void *def)
{
	NENTRY *entry;

	entry = (NENTRY *) nkey_find(tab, num);
	return (void *) (entry && entry->ptr ? entry->ptr : def);
}


/*	Wert setzen
*/

void *nkey_set(xtab_t *tab, int num, const void *ptr)
{
	NENTRY *x, *y;

	if	(tab == NULL)	return NULL;

	x = (NENTRY *) new_data(&nkeytab);
	x->num = num;
	x->ptr = ptr;
	y = (NENTRY *) xsearch(tab, (void *) x, XS_ENTER);

	if	(y != x)
	{
		ptr = y->ptr;
		y->ptr = x->ptr;
		del_data(&nkeytab, x);
		return (void *) ptr;
	}
	else	return NULL;
}
