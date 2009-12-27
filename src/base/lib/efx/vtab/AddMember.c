/*
Mitgliederdefinition registrieren

$Copyright (C) 2001 Erich Frühstück
This file is part of EFEU.

EFEU is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

EFEU is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public
License along with EFEU; see the file COPYING.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <EFEU/object.h>

static void var_clean (Var_t *var)
{
	memfree((char *) var->name);
	memfree((char *) var->desc);
}

void AddMember (VarTab_t *tab, MemberDef_t *def, size_t dim)
{
	Var_t *var;

	if	(tab == NULL)
	{
		if	(LocalVar == NULL)
			LocalVar = VarTab(NULL, 0);

		tab = LocalVar;
	}

	while (dim-- > 0)
	{
		var = NewVar(NULL, def->name, 0);
		var->type = def->type;
		var->member = def->member;
		var->par = def->par;
		var->desc = mlangcpy(def->desc, NULL);
		var->clean = (clean_t) var_clean;
		AddVar(tab, var, 1);
		def++;
	}
}
