/*
:*:red black trees with reference counter
:de:Rot-Schwarz-Bäume mit Refernzzähler

$Header	<EFEU/$1>

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

#ifndef	EFEU_RBTree_h
#define	EFEU_RBTree_h	1

#include <EFEU/extension.h>
#include <EFEU/refdata.h>
#include <limits.h>
#include <search.h>

#ifndef	CHAR_BIT
#define	CHAR_BIT	8
#endif

typedef struct RBTreeNode RBTreeNode;

struct RBTreeNode {
	RBTreeNode *left;
	RBTreeNode *right;
	size_t red : 1; 
	size_t refcount : CHAR_BIT * sizeof(size_t) - 1;
};

typedef struct {
	size_t size;	/* Size of key */
	int (*compare) (const void *key, const void *data, size_t size);
	void (*copy) (void *tg, const void *src);
	void (*clean) (void *data);

/*	Private data
*/
	RBTreeNode *root;	/* root of tree */
	RBTreeNode *store;	/* node chunk store */

/*	Stack of pointers to permanent store allocated blocks and temporarly
	store node pointers by rbt_delete.
*/
	void **stack;		/* pointer vector */
	size_t stack_size;	/* Size of vector */
	size_t nblocks;		/* number of blocks */
	size_t bsize;		/* block size */
} RBTree;

/*	Internal used functions
*/

void rbt_expand (RBTree *tree);
RBTreeNode *rbt_new (RBTree *tree);

/*	Node state functions
*/

size_t rbt_count (const void *data);
int rbt_isred (const void *data);

/*	Tree declaration macros
*/

#define RBT_DATA(size,cmp,copy,clean)	\
	{ (size), cmp, copy, clean, NULL, NULL, NULL, 0, 0, 0 }

#define RBT_DECL(name,size,cmp,copy,clean)	\
	RBTree name = RBT_DATA(size,cmp,copy,clean)

/*	Tree manipulation and query functions
*/

void *rbt_search (RBTree *tree, const void *key);
void *rbt_find (RBTree *tree, const void *key);
int rbt_delete (RBTree *tree, const void *key);
void rbt_destroy (RBTree *tree);
void rbt_walk (RBTree *tree, void (*action) (const void *entry,
	const VISIT which, const int depth));
void *rbt_pwalk (RBTree *tree, void *(*action) (const void *entry,
	void *ptr), void *ptr);

#endif	/* EFEU/RBTree.h */
