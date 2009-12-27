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

static InfoNode *node_free = NULL;
static int node_used = 0;
static int node_alloc = 0;

InfoNode *NewInfo (InfoNode *root, char *name)
{
	InfoNode *node;

	if	(node_free == NULL)
	{
		int i;

		node = node_free = lmalloc(NODE_BSIZE * sizeof(InfoNode));

		for (i = 1; i < NODE_BSIZE; i++)
		{
			node->prev = node + 1;
			node++;
		}

		node->prev = NULL;
		node_alloc += NODE_BSIZE;
	}

	node = node_free;
	node_free = node->prev;
	node_used++;

	memset(node, 0, sizeof(InfoNode));
	node->prev = root;
	node->name = name;
	return node;
}
