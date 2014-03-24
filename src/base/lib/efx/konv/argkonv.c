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

static EfiFunc *KonvFunc(const EfiType *old, const EfiType *new)
{
	EfiFunc **ftab;
	int i;

	if	(old == NULL)	return NULL;

	ftab = old->konv.data;

	for (i = 0; i < old->konv.used; i++)
		if (ftab[i]->type == new) return ftab[i];

	return NULL;
}

EfiArgKonv *GetArgKonv(const EfiType *old, const EfiType *new)
{
	static EfiArgKonv buf;

	buf.type = NULL;
	buf.dist = 0;

	for (; new != NULL; new = new->dim ? NULL : new->base)
	{
		buf.func = NULL;

		if	(old == new)	return &buf;

		if	((buf.func = KonvFunc(old, new)))
		{
			switch (buf.func->weight)
			{
			case KONV_PROMOTION:
				buf.dist |= D_PROMOTE;
				break;
			case KONV_RESTRICTED:
				buf.dist |= D_RESTRICTED;
				break;
			default:
				buf.dist |= D_KONVERT;
				break;
			}

			return &buf;
		}

		buf.type = (EfiType *) new;
		buf.dist = D_EXPAND;
	}

	return NULL;
}

void ArgKonv(EfiArgKonv *konv, void *tg, void *src)
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

int ArgKonvDist (const EfiType *old, const EfiType *new)
{
	EfiArgKonv *konv = GetArgKonv(old, new);
	return konv ? konv->dist : D_RESTRICTED;
}
