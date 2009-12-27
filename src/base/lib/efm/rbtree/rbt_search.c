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
Red/black trees are binary trees in which the edges are colored either
red or black.  They have the following properties:
1. The number of black edges on every path from the root to a leaf is
constant.
2. No two red edges are adjacent.
Therefore there is an upper bound on the length of every path, it's
O(log n) where n is the number of nodes in the tree.  No path can be
longer than 1+2*P where P is the length of the shortest path in the
tree.
Useful for the implementation:
3. If one of the children of a node is NULL, then the other one is red
(if it exists).

In the implementation, not the edges are colored, but the nodes.  The
color interpreted as the color of the edge leading to this node.  The
color is meaningless for the root node, but we color the root node black
for convenience.  All added nodes are red initially.

Adding to a red/black tree is rather easy.  The right place is searched
with a usual binary tree search.  Additionally, whenever a node N is
reached that has two red successors, the successors are colored black
and the node itself colored red.  This moves red edges up the tree where
they pose less of a problem once we get to really insert the new node.
Changing N's color to red may violate rule 2, however, so rotations may
become necessary to restore the invariants.  Adding a new red leaf may
violate the same rule, so afterwards an additional check is run and the
tree possibly rotated.

Deleting is hairy.  There are mainly two nodes involved: the node to be
deleted (n1), and another node that is to be unchained from the tree
(n2).  If n1 has a successor (the node with a smallest key that is
larger than n1), then the successor becomes n2 and its contents are
copied into n1, otherwise n1 becomes n2.  Unchaining a node may violate
rule 1: if n2 is black, one subtree is missing one black edge
afterwards.  The algorithm must try to move this error upwards towards
the root, so that the subtree that does not have enough black edges
becomes the whole tree.  Once that happens, the error has disappeared.
It may not be necessary to go all the way up, since it is possible that
rotations and recoloring can fix the error before that.

Although the deletion algorithm must walk upwards through the tree, we
do not store parent pointers in the nodes.  Instead, delete allocates a
small array of parent pointers and fills it while descending the tree.
Since we know that the length of a path is O(log n), where n is the
number of nodes, this is likely to use less memory.

Tree rotations look like this:
A                C
/ \              / \
B   C            A   G
/ \ / \  -->     / \
D E F G         B   F
	  / \
	 D   E

In this case, A has been rotated left.  This preserves the ordering of
the binary tree.
*/

/*
Possibly "split" a node with two red successors, and/or fix up two red
edges in a row.  ROOTP is a pointer to the lowest node we visited, PARENTP
and GPARENTP pointers to its parent/grandparent.  P_R and GP_R contain the
comparison values that determined which way was taken in the tree to reach
ROOTP.  MODE is 1 if we need not do the split, but must check for two red
edges between GPARENTP and ROOTP.
*/

static void maybe_split_for_insert (RBTreeNode **rootp,
	RBTreeNode **parentp, RBTreeNode **gparentp,
	int p_r, int gp_r, int mode)
{
  RBTreeNode *root = *rootp;
  RBTreeNode **rp, **lp;
  rp = &(*rootp)->right;
  lp = &(*rootp)->left;

  /* See if we have to split this node (both successors red).  */
  if (mode == 1
      || ((*rp) != NULL && (*lp) != NULL && (*rp)->red && (*lp)->red))
    {
      /* This node becomes red, its successors black.  */
      root->red = 1;
      if (*rp)
	(*rp)->red = 0;
      if (*lp)
	(*lp)->red = 0;

      /* If the parent of this node is also red, we have to do
	 rotations.  */
      if (parentp != NULL && (*parentp)->red)
	{
	  RBTreeNode *gp = *gparentp;
	  RBTreeNode *p = *parentp;
	  /* There are two main cases:
	     1. The edge types (left or right) of the two red edges differ.
	     2. Both red edges are of the same type.
	     There exist two symmetries of each case, so there is a total of
	     4 cases.  */
	  if ((p_r > 0) != (gp_r > 0))
	    {
	      /* Put the child at the top of the tree, with its parent
		 and grandparent as successors.  */
	      p->red = 1;
	      gp->red = 1;
	      root->red = 0;
	      if (p_r < 0)
		{
		  /* Child is left of parent.  */
		  p->left = *rp;
		  *rp = p;
		  gp->right = *lp;
		  *lp = gp;
		}
	      else
		{
		  /* Child is right of parent.  */
		  p->right = *lp;
		  *lp = p;
		  gp->left = *rp;
		  *rp = gp;
		}
	      *gparentp = root;
	    }
	  else
	    {
	      *gparentp = *parentp;
	      /* Parent becomes the top of the tree, grandparent and
		 child are its successors.  */
	      p->red = 0;
	      gp->red = 1;
	      if (p_r < 0)
		{
		  /* Left edges.  */
		  gp->left = p->right;
		  p->right = gp;
		}
	      else
		{
		  /* Right edges.  */
		  gp->right = p->left;
		  p->left = gp;
		}
	    }
	}
    }
}

/*
Find or insert datum into search tree.
KEY is the key to be located, ROOTP is the address of tree root,
COMPAR the ordering function. 
*/

void *rbt_search (RBTree *tree, const void *key)
{
	RBTreeNode *q;
	RBTreeNode **parentp = NULL, **gparentp = NULL;
	RBTreeNode **rootp;
	RBTreeNode **nextp;
	int (*compare) (const void *key, const void *data, size_t size);
	int r = 0, p_r = 0, gp_r = 0; /* No they might not, Mr Compiler.  */

	if	(!tree || !key)
		return NULL;

	compare = tree->compare ? tree->compare : memcmp;
	rootp = &tree->root;

	/* This saves some additional tests below.  */

	if (*rootp != NULL)
		(*rootp)->red = 0;

	nextp = rootp;

	while (*nextp != NULL)
	{
		RBTreeNode *root = *rootp;
		r = compare (key, root + 1, tree->size);

		if	(r == 0)
		{
			root->refcount++;
			return root + 1;
		}

		maybe_split_for_insert (rootp, parentp, gparentp, p_r, gp_r, 0);
	/*
	If that did any rotations, parentp and gparentp are now garbage.
	That doesn't matter, because the values they contain are never
	used again in that case. 
	*/
		nextp = r < 0 ? &root->left : &root->right;

		if	(*nextp == NULL)
			break;

		gparentp = parentp;
		parentp = rootp;
		rootp = nextp;

		gp_r = p_r;
		p_r = r;
	}

	q = rbt_new(tree);

	if	(q != NULL)
	{
		*nextp = q;			/* link new node to old */
		q->red = 1;
		q->refcount = 1;
		q->left = q->right = NULL;

		if	(tree->copy)
		{
			tree->copy(q + 1, key);
		}
		else	memcpy(q + 1, key, tree->size);
	}

	/*
	There may be two red edges in a row now, which we must avoid by
	rotating the tree.
	*/

	if (nextp != rootp)
		maybe_split_for_insert (nextp, rootp, parentp, r, p_r, 1);

	return q;
}
