/*	EDB-Datenbanken mischen

$Copyright (C) 2004 Erich Frühstück
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

#include <EFEU/EDB.h>
#include <EFEU/Debug.h>
#include <EFEU/ioctrl.h>

typedef struct {
	EDB *edb;
	IO *io;
	StrBuf buf;
} ENTRY;

static void entry_clean (ENTRY *entry)
{
	rd_deref(entry->io);
	sb_free(&entry->buf);
	rd_deref(entry->edb);
	entry->edb = NULL;
}

static void entry_load (ENTRY *entry)
{
	sb_clean(&entry->buf);

	if	(edb_read(entry->edb))
	{
		EfiObj *obj = entry->edb->obj;
		WriteData(obj->type, obj->data, entry->io);
	}
	else	entry_clean(entry);
}

static void entry_init (ENTRY *entry, EDB *edb)
{
	entry->edb = edb;
	sb_init(&entry->buf, 0);
	entry->io = io_strbuf(&entry->buf);
	entry_load(entry);
}

typedef struct {
	REFVAR;
	ENTRY *tab;
	size_t dim;
	CmpDef *cdef;
	int (*cmp) (CmpDef *cdef, const ENTRY *a, const ENTRY *b);
} MPAR;

static void mpar_clean (void *data)
{
	MPAR *mpar = data;

	while (mpar->dim)
	{
		mpar->dim--;

		if	(mpar->tab[mpar->dim].edb)
			entry_clean(mpar->tab + mpar->dim);
	}

	rd_deref(mpar->cdef);
	memfree(mpar);
}

static RefType mpar_reftype = REFTYPE_INIT("EDB_MERGE", NULL, mpar_clean);

static int std_cmp (CmpDef *cdef, const ENTRY *a, const ENTRY *b)
{
	int r = memcmp(a->buf.data, b->buf.data,
		a->buf.pos < b->buf.pos ? a->buf.pos : b->buf.pos);

	if	(r)				return r;
	else if	(a->buf.pos < b->buf.pos)	return -1;
	else if	(a->buf.pos > b->buf.pos)	return 1;
	else					return 0;
}

static int ext_cmp (CmpDef *cdef, const ENTRY *a, const ENTRY *b)
{
	return cmp_data(cdef, a->edb->obj->data, b->edb->obj->data);
}

static ENTRY *mpar_search (MPAR *par)
{
	ENTRY *p, *select;
	int i;

	select = NULL;

	for (p = par->tab, i = par->dim; i-- > 0; p++)
	{
		if	(!p->edb)	continue;

		if	(!select || par->cmp(par->cdef, select, p) > 0)
			select = p;
	}

	return select;
}

static int mpar_read (EfiType *type, void *data, void *par)
{
	MPAR *mpar = par;
	ENTRY *select = mpar_search(mpar);

	if	(select)
	{
		select->buf.pos = 0;
		ReadData(type, data, select->io);
		entry_load(select);
		return 1;
	}

	return 0;
}

EDB *edb_merge (CmpDef *cdef, EDB **tab, size_t dim)
{
	if	(dim > 1)
	{
		MPAR *mpar;
		EDB *edb;
		int n;
		
		mpar = memalloc(sizeof mpar[0] + dim * sizeof mpar->tab[0]);
		mpar->tab = (void *) (mpar + 1);
		mpar->dim = dim;
		mpar->cdef = cdef;
		mpar->cmp = cdef ? ext_cmp : std_cmp;
		rd_init(&mpar_reftype, mpar);

		for (n = 0; n < dim; n++)
			entry_init(mpar->tab + n, tab[n]);

		edb = edb_create(tab[0]->obj->type);
		edb->read = mpar_read;
		edb->ipar = mpar;
		edb->desc = tab[0]->desc;
		tab[0]->desc = NULL;
		return edb;
	}

	rd_deref(cdef);
	return dim ? tab[0] : NULL;
}
