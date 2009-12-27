/*
:*:	data type info
:de:	Datenbanktypen

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

#include <DB/PG.h>

#if	HAS_PQ
typedef struct {
	size_t dim;
	struct {
		unsigned oid;
		char *name;
	} *entry;
} TINFO;

void PGType_clean (void *handle)
{
	TINFO *tinfo = handle;

	if	(!tinfo)	return;

	while (tinfo->dim)
	{
		tinfo->dim--;
		memfree(tinfo->entry[tinfo->dim].name);
	}

	memfree(tinfo);
}

void *PGType_create (PG *pg)
{
	TINFO *ptr;
	size_t i, n;

	if	(!pg)	return NULL;

	PG_query(pg, "select oid, typname from pg_type");

	if	(!pg->res || !(n = PQntuples(pg->res)))
		return NULL;

	ptr = memalloc(sizeof *ptr + n * sizeof ptr->entry[0]);
	ptr->entry = (void *) (ptr + 1);
	ptr->dim = n;

	for (i = 0; i < n; i++)
	{
		ptr->entry[i].oid = strtoul(PQgetvalue(pg->res, i, 0),
			NULL, 10);
		ptr->entry[i].name = mstrcpy(PQgetvalue(pg->res, i, 1));
	}

	return ptr;
}

char *PGType_name (void *handle, Oid oid)
{
	TINFO *ptr = handle;
	size_t i;

	if	(!ptr)	return NULL;

	for (i = 0; i < ptr->dim; i++)
		if (ptr->entry[i].oid == oid)
			return ptr->entry[i].name;

	return NULL;
}

void PGType_list (void *handle, IO *out)
{
	TINFO *ptr = handle;
	size_t i;

	if	(!ptr || !out)
		return;

	for (i = 0; i < ptr->dim; i++)
		io_printf(out, "%u\t%s\n", ptr->entry[i].oid,
			ptr->entry[i].name);
}

#endif
