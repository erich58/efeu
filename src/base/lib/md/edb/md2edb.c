/*	EDB-Datenfile auzf Multidimensiomnale Datenmatrix aufsetzen

$Copyright (C) 2006 Erich Frühstück
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

#include <EFEU/mdmat.h>
#include <EFEU/printobj.h>

static EfiStruct *axis2var (mdaxis *x)
{
	EfiType *type;
	int i;

	type = NewEnumType(NULL, 4);

	for (i = 0; i < x->dim; i++)
		AddEnumKey(type, StrPool_mget(x->sbuf, x->idx[i].i_name),
			StrPool_mget(x->sbuf, x->idx[i].i_desc), i + 1);

	type = AddEnumType(type);
	return NewEfiStruct(type, StrPool_get(x->sbuf, x->i_name), 0);
}

static void save_data (IO *io, const EfiType *type, mdaxis *x,
	unsigned char *buf, size_t offset, void *data)
{
	if	(x != NULL)
	{
		unsigned i;

		for (i = 1; i <= x->dim; i++)
		{
			buf[offset] = (i >> 24) & 0x7F;
			buf[offset + 1] = (i >> 16) & 0xFF;
			buf[offset + 2] = (i >> 8) & 0xFF;
			buf[offset + 3] = i & 0xFF;
			save_data(io, type, x->next, buf, offset + 4, data);
			data = ((char *) data) + x->size;
		}
	}
	else
	{
		io_write(io, buf, offset);
		WriteData(type, data, io);
	}
}
		
EDB *md2edb (mdmat *md)
{
	EfiStruct *var, **ptr;
	mdaxis *x;
	EDB *edb;
	IO *tmp;
	unsigned char *buf;
	size_t n;

	if	(!md)	return NULL;

	var = NULL;
	ptr = &var;
	n = 0;

	for (x = md->axis; x; x = x->next)
	{
		*ptr = axis2var(x);
		ptr = &(*ptr)->next;
		n += 4;
	}

	*ptr = NewEfiStruct(md->type, mstrcpy("data"), 0);

	edb = edb_alloc(LvalObj(NULL, MakeStruct(NULL, NULL, var)),
		mstrcpy(StrPool_get(md->sbuf, md->i_name)));

	tmp = io_tmpfile();
	buf = memalloc(n);
	save_data(tmp, md->type, md->axis, buf, 0, md->data);
	memfree(buf);

	io_rewind(tmp);
	edb_data(edb, tmp, "binary");
	return edb;
}
