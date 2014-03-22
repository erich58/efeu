/*
Klassifikation eintragen

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

#include <EFEU/EfiClass.h>

typedef struct {
	EfiType *type;
	int (*update) (const EfiObj *base, void *par);
	void *par;
} CPAR;

static EfiObj *get_class (const EfiObj *base, void *data)
{
	CPAR *par = data;
	int val = par->update(base, par->par);
	return NewObj(par->type, &val);
}

static void class_clean (VarTabEntry *entry)
{
	memfree((void *) entry->name);
	memfree(entry->desc);
	memfree(entry->data);
}

void AddEfiClass (EfiVarTab *tab, const char *name, const char *desc,
	EfiType *type, int (*update) (const EfiObj *, void *), void *data)
{
	VarTabEntry entry;
	CPAR *par;

	par = memalloc(sizeof *par);
	par->type = type;
	par->update = update;
	par->par = data;

	entry.name = mstrcpy(name);
	entry.desc = mlangcpy(desc, NULL);
	entry.type = type;
	entry.obj = NULL;
	entry.get = get_class;
	entry.data = par;
	entry.entry_clean = class_clean;
	VarTab_add(tab, &entry);
}

void AddEfiClassDef (EfiVarTab *tab, EfiClassDef *def, size_t dim)
{
	for (; dim-- > 0; def++)
	{
		EfiType *type = NewEnumType(NULL, EnumTypeRecl(def->dim + 1));
		Label *label = def->label;
		int val = 1;

		for (; val <= def->dim; val++, label++)
			VarTab_xadd(type->vtab, label->name, label->desc,
				NewObj(type, &val));

		type = AddEnumType(type);
		AddEfiClass(tab, def->name, def->desc,
			type, def->update, def->par);
	}
}
