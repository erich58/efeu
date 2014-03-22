/*
:*:red black trees with reference count
:de:Rot-Schwarz-Bäume mit Referenzzähler

$Copyright (C) 2008 Erich Frühstück

This code is based on tsearch.c from the GNU C Library.
Copyright (C) 1995, 1996, 1997, 2000 Free Software Foundation, Inc.
Contributed by Bernd Schmidt <crux@Pool.Informatik.RWTH-Aachen.DE>, 1997.

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

/*
Tree search for red/black trees.
The algorithm for adding nodes is taken from one of the many "Algorithms"
books by Robert Sedgewick, although the implementation differs.
The algorithm for deleting nodes can probably be found in a book named
"Introduction to Algorithms" by Cormen/Leiserson/Rivest.  At least that's
the book that my professor took most algorithms from during the "Data
Structures" course...

Totally public domain.
*/

#include <stdlib.h>
#include <string.h>
#include <search.h>
#include <EFEU/RBTree.h>

/*
Find datum in search tree.  KEY is the key to be located.
*/

void *rbt_find (RBTree *tree, const void *key)
{
	RBTreeNode *root;
	int (*compare) (const void *key, const void *data, size_t size);

	if	(!tree || !tree->root || !key)
		return NULL;

	compare = tree->compare ? tree->compare : memcmp;
	root = tree->root;

	while (root)
	{
		int r = compare (key, root + 1, tree->size);

		if (r == 0)
			return root + 1;

		root = r < 0 ? root->left : root->right;
	}

	return NULL;
}
