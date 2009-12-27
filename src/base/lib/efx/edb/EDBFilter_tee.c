/*	tee - filter

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
#include <EFEU/EDBFilter.h>

static int tee_read (EfiType *type, void *data, void *par)
{
	EDB *base = par;

	if	(edb_read(base))
	{
		edb_write(base);
		return 1;
	}

	return 0;
}

static EDB *tee_create (EDBFilter *filter, EDB *base,
	const char *opt, const char *arg)
{
	EDB *edb = edb_share(base);
	edb->read = tee_read;
	edb->ipar = rd_refer(base);
	edb_fout(base, arg, opt);
	return edb;
}

EDBFilter EDBFilter_tee = EDB_FILTER(NULL,
	"tee", "[mode]=path", tee_create, NULL,
	":*:copy records and write into file"
	":de:Datensätze in Datei mitprotokollieren"
);
