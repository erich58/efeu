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
#include <EFEU/RBTree.h>
#include <EFEU/memalloc.h>

/*
The standardized functions miss an important functionality: the
tree cannot be removed easily.  We provide a function to do this.
*/

static void tdestroy_recurse (RBTreeNode *root, void (*freefct)(void *))
{
	if	(root->left)
		tdestroy_recurse (root->left, freefct);

	if	(root->right)
		tdestroy_recurse (root->right, freefct);

	freefct (root + 1);
}

void rbt_destroy (RBTree *tree)
{
	if	(!tree)	return;

	if	(tree->root && tree->clean)
		tdestroy_recurse (tree->root, tree->clean);

	tree->root = NULL;
	tree->store = NULL;

	while (tree->nblocks)
		lfree(tree->stack[--tree->nblocks]);

	lfree(tree->stack);
	tree->stack = NULL;
	tree->stack_size = 0;
}
