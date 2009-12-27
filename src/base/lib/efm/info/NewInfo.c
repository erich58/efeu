/*
Neuen Informationsknoten generieren

$Copyright (C) 1998 Erich Frühstück
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

#include <EFEU/Info.h>
#include <EFEU/mstring.h>

#define	NODE_BSIZE	64

static ALLOCTAB(info_tab, 0, sizeof(InfoNode));

static void entry_clean (void *data)
{
	rd_deref(*((InfoNode **) data));
}

static void info_clean (void *ptr)
{
	InfoNode *info = ptr;
	memfree(info->name);
	memfree(info->label);
	rd_deref(info->prev);
	vb_clean(info->list, entry_clean);
	del_data(&info_tab, info);
}

static char *info_ident (const void *ptr)
{
	StrBuf *sb = sb_create(0);
	IO *io = io_strbuf(sb);
	InfoName(io, NULL, (InfoNode *) ptr);
	io_close(io);
	return sb2str(sb);
}

RefType Info_reftype = REFTYPE_INIT("InfoNode", info_ident, info_clean);

InfoNode *NewInfo (InfoNode *root, char *name)
{
	InfoNode *node;
	
	node = rd_init(&Info_reftype, new_data(&info_tab));
	node->prev = rd_refer(root);
	node->name = name;
	return node;
}
