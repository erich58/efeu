/*
Datensicht auf Vektorelemente

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

#include <EFEU/EfiView.h>

static void event (EfiView *view, int dat)
{
	EfiVec *vec = Val_ptr(view->base->data);
	int n;

	for (n = 0; n < vec->buf.used; n++)
	{
		CopyData(vec->type, view->eval_obj->data,
			(char *) vec->buf.data + n * vec->buf.elsize);
		EfiView_event(view, dat, 1.);
	}
}

static void eval_range (EfiView *view)
{
	for (view->range_idx = 0;
			view->range_idx < view->range_dim;
			view->range_idx++)
		event(view, view->range[view->range_idx].dat);
}

static void eval_record (EfiView *view)
{
	event(view, 0);
}

static void ve_create (EfiView *view, const char *opt, const char *arg)
{
	EfiVec *vec = Val_ptr(view->base->data);

	if	(view->range_dim)
	{
		view->eval_total = eval_range;
	}
	else	view->eval_total = eval_record;

	view->eval_obj = LvalObj(NULL, vec->type);
}

EfiViewPar EfiView_element = EFI_VIEW (NULL, "element", NULL, ve_create,
	":*:vector element view"
	":de:Sicht auf Vektorelemente"
);
