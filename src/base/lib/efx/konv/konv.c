/*
Funktionsargumente konvertieren

$Copyright (C) 1996 Erich Frühstück
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
#include <EFEU/konvobj.h>

static Func_t *Konverter(const Type_t *old, const Type_t *new)
{
	Func_t **ftab;
	int i;

	ftab = old->konv.data;

	for (i = 0; i < old->konv.used; i++)
		if (ftab[i]->type == new) return ftab[i];

	return NULL;
}


static Func_t *Konstruktor(const Type_t *old, const Type_t *new)
{
	if	(new->create)
	{
		FuncArg_t arg;
		arg.type = (Type_t *) old;
		arg.lval = 0;
		arg.nokonv = 0;
		arg.name = NULL;
		arg.defval = NULL;
		return XGetFunc(NULL, new->create, &arg, 1);
	}
	else	return NULL;
}


Konv_t *GetKonv(Konv_t *konv, const Type_t *old, const Type_t *new)
{
	static Konv_t buf;

	if	(konv == NULL)	konv = &buf;

	konv->func = NULL;
	konv->type = (Type_t *) new;
	konv->dist = 0;

	while (konv->type != NULL)
	{
		if	(konv->type == old)
		{
			konv->func = NULL;
			return &buf;
		}
		else if	((konv->func = Konverter(old, konv->type)))
		{
			switch (konv->func->weight)
			{
			case KONV_PROMOTION:
				konv->dist |= D_PROMOTE;
				break;
			case KONV_RESTRICTED:
				konv->dist |= D_RESTRICTED;
				break;
			default:
				konv->dist |= D_KONVERT;
				break;
			}

			return konv;
		}
		else if	((konv->func = Konstruktor(old, konv->type)))
		{
			konv->dist |= D_CREATE;
			return konv;
		}

		if	(konv->type->dim)	break;

		konv->type = konv->type->base;
		konv->dist = D_EXPAND;
	}

	return NULL;
}


void KonvData(Konv_t *konv, void *tg, void *src)
{
	if	(konv->func)
	{
		konv->func->eval(konv->func, tg, &src);
	}
	else if	(konv->type)
	{
		CopyData(konv->type, tg, src);
	}
}


int KonvDist (const Type_t *old, const Type_t *new)
{
	Konv_t *konv = GetKonv(NULL, old, new);
	return konv ? konv->dist : -1;
}
