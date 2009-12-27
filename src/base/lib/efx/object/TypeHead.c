/*
Headerzeilen für Datentype ausgeben

$Copyright (C) 2005 Erich Frühstück
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
#include <ctype.h>

static int add_head (StrBuf *sb, const EfiType *type)
{
	if	(type == NULL)
	{
		return 0;
	}
	else if	(type->src)
	{
		if	(sb->pos)
			sb_putc('\n', sb);

		sb_puts(type->src->cmd, sb);
		return 1;
	}
	else if	(type->list)
	{
		int n;
		EfiStruct *v;

		for (n = 0, v = type->list; v; v = v->next)
			n += add_head(sb, v->type);
	}
	else	return add_head(sb, type->base);

	return 0;
}

char *TypeHead (const EfiType *type)
{
	if	(type == NULL)	return NULL;

	if	(type->src)
	{
		return mstrcpy(type->src->cmd);
	}
	else if	(type->list)
	{
		StrBuf *sb = sb_create(0);
		add_head(sb, type);
		return sb2str(sb);
	}
	else	return TypeHead(type->base);

	return NULL;
}
