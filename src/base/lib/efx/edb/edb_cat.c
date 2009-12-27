/*	EDB-Datenbanken zusammenhängen

$Copyright (C) 2005 Erich Frühstück
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
	REFVAR;
	EDB **tab;
	size_t dim;
} CPAR;

static void cpar_clean (void *data)
{
	CPAR *cpar = data;

	while (cpar->dim)
		rd_deref(cpar->tab[--cpar->dim]);

	memfree(cpar);
}

static RefType cpar_reftype = REFTYPE_INIT("EDB_CAT", NULL, cpar_clean);

static int cpar_read (EfiType *type, void *data, void *par)
{
	CPAR *cpar = par;

	while (cpar->dim)
	{
		EDB *edb = cpar->tab[cpar->dim - 1];

		if	(edb_read(edb))
		{
			EfiObj *obj = EvalObj(RefObj(edb->obj), type);

			if	(obj)
			{
				CopyData(type, data, obj->data);
				UnrefObj(obj);
			}
			else	CleanData(type, data);
			
			return 1;
		}

		rd_deref(edb);
		cpar->dim--;
	}

	return 0;
}

EDB *edb_cat (EDB **tab, size_t dim)
{
	if	(dim > 1)
	{
		CPAR *cpar;
		EDB *edb;
		int n;
		
		cpar = memalloc(sizeof cpar[0] + dim * sizeof cpar->tab[0]);
		cpar->tab = (void *) (cpar + 1);
		cpar->dim = dim;
		rd_init(&cpar_reftype, cpar);

		for (n = 0; dim-- > 0; n++)
			cpar->tab[n] = tab[dim];

		edb = edb_create(LvalObj(NULL, tab[0]->obj->type), NULL);
		edb->read = cpar_read;
		edb->ipar = cpar;
		return edb;
	}

	return dim ? tab[0] : NULL;
}
