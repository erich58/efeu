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

#define	ERR	"sorry: malloc(%lu) failed\n"

#define	PAGESIZE	8192
#define	OVERHEAD	(2 * sizeof(size_t))	/* Malloc - Overhead */
#define	MINBSIZE	255

#define	STACK_OVERHEAD	2
#define	STACK_BSIZE	32

void rbt_expand (RBTree *tree)
{
	size_t n;

	if	(tree->stack_size)
	{
		tree->stack_size += tree->stack_size + STACK_OVERHEAD;
	}
	else	tree->stack_size = STACK_BSIZE - STACK_OVERHEAD;

	n = tree->stack_size * sizeof tree->stack[0];
	tree->stack = realloc(tree->stack, n);

	if	(tree->stack == NULL)
	{
		fprintf(stderr, ERR, (unsigned long) n);
		exit(EXIT_FAILURE);
	}
}

static void load (RBTree *tree)
{
	size_t n, size;
	RBTreeNode *node;
	void *p;

	if	(tree->stack_size <= tree->nblocks)
		rbt_expand(tree);

	size = sizeof *tree->store + tree->size;

	if	(!tree->bsize)
	{
		tree->bsize = PAGESIZE / size;

		if	(tree->bsize < MINBSIZE)
			tree->bsize = MINBSIZE;

		n = (tree->bsize * size + OVERHEAD + PAGESIZE - 1) / PAGESIZE;
		tree->bsize = (n * PAGESIZE - OVERHEAD) / size;
	}

	p = malloc(size * tree->bsize);

	if	(p == NULL)
	{
		fprintf(stderr, ERR, (unsigned long) (size * tree->bsize));
		exit(EXIT_FAILURE);
	}

	tree->stack[tree->nblocks++] = p;

	for (n = 0; n < tree->bsize; n++)
	{
		node = p;
		node->right = tree->store;
		tree->store = node;
		p = (char *) p + size;
	}
}

RBTreeNode *rbt_new (RBTree *tree)
{
	RBTreeNode *node;

	if	(!tree)	return NULL;

	if	(!tree->store)
		load(tree);

	node = tree->store;
	tree->store = node->right;
	return node;
}

size_t rbt_count (const void *data)
{
	return data ? (((const RBTreeNode *) data) - 1)->refcount : 0;
}

int rbt_isred (const void *data)
{
	return data ? (((const RBTreeNode *) data) - 1)->red : 0;
}
