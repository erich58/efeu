/*
Variablenstack

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

#include <EFEU/object.h>

EfiVarTab *GlobalVar = NULL;
EfiVarTab *LocalVar = NULL;
EfiObj *LocalObj = NULL;

EfiVarTab *ContextVar = NULL;
EfiObj *ContextObj = NULL;

EfiVarStack *VarStack = NULL;
EfiVarStack *ContextStack = NULL;

EfiVarStack *SaveVarStack (void)
{
	EfiVarStack *save;

	PushVarTab(NULL, NULL);
	save = VarStack;
	VarStack = NULL;
	return save;
}

void RestoreVarStack (EfiVarStack *vstack)
{
	VarStack = vstack;
	PopVarTab();
}

#define	USE_ALLOCTAB	0

#if	USE_ALLOCTAB
static ALLOCTAB(stack_tab, 64, sizeof(EfiVarStack));
#endif

void PushVarTab (EfiVarTab *tab, EfiObj *obj)
{
	EfiVarStack *x = VarStack;
#if	USE_ALLOCTAB
	VarStack = new_data(&stack_tab);
#else
	VarStack = memalloc(sizeof(EfiVarStack));
#endif
	VarStack->next = x;
	VarStack->tab = LocalVar;
	VarStack->obj = LocalObj;
	LocalVar = tab;
	LocalObj = obj;
}

void PushContext (EfiVarTab *tab, EfiObj *obj)
{
	EfiVarStack *x = ContextStack;
#if	USE_ALLOCTAB
	ContextStack = new_data(&stack_tab);
#else
	ContextStack = memalloc(sizeof(EfiVarStack));
#endif
	ContextStack->next = x;
	ContextStack->tab = ContextVar;
	ContextStack->obj = ContextObj;
	ContextVar = tab;
	ContextObj = obj;
}


void PopVarTab(void)
{
	if	(VarStack != NULL)
	{
		EfiVarStack *x = VarStack;
		VarStack = x->next;
		DelVarTab(LocalVar);
		UnrefObj(LocalObj);
		LocalVar = x->tab;
		LocalObj = x->obj;
#if	USE_ALLOCTAB
		del_data(&stack_tab, x);
#else
		memfree(x);
#endif
	}
}


void PopContext(void)
{
	if	(ContextStack)
	{
		EfiVarStack *x = ContextStack;
		ContextStack = x->next;
		ContextVar = x->tab;
		DelVarTab(ContextVar);
		UnrefObj(ContextObj);
		ContextObj = x->obj;
#if	USE_ALLOCTAB
		del_data(&stack_tab, x);
#else
		memfree(x);
#endif
	}
}
