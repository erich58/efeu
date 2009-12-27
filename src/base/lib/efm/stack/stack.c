/*
Stackstruktur

$Copyright (C) 1996 Erich Frühstück
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

#include <EFEU/stack.h>
#include <EFEU/memalloc.h>

#define	STACK_BSIZE	63	/* Blockgröße für Stacksegmente */

static ALLOCTAB(Stacktab, "Stack", STACK_BSIZE, sizeof(Stack));

void pushstack (Stack **ptr, void *data)
{
	Stack *par = new_data(&Stacktab);
	par->next = *ptr;
	par->data = data;
	*ptr = par;
}

void *popstack (Stack **ptr, void *data)
{
	if	(*ptr)
	{
		Stack *par = *ptr;
		*ptr = par->next;
		data = par->data;
		del_data(&Stacktab, par);
	}

	return data;
}
