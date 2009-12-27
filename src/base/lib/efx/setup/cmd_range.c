/*
Bereichslistengenerierung

$Copyright (C) 1994 Erich Frühstück
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

#include <EFEU/object.h>
#include <EFEU/cmdsetup.h>

#define	VLFUNC	FUNC_VIRTUAL, &Type_list

static EfiFuncDef fdef_range[] = {
	{ VLFUNC, "operator: (int a, int b)", RangeFunc },
	{ VLFUNC, "operator: (int a, int b, int s)", RangeFunc },
	{ VLFUNC, "operator: (unsigned a, unsigned b)", RangeFunc },
	{ VLFUNC, "operator: (unsigned a, unsigned b, unsigned s)", RangeFunc },
	{ VLFUNC, "operator: (int64_t a, int64_t b)", RangeFunc },
	{ VLFUNC, "operator: (int64_t a, int64_t b, int64_t s)", RangeFunc },
	{ VLFUNC, "operator: (uint64_t a, uint64_t b)", RangeFunc },
	{ VLFUNC, "operator: (uint64_t a, uint64_t b, uint64_t s)", RangeFunc },
	{ VLFUNC, "operator: (double a, double b)", RangeFunc },
	{ VLFUNC, "operator: (double a, double b, double s)", RangeFunc },
};

/*	Initialisierung
*/

void CmdSetup_range(void)
{
	AddFuncDef(fdef_range, tabsize(fdef_range));
}
