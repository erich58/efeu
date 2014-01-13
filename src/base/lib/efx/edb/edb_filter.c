/*	EDB-Datenbankfilter

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
#include <EFEU/parsearg.h>
#include <EFEU/EDBFilter.h>

#define	FMT_UNDEF	"edb_filter: undefined filter \"$1\".\n"

EDB *edb_filter (EDB *base, const char *filter)
{
	AssignArg *x;
	char *p;

	if	(!filter)	return base;

	while (base && (x = assignarg(filter, &p, " \t\n")) != NULL)
	{
		EDB *next = NULL;
		EDBFilter *def;
		
		if	(!x->name[0])
		{
			next = base;
		}
		else if	(!(def = SearchEfiPar(base->obj->type,
			&EfiPar_EDBFilter, x->name)))
		{
			log_error(edb_err, FMT_UNDEF, "s", x->name);
		}
		else if	(mstrcmp(x->arg, "?") == 0)
		{
			PrintEfiPar(ioerr, def);
			next = NULL;
		}
		else if	(def->create)
		{
			next = def->create(def, base, x->opt, x->arg);
			rd_debug(next, "filter %s[%s]=%s",
				x->name, x->opt, x->arg);
		}
		else	next = base;

		memfree(x);
		filter = p;
		base = next;
	}

	return base;
}
