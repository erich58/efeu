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
#include <EFEU/conv.h>

static EfiFunc *Konverter(const EfiType *old, const EfiType *new)
{
	EfiFunc **ftab;
	int i;

	ftab = old->conv.data;

	for (i = 0; i < old->conv.used; i++)
		if (ftab[i]->type == new) return ftab[i];

	return NULL;
}


static EfiFunc *Konstruktor(const EfiType *old, const EfiType *new)
{
	if	(new->create)
	{
		EfiFuncArg arg;
		arg.type = (EfiType *) old;
		arg.lval = 0;
		arg.noconv = 0;
		arg.name = NULL;
		arg.defval = NULL;
		return rd_deref(XGetFunc(NULL, new->create, &arg, 1));
	}
	else	return NULL;
}


EfiConv *GetConv(EfiConv *conv, const EfiType *old, const EfiType *new)
{
	static EfiConv buf;

	if	(conv == NULL)	conv = &buf;

	conv->func = NULL;
	conv->type = (EfiType *) new;
	conv->dist = 0;

	while (conv->type != NULL)
	{
		if	(conv->type == old)
		{
			conv->func = NULL;
			return conv;
		}
		else if	((conv->func = Konverter(old, conv->type)))
		{
			switch (conv->func->weight)
			{
			case CONV_PROMOTION:
				conv->dist |= D_PROMOTE;
				break;
			case CONV_RESTRICTED:
			case CONV_GENERATED:
				conv->dist |= D_RESTRICTED;
				break;
			default:
				conv->dist |= D_CONVERT;
				break;
			}

			return conv;
		}
		else if	((conv->func = Konstruktor(old, conv->type)))
		{
			conv->dist |= D_CREATE;
			return conv;
		}

		if	(conv->type->dim)	break;

		conv->type = conv->type->base;
		conv->dist = D_EXPAND;
	}

	return NULL;
}


void ConvData(EfiConv *conv, void *tg, void *src)
{
	if	(conv->func)
	{
		conv->func->eval(conv->func, tg, &src);
	}
	else if	(conv->type)
	{
		CopyData(conv->type, tg, src);
	}
}


int ConvDist (const EfiType *old, const EfiType *new)
{
	EfiConv *conv = GetConv(NULL, old, new);
	return conv ? conv->dist : -1;
}
