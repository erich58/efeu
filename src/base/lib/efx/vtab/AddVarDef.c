/*
Variablendefinitionen registrieren

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

/*
Die Funktion |$1| erweitert die Variablentabelle <tab> um die
Variablendefinitionen <def>. Die Variablendefinitionen müssen
über die gesamte Laufzeit der Tabelle <tab> verfügbar sein.
*/

void AddVarDef (EfiVarTab *tab, EfiVarDef *def, size_t dim)
{
	tab = CurrentVarTab(tab);

	for (; dim-- > 0; def++)
	{
		VarTabEntry entry;
		entry.name = def->name;
		entry.desc = mlangcpy(def->desc, NULL);
		entry.type = def->type;
		entry.obj = LvalObj(&Lval_ptr, def->type, def->data);
		entry.get = NULL;
		entry.data = def;
		entry.entry_clean = NULL;
		VarTab_add(tab, &entry);
	}
}
