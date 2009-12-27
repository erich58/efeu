/*
Objektallocation

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

#include <EFEU/object.h>
#include <EFEU/refdata.h>

#define	MEMCHECK	0

#define	SIZE_PTR	(sizeof(Obj_t))
#define	SIZE_SMALL	(sizeof(Obj_t) + sizeof(long))
#define	SIZE_LARGE	(sizeof(Obj_t) + 4 * sizeof(void *))

#if	MEMCHECK
#define	CHECK_SIZE	8
#define	CHECK_MASK	"12345678"

#define	do_check(obj)	Obj_check(obj)

#else
#define	CHECK_SIZE	0
#define	CHECK_MASK	""

#define	do_check(obj)
#endif

static ALLOCTAB(tab_ptr, 100, SIZE_PTR + CHECK_SIZE);
static ALLOCTAB(tab_small, 70, SIZE_SMALL + CHECK_SIZE);
static ALLOCTAB(tab_large, 50, SIZE_LARGE + CHECK_SIZE);

static size_t stat_alloc = 0;
static size_t stat_free = 0;

Obj_t *Obj_alloc (size_t size)
{
	Obj_t *obj;

	if	(size <= SIZE_PTR)	obj = new_data(&tab_ptr);
	else if	(size <= SIZE_SMALL)	obj = new_data(&tab_small);
	else if	(size <= SIZE_LARGE)	obj = new_data(&tab_large);
	else				obj = lmalloc(size), stat_alloc++;

#if	MEMCHECK
	memcpy(((char *) obj) + size, CHECK_MASK, CHECK_SIZE);
#endif
	return obj;
}

void Obj_free (Obj_t *obj, size_t size)
{
	if	(size <= SIZE_PTR)	del_data(&tab_ptr, obj);
	else if	(size <= SIZE_SMALL)	del_data(&tab_small, obj);
	else if	(size <= SIZE_LARGE)	del_data(&tab_large, obj);
	else				lfree(obj), stat_free++;
}

#define	STAT_1	"%s: Size %3ld +%3ld: %5ld used, %5ld free, %5ld byte (%ldx%ldx%ld)\n"
#define	STAT_L	"%s: Large objects:  %5ld used, %5ld requests\n"

static void show_alloc(alloctab_t *tab, const char *prompt)
{
	chain_t *x;
	size_t n;

	x = tab->blklist;

	for (n = 0; x != NULL; n++)
		x = x->next;

	fprintf(stderr, STAT_1, prompt,
		(ulong_t) sizeof(Obj_t),
		(ulong_t) (tab->elsize - sizeof(Obj_t)),
		(ulong_t) tab->nused,
		(ulong_t) tab->nfree,
		(ulong_t) n * tab->blksize * tab->elsize,
		(ulong_t) n,
		(ulong_t) tab->blksize,
		(ulong_t) tab->elsize);
}

/*
Die Funktion |$1| schreibt eine Liste der
von Objekten reservierten Speichersegmete
zur Standardfehlerausgabe.
*/

void Obj_stat (const char *prompt)
{
	if	(!prompt)	prompt = "allocstat()";

	show_alloc(&tab_ptr, prompt);
	show_alloc(&tab_small, prompt);
	show_alloc(&tab_large, prompt);

	fprintf(stderr, STAT_L, prompt,
		(ulong_t) (stat_alloc - stat_free), (ulong_t) stat_alloc);
}

#define	CHECK_MSG	"%s: Speicherfehler von Objekt %lx, Type %#s.\n"


void Obj_check(const Obj_t *obj)
{
#if	MEMCHECK
	size_t size;
	
	if	(obj == NULL || obj->lref)	return;

	size = sizeof(Obj_t) + obj->type->size;

	if	(memcmp(((const char *) obj) + size, CHECK_MASK, CHECK_SIZE) != 0)
		io_printf(ioerr, CHECK_MSG, ProgIdent, (ulong_t) obj, obj->type->name);
#else
	;
#endif
}
