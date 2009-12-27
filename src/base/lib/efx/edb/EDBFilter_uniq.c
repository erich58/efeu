/*	uniq - filter

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

typedef struct {
	REFVAR;
	EDB *base;
	StrBuf buf;
	IO *io;
} UNIQ;

static void uniq_clean (void *data)
{
	UNIQ *uniq = data;
	rd_deref(uniq->base);
	io_close(uniq->io);
	memfree(uniq);
}

static RefType uniq_reftype = REFTYPE_INIT("EDB_UNIQ", NULL, uniq_clean);

static void *uniq_alloc (EDB *edb)
{
	UNIQ *uniq = memalloc(sizeof *uniq);
	uniq->base = edb;
	sb_init(&uniq->buf, 0);
	uniq->io = io_strbuf(&uniq->buf);
	return rd_init(&uniq_reftype, uniq);
}

static int read_uniq (EfiType *type, void *data, void *par)
{
	UNIQ *uniq = par;
	int rval;
	int pos;

	if	(!(rval = edb_read(uniq->base)))
		return 0;

	io_rewind(uniq->io);
	WriteData(type, uniq->base->obj->data, uniq->io);
	pos = uniq->buf.pos;

	while ((edb_read(uniq->base)))
	{
		uniq->buf.pos = pos;
		sb_sync(&uniq->buf);
		WriteData(type, uniq->base->obj->data, uniq->io);

		if	(2 * pos != uniq->buf.pos)
			break;

		if	(memcmp(uniq->buf.data, uniq->buf.data + pos, pos))
			break;
	}

	edb_unread(uniq->base);
	io_rewind(uniq->io);
	ReadData(type, data, uniq->io);
	return rval;
}

static EDB *fdef_uniq (EDBFilter *filter, EDB *base,
	const char *opt, const char *arg)
{
	EDB *edb = edb_create(LvalObj(NULL, base->obj->type), NULL);
	edb->read = read_uniq;
	edb->ipar = uniq_alloc(base);
	return edb;
}

EDBFilter EDBFilter_uniq = {
	"uniq", NULL, fdef_uniq, NULL,
	":*:remove duplicate data lines"
	":de:Doppelte Datensätze entfernen"
};
