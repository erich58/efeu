/*
Bitfeldkomponenten registrieren

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

typedef struct {
	EfiBFMember *def;
	EfiObj *base;
	unsigned data;
} BFDATA;

static EfiObj *bf_alloc (const EfiType *type, va_list list)
{
	EfiObj *obj = Obj_alloc(sizeof(EfiObj) + sizeof(BFDATA));
	BFDATA *bf = (void *) (obj + 1);

	bf->def = va_arg(list, EfiBFMember *);
	bf->base = RefObj(va_arg(list, EfiObj *));
	obj->data = &bf->data;
	return obj;
}

static void bf_free (EfiObj *obj)
{
	BFDATA *bf = (void *) (obj + 1);
	UnrefObj(bf->base);
}

static void bf_update (EfiObj *obj)
{
	BFDATA *bf = (void *) (obj + 1);
	bf->def->update(obj->data, bf->base->data);
}

static void bf_sync (EfiObj *obj)
{
	BFDATA *bf = (void *) (obj + 1);
	bf->def->sync(obj->data, bf->base->data);
	SyncLval(bf->base);
}

static char *bf_ident (const EfiObj *obj)
{
	BFDATA *bf = (void *) (obj + 1);
	return mstrpaste("::", bf->base->type->name, bf->def->name);
}

EfiLval Lval_bf = {
	"bf",
	NULL,
	bf_alloc,
	bf_free,
	bf_update,
	bf_sync,
	NULL,
	bf_ident,
};

static EfiObj *bf_get (const EfiObj *base, void *data)
{
	return base ? LvalObj(&Lval_bf, &Type_uint, data, base) : NULL;
}

/*
Die Funktion |$1| erweitert fie Variablentabelle <tab> um die
Bitfelddefinitionen <def>. Die Bitfelddefinitionen müssen
über die gesamte Laufzeit der Tabelle <tab> verfügbar sein.
*/

void AddEfiBFMember (EfiVarTab *tab, EfiBFMember *def, size_t dim)
{
	for (; dim-- > 0; def++)
	{
		VarTabEntry entry;
		entry.name = def->name;
		entry.desc = mlangcpy(def->desc, NULL);
		entry.type = &Type_uint;
		entry.obj = NULL;
		entry.get = bf_get;
		entry.data = def;
		entry.entry_clean = NULL;
		VarTab_add(tab, &entry);
	}
}
