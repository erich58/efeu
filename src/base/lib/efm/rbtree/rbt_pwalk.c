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
Walk the nodes of a tree.
*/

static void *trecurse (const RBTreeNode *root,
	void *(*action) (const void *entry, void *ptr), void *ptr)
{
	void *data = NULL;

	if	(root->left && (data = trecurse(root->left, action, ptr)))
		return data;

	if	((data = action(root + 1, ptr)))
		return data;

	if	(root->right && (data = trecurse(root->right, action, ptr)))
		return data;

	return NULL;
}


/*
Walk the nodes of a tree.
ROOT is the root of the tree to be walked, ACTION the function to be
called at each node. 
*/

void *rbt_pwalk (RBTree *tree,
	void *(*action) (const void *entry, void *ptr), void *ptr)
{
	if	(tree && tree->root && action)
		return trecurse(tree->root, action, ptr);
	return NULL;
}
