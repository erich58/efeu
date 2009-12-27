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


#include <EFEU/RBTree.h>

/* Delete node with given key.
   KEY is the key to be deleted, ROOTP is the address of the root of tree,
   COMPAR the comparison function.  */

extern void *alloca (size_t size);

int rbt_delete (RBTree *tree, const void *key)
{
	RBTreeNode *p, *q, *r;
	int cmp;
	RBTreeNode **rootp = &tree->root;
	RBTreeNode *root, *unchained;
	int (*compare) (const void *key, const void *data, size_t size);
	int spos;

/*	Stack of nodes so we remember the parents without recursion.  It's
	_very_ unlikely that there are paths longer than 40 nodes.  The tree
	would need to have around 250.000 nodes. 
*/
	if	(!tree || !tree->root || !key)
		return 0;

	compare = tree->compare ? tree->compare : memcmp;
	rootp = &tree->root;
	spos = tree->nblocks;

	while ((cmp = compare (key, (*rootp) + 1, tree->size)) != 0)
	{
		if	(spos >= tree->stack_size)
			rbt_expand(tree);

		tree->stack[spos++] = rootp;
		rootp = ((cmp < 0) ? &(*rootp)->left : &(*rootp)->right);

		if (*rootp == NULL)
			return 0;
	}

	if	((*rootp)->refcount > 1)
	{
		(*rootp)->refcount--;
		return 1;
	}

	if	(tree->clean)
		tree->clean((*rootp) + 1);

/*	We don't unchain the node we want to delete. Instead, we overwrite it
	with its successor and unchain the successor.  If there is no
	successor, we really unchain the node to be deleted. 
*/
	root = *rootp;
	r = root->right;
	q = root->left;

	if	(q == NULL || r == NULL)
	{
		unchained = root;
	}
	else
	{
		RBTreeNode **parent = rootp, **up = &root->right;

		for (;;)
		{
			if	(spos >= tree->stack_size)
				rbt_expand(tree);

			tree->stack[spos++] = parent;
			parent = up;

			if ((*up)->left == NULL)
				break;

			up = &(*up)->left;
		}

		unchained = *up;
	}

/*	We know that either the left or right successor of UNCHAINED is NULL.
	R becomes the other one, it is chained into the parent of UNCHAINED.
*/
	r = unchained->left;

	if	(r == NULL)
		r = unchained->right;

	if	(spos > tree->nblocks)
	{
		q = * (RBTreeNode **) tree->stack[spos - 1];

		if	(unchained == q->right)	q->right = r;
		else				q->left = r;
	}
	else	*rootp = r;

	if	(unchained != root)
		memcpy(root + 1, unchained + 1, tree->size);

	if (!unchained->red)
	{
/*	Now we lost a black edge, which means that the number of black edges on
	every path is no longer constant.  We must balance the tree.
	NODESTACK now contains all parents of R.  R is likely to be NULL
	in the first iteration.
	NULL nodes are considered black throughout - this is necessary for
	correctness.
*/
	while (spos > tree->nblocks && (r == NULL || !r->red))
	{

	  RBTreeNode **pp = tree->stack[spos - 1];
	  p = *pp;
	  /* Two symmetric cases.  */
	  if (r == p->left)
	    {
	      /* Q is R's brother, P is R's parent.  The subtree with root
		 R has one black edge less than the subtree with root Q.  */
	      q = p->right;
	      if (q != NULL && q->red)
		{
		/*	If Q is red, we know that P is black. We rotate P left
			so that Q becomes the top node in the tree, with P
			below it.  P is colored red, Q is colored black.  This
			action does not change the black edge count for any
			leaf in the tree, but we will be able to recognize one
			of the following situations, which all require that Q
			is black.
		*/
		  q->red = 0;
		  p->red = 1;
		  /* Left rotate p.  */
		  p->right = q->left;
		  q->left = p;
		  *pp = q;
		  /* Make sure pp is right if the case below tries to use
		     it.  */
		  tree->stack[spos++] = pp = &q->left;
		  q = p->right;
		}
	      /* We know that Q can't be NULL here.  We also know that Q is
		 black.  */
	      if ((q->left == NULL || !q->left->red)
		  && (q->right == NULL || !q->right->red))
		{
		  /* Q has two black successors.  We can simply color Q red.
		     The whole subtree with root P is now missing one black
		     edge.  Note that this action can temporarily make the
		     tree invalid (if P is red).  But we will exit the loop
		     in that case and set P black, which both makes the tree
		     valid and also makes the black edge count come out
		     right.  If P is black, we are at least one step closer
		     to the root and we'll try again the next iteration.  */
		  q->red = 1;
		  r = p;
		}
	      else
		{
		  /* Q is black, one of Q's successors is red.  We can
		     repair the tree with one operation and will exit the
		     loop afterwards.  */
		  if (q->right == NULL || !q->right->red)
		    {
		      /* The left one is red.  We perform the same action as
			 in maybe_split_for_insert where two red edges are
			 adjacent but point in different directions:
			 Q's left successor (let's call it Q2) becomes the
			 top of the subtree we are looking at, its parent (Q)
			 and grandparent (P) become its successors. The former
			 successors of Q2 are placed below P and Q.
			 P becomes black, and Q2 gets the color that P had.
			 This changes the black edge count only for node R and
			 its successors.  */
		      RBTreeNode *q2 = q->left;
		      q2->red = p->red;
		      p->right = q2->left;
		      q->left = q2->right;
		      q2->right = q;
		      q2->left = p;
		      *pp = q2;
		      p->red = 0;
		    }
		  else
		    {
		      /* It's the right one.  Rotate P left. P becomes black,
			 and Q gets the color that P had.  Q's right successor
			 also becomes black.  This changes the black edge
			 count only for node R and its successors.  */
		      q->red = p->red;
		      p->red = 0;

		      q->right->red = 0;

		      /* left rotate p */
		      p->right = q->left;
		      q->left = p;
		      *pp = q;
		    }

		  /* We're done.  */
		  spos = tree->nblocks + 1;
		  r = NULL;
		}
	    }
	  else
	    {
	      /* Comments: see above.  */
	      q = p->left;
	      if (q != NULL && q->red)
		{
		  q->red = 0;
		  p->red = 1;
		  p->left = q->right;
		  q->right = p;
		  *pp = q;
		  tree->stack[spos++] = pp = &q->right;
		  q = p->left;
		}
	      if ((q->right == NULL || !q->right->red)
		       && (q->left == NULL || !q->left->red))
		{
		  q->red = 1;
		  r = p;
		}
	      else
		{
		  if (q->left == NULL || !q->left->red)
		    {
		      RBTreeNode *q2 = q->right;
		      q2->red = p->red;
		      p->left = q2->right;
		      q->right = q2->left;
		      q2->left = q;
		      q2->right = p;
		      *pp = q2;
		      p->red = 0;
		    }
		  else
		    {
		      q->red = p->red;
		      p->red = 0;
		      q->left->red = 0;
		      p->left = q->right;
		      q->right = p;
		      *pp = q;
		    }
		  spos = tree->nblocks + 1;
		  r = NULL;
		}
	    }
	  --spos;
	}

		if (r != NULL)
			r->red = 0;
	}

	unchained->right = tree->store;
	tree->store = unchained;
	return 2;
}
