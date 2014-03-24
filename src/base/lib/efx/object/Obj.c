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
#include <EFEU/printobj.h>
#include <EFEU/refdata.h>
#include <EFEU/stdtype.h>
#include <EFEU/Debug.h>

#define	MEMCHECK	0

#define	SIZE_PTR	(sizeof(EfiObj))
#define	SIZE_SMALL	(sizeof(EfiObj) + sizeof(void *))
#define	SIZE_LARGE	(sizeof(EfiObj) + 6 * sizeof(void *))

#if	MEMCHECK
#define	CHECK_SIZE	8
#define	CHECK_MASK	"12345678"

#define	do_check(obj)	Obj_check(obj)

#else
#define	CHECK_SIZE	0
#define	CHECK_MASK	""

#define	do_check(obj)
#endif

extern LogControl ObjDebugControl;

static ALLOCTAB(tab_ptr, "PtrObj", 100, SIZE_PTR + CHECK_SIZE);
static ALLOCTAB(tab_small, "SmallObj", 292, SIZE_SMALL + CHECK_SIZE);
static ALLOCTAB(tab_large, "LargeObj", 50, SIZE_LARGE + CHECK_SIZE);

static size_t stat_alloc = 0;
static size_t stat_free = 0;

static void clean_data (EfiObj *obj)
{
	UnlinkUpdateList(obj->list);

	if	(obj->lval && obj->lval->free)
	{
		obj->lval->free(obj);
	}
	else if	(obj->data)
	{
		CleanData(obj->type, obj->data, 1);
	}
}

static void clean_ptr (void *data)
{
	clean_data(data);
	del_data(&tab_ptr, data);
}

static void clean_ext (void *data)
{
	del_data(&tab_ptr, data);
}

static void clean_small (void *data)
{
	clean_data(data);
	del_data(&tab_small, data);
}

static void clean_large (void *data)
{
	clean_data(data);
	del_data(&tab_large, data);
}

static void clean_huge (void *data)
{
	clean_data(data);
	lfree(data);
	stat_free++;
}

static RefType reftype_ptr = REFTYPE_EXT("PtrObj",
	NULL, clean_ptr, &ObjDebugControl);
static RefType reftype_ext = REFTYPE_EXT("ExtObj",
	NULL, clean_ext, &ObjDebugControl);
static RefType reftype_small = REFTYPE_EXT("SmallObj",
	NULL, clean_small, &ObjDebugControl);
static RefType reftype_large = REFTYPE_EXT("LargeObj",
	NULL, clean_large, &ObjDebugControl);
static RefType reftype_huge = REFTYPE_EXT("HugeObj",
	NULL, clean_huge, &ObjDebugControl);

static RefType reftype_large_lval = REFTYPE_EXT("LargeLval",
	NULL, clean_large, &ObjDebugControl);
static RefType reftype_huge_lval = REFTYPE_EXT("HugeLval",
	NULL, clean_huge, &ObjDebugControl);

EfiObj *ExtObj (const EfiType *type, const void *data)
{
	EfiObj *obj = rd_init(&reftype_ext, new_data(&tab_ptr));
	obj->lval = NULL;
	obj->type = (EfiType *) type;
	obj->data = (void *) data;
	obj->list = NULL;
	return obj;
}

EfiObj *Obj_alloc (size_t size)
{
	EfiObj *obj;

	if	(size <= SIZE_PTR)
	{
		obj = rd_init(&reftype_ptr, new_data(&tab_ptr));
	}
	else if	(size <= SIZE_SMALL)
	{
		obj = rd_init(&reftype_small, new_data(&tab_small));
	}
	else if	(size <= SIZE_LARGE)
	{
		obj = rd_init(&reftype_large, new_data(&tab_large));
	}
	else			
	{
		obj = lmalloc(size);
		memset(obj, 0, size);
		rd_init(&reftype_huge, obj);
		stat_alloc++;
	}

#if	MEMCHECK
	memcpy(((char *) obj) + size, CHECK_MASK, CHECK_SIZE);
#endif
	return obj;
}

void *Lval_alloc (size_t size)
{
	void *obj;

	if	(size <= SIZE_LARGE)
	{
		obj = rd_init(&reftype_large_lval, new_data(&tab_large));
	}
	else			
	{
		obj = lmalloc(size);
		memset(obj, 0, size);
		rd_init(&reftype_huge_lval, obj);
		stat_alloc++;
	}

#if	MEMCHECK
	memcpy(((char *) obj) + size, CHECK_MASK, CHECK_SIZE);
#endif
	return obj;
}

#define	STAT_1	\
	"%s: Size %3ld +%3ld: %5ld used, %5ld free, %5ld byte (%ldx%ldx%ld)\n"
#define	STAT_L	\
	"%s: Large objects: %5ld used, %5ld requests\n"

static void show_alloc (AllocTab *tab, const char *prompt)
{
	AllocTabList *x;
	size_t n;

	x = tab->blklist;

	for (n = 0; x != NULL; n++)
		x = x->next;

	fprintf(stderr, STAT_1, prompt,
		(unsigned long) sizeof(EfiObj),
		(unsigned long) (tab->elsize - sizeof(EfiObj)),
		(unsigned long) tab->nused,
		(unsigned long) tab->nfree,
		(unsigned long) n * tab->blksize * tab->elsize,
		(unsigned long) n,
		(unsigned long) tab->blksize,
		(unsigned long) tab->elsize);
}

void Obj_putkey (const EfiObj *obj, IO *out)
{
	if	(!obj)
	{
		io_puts("NULL", out);
		return;
	}

	io_printf(out, "%p:", obj);

	if	(obj->reftype)
		io_printf(out, " %s", obj->reftype->label);

	io_printf(out, " %u", obj->refcount);

	if	(obj->type)
		io_printf(out, " %s", obj->type->name);

	if	(obj->lval)
		io_printf(out, " &%s", obj->lval->name);
}

/*
Die Funktion |$1| liefert einen Identifikationsstring zum angegebenen
Object.
*/

char *Obj_ident (const EfiObj *obj)
{
	if	(obj)
	{
		StrBuf *buf;
		IO *io;
			
		buf = sb_acquire();
		io = io_strbuf(buf);
		Obj_putkey(obj, io);

		if	(obj->lval)
		{
			const EfiObjList *list;

			if	(obj->lval->ident)
			{
				char *p = obj->lval->ident(obj);

				if	(p)
				{
					sb_puts(" (", buf);
					sb_puts(p, buf);
					sb_putc(')', buf);
					memfree(p);
				}
			}

			sb_puts(obj->list ? " {\n" : " {", buf);

			for (list = obj->list; list; list = list->next)
			{
				sb_putc('\t', buf);
				Obj_putkey(list->obj, io);
				sb_putc('\n', buf);
			}

			sb_puts("}", buf);
		}

#if	0
		sb_puts(" = ", buf);
		PrintObj(io, obj);
#endif
		io_close(io);
		return sb_cpyrelease(buf);
	}

	return NULL;
}

/*
Die Funktion |$1| schreibt eine Liste der von Objekten reservierten
Speichersegmete zur Standardfehlerausgabe.
*/

void Obj_stat (const char *prompt)
{
	if	(!prompt)	prompt = "allocstat()";

	show_alloc(&tab_ptr, prompt);
	show_alloc(&tab_small, prompt);
	show_alloc(&tab_large, prompt);

	fprintf(stderr, STAT_L, prompt,
		(unsigned long) (stat_alloc - stat_free),
		(unsigned long) stat_alloc);
}

#define	CHECK_MSG	"%s: Speicherfehler von Objekt %p, Type %#s.\n"


void Obj_check (const EfiObj *obj)
{
#if	MEMCHECK
	size_t size;
	
	if	(obj == NULL || obj->lref)	return;

	size = sizeof(EfiObj) + obj->type->size;

	if	(memcmp(((const char *) obj) + size, CHECK_MASK, CHECK_SIZE) != 0)
		io_xprintf(ioerr, CHECK_MSG, ProgIdent, obj, obj->type->name);
#else
	;
#endif
}
