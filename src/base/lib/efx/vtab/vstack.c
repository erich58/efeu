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

VarTab_t *GlobalVar = NULL;
VarTab_t *LocalVar = NULL;
Obj_t *LocalObj = NULL;

VarTab_t *ContextVar = NULL;
Obj_t *ContextObj = NULL;

VarStack_t *VarStack = NULL;
VarStack_t *ContextStack = NULL;

VarStack_t *SaveVarStack (void)
{
	VarStack_t *save;

	PushVarTab(NULL, NULL);
	save = VarStack;
	VarStack = NULL;
	return save;
}

void RestoreVarStack (VarStack_t *vstack)
{
	VarStack = vstack;
	PopVarTab();
}


void PushVarTab (VarTab_t *tab, Obj_t *obj)
{
	VarStack_t *x = VarStack;
	VarStack = memalloc(sizeof(VarStack_t));
	VarStack->next = x;
	VarStack->tab = LocalVar;
	VarStack->obj = LocalObj;
	LocalVar = tab;
	LocalObj = obj;
}

void PushContext (VarTab_t *tab, Obj_t *obj)
{
	VarStack_t *x = ContextStack;
	ContextStack = memalloc(sizeof(VarStack_t));
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
		VarStack_t *x = VarStack;
		VarStack = x->next;
		DelVarTab(LocalVar);
		UnrefObj(LocalObj);
		LocalVar = x->tab;
		LocalObj = x->obj;
		memfree(x);
	}
}


void PopContext(void)
{
	if	(ContextStack)
	{
		VarStack_t *x = ContextStack;
		ContextStack = x->next;
		ContextVar = x->tab;
		DelVarTab(ContextVar);
		UnrefObj(ContextObj);
		ContextObj = x->obj;
		memfree(x);
	}
}
