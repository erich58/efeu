/*
Bitfeldkomponente registrieren

$Copyright (C) 2006 Erich Fr�hst�ck
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

#include <EFEU/mktype.h>

typedef struct {
	OBJECT_VAR;
	EfiObj *base;
	EfiBitfield *def;
} BFPar;

static EfiObj *BFPar_alloc (EfiType *type, va_list list)
{
	BFPar *par = (void *) Obj_alloc(sizeof *par + type->size);
	par->data = (par + 1);
	par->base = RefObj(va_arg(list, EfiObj *));
	par->def = va_arg(list, EfiBitfield *);
	return (EfiObj *) par;
}

static void BFPar_free (EfiObj *obj)
{
	BFPar *par = (void *) obj;
	UnrefObj(par->base);
}

static void BFPar_update (EfiObj *obj)
{
	BFPar *bf = (void *) obj;
	UpdateLval(bf->base);
	bf->def->update(obj->data, bf->base->data);
}

static void BFPar_sync (EfiObj *obj)
{
	BFPar *bf = (void *) obj;
	bf->def->sync(obj->data, bf->base->data);
	SyncLval(bf->base);
}

static char *BFPar_ident (const EfiObj *obj)
{
	BFPar *bf = (void *) obj;
	return mstrpaste("::", bf->base->type->name, bf->def->name);
}

EfiLval Lval_bitfield = {
	"bitfield",
	NULL,
	BFPar_alloc,
	BFPar_free,
	BFPar_update,
	BFPar_sync,
	BFPar_ident,
};

static EfiObj *BFPar_get (const EfiStruct *var, const EfiObj *base)
{
	EfiBitfield *bf = var->par;
	return base ? LvalObj(&Lval_bitfield, var->type, base, bf) : NULL;
}

/*
Die Funktion |$1| erweitert den Datentyp base um eine Bitfeldkomponente
*/

EfiStruct *EfiType_bitfield (EfiType *base, EfiStruct *last, 
	size_t offset, EfiBitfield *bf)
{
	EfiStruct *var;

	var = NewEfiStruct(NULL, NULL, 0);
	var->type = str2Type(bf->type);
	var->name = (char *) bf->name;
	var->desc = mlangcpy(bf->desc, NULL);
	var->offset = offset;
	var->member = BFPar_get;
	var->par = bf;
	return EfiType_addvar(base, last, var);
}