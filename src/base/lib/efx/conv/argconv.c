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

static EfiFunc *ConvFunc(const EfiType *src, const EfiType *tg)
{
	EfiFunc **ftab;
	int i;

	if	(src == NULL)	return NULL;

	ftab = src->conv.data;

	for (i = 0; i < src->conv.used; i++)
		if (ftab[i]->type == tg) return ftab[i];

	return NULL;
}

EfiConv *GetArgConv(const EfiType *src, const EfiType *tg)
{
	static EfiConv buf;

	buf.type = NULL;
	buf.dist = 0;

	for (; tg != NULL; tg = tg->dim ? NULL : tg->base)
	{
		buf.func = NULL;

		if	(src == tg)	return &buf;

		if	((buf.func = ConvFunc(src, tg)))
		{
			switch (buf.func->weight)
			{
			case CONV_PROMOTION:
				buf.dist |= D_PROMOTE;
				break;
			case CONV_RESTRICTED:
			case CONV_GENERATED:
				buf.dist |= D_RESTRICTED;
				break;
			default:
				buf.dist |= D_CONVERT;
				break;
			}

			return &buf;
		}

		buf.type = (EfiType *) tg;
		buf.dist = D_EXPAND;
	}

	return NULL;
}

void ArgConv(EfiConv *conv, void *tg, void *src)
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
