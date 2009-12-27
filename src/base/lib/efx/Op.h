/*
Operatordefinitionen

$Header <EFEU/$1>

$Copyright (C) 1994 Erich Frühstück
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

#ifndef	EFEU_OPERATOR_H
#define	EFEU_OPERATOR_H	1

#include <EFEU/object.h>

/*	Falls der Makro COMMA_LIST auf 1 gesetzt ist, generiert
	der Kommaoperator Listen. Ansonsten wirkt er wie in C.
*/

#define	COMMA_LIST	1

#define	OpPrior_Comma	2
#define	OpPrior_Assign	4
#define	OpPrior_Range	6
#define	OpPrior_Cond	8

#define	OpPrior_Or	10
#define	OpPrior_And	12
#define	OpPrior_BitOr	14
#define	OpPrior_BitXor	16
#define	OpPrior_BitAnd	18

#define	OpPrior_Equal	20
#define	OpPrior_Compare	22
#define	OpPrior_Shift	24

#define	OpPrior_Add	26
#define	OpPrior_Mult	28
#define	OpPrior_Power	30

#define	OpPrior_Unary	32
#define	OpPrior_Member	34
#define	OpPrior_Scope	36

#define	OpAssoc_Left	0	/* Links Assoziativ */
#define	OpAssoc_Right	1	/* Rechts Assoziativ */

typedef struct Op_s {
	char *name;	/* Operatorname */
	char prior;	/* Priorität */
	char assoc;	/* Assoziativität */
	Obj_t *(*parse) (io_t *io, struct Op_s *op, Obj_t *left);
} Op_t;

Obj_t *PrefixOp (io_t *io, Op_t *op, Obj_t *left);
Obj_t *PostfixOp (io_t *io, Op_t *op, Obj_t *left);
Obj_t *BinaryOp (io_t *io, Op_t *op, Obj_t *left);
Obj_t *AssignOp (io_t *io, Op_t *op, Obj_t *left);

void StdOpDef (void);
void AddOpDef (xtab_t *tab, Op_t *def, size_t dim);

extern xtab_t PrefixTab;
extern xtab_t PostfixTab;


Obj_t *CondTerm (Obj_t *test, Obj_t *ifpart, Obj_t *elsepart);
Obj_t *RangeTerm (Obj_t *first, Obj_t *last, Obj_t *step);
Obj_t *CommaTerm (Obj_t *left, Obj_t *right);
Obj_t *UnaryTerm (const char *a, Obj_t *obj, int flag);
Obj_t *BinaryTerm (const char *a, Obj_t *left, Obj_t *right);

#endif	/* EFEU_OPERATOR_H */
