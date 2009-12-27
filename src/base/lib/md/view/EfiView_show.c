/*
:*:display view parameters
:de:Darstellung der Sichtparameter

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
#include <EFEU/TimeRange.h>
#include <EFEU/printobj.h>

void EfiView_show (EfiView *view, IO *out)
{
	if	(!view || !out)	return;

	PrintType(out, view->data->type, 3);
	io_putc('\n', out);

	if	(view->data != view->base)
	{
		PrintType(out, view->base->type, 3);
		io_putc('\n', out);
	}

	PrintTimeRange(out, view->range_buf.data, view->range_buf.used);
	ShowVarTab(out, "var", view->vtab);
	io_putc('\n', out);

	if	(view->tg)
	{
		PrintType(out, view->tg->type, 3);
		io_putc('\n', out);
	}
}
