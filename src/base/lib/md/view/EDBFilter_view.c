/*	view - filter

$Copyright (C) 2007 Erich Frühstück
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
#include <EFEU/EfiView.h>
#include <ctype.h>

static int view_read (EfiType *type, void *data, void *par)
{
	EfiView *view = par;

	if	(!view->nsave)
	{
		io_rewind(view->out);

		do
		{
			if	(!edb_read(view->edb))
				return 0;
		
			EfiView_process(view);
		}
		while (!view->nsave);

		if	(view->nsave == 1)
		{
			view->nsave = 0;
			return 1;
		}

		io_rewind(view->out);
	}

	ReadData(view->tg->type, view->tg->data, view->out);
	view->nsave--;
	return 1;
}

static EDB *fdef_view (EDBFilter *filter, EDB *base,
	const char *opt, const char *arg)
{
	EDB *edb;
	EfiView *view;
	
	view = EfiView_create(base->obj, opt);
	view->edb = base;

	if	(!arg || !*arg)
	{
		EfiView_list(ioerr, NULL, view->vtab);
		exit(EXIT_SUCCESS);
	}

	EfiView_var(view, arg);
	view->out = io_bigbuf(0, NULL);

	edb = edb_alloc(RefObj(view->tg), mstrcpy(base->desc));
	edb->read = view_read;
	edb->ipar = view;
	return edb;
}

EDBFilter EDBFilter_view = EDB_FILTER(NULL,
	"view", "[weight*member:method;range]=varlist", fdef_view, NULL,
	":*:select members from data view"
	":de:Auswahl von Komponenten einer Datensicht"
);
