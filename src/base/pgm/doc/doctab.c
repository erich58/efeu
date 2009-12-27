/*
Befehls/Makrotabelle abrufen

$Copyright (C) 1999 Erich Frühstück
This file is part of EFEU.

EFEU is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

EFEU is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public
License along with EFEU; see the file COPYING.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include "efeudoc.h"
#include <ctype.h>

static int testtab (const char *name, DocTab_t *tab)
{
	return (name == NULL || (tab && mstrcmp(name, tab->name) == 0));
}

DocTab_t *Doc_gettab (Doc_t *doc, const char *name)
{
	stack_t *ptr;

	if	(!(doc && doc->cmd_stack))	return NULL;

	for (ptr = doc->cmd_stack; ptr != NULL; ptr = ptr->next)
		if (testtab(name, ptr->data)) return ptr->data;

	return NULL;
}

void Doc_pushvar (Doc_t *doc)
{
	PushVarTab(RefVarTab(Type_Doc.vtab), ConstObj(&Type_Doc, &doc));

	if	(doc && doc->cmd_stack)
	{
		DocTab_t *tab = doc->cmd_stack->data;
		PushVarTab(RefVarTab(tab->var), NULL);
	}
	else	PushVarTab(NULL, NULL);
}

void Doc_popvar (Doc_t *doc)
{
	PopVarTab();
	PopVarTab();
}

VarTab_t *Doc_vartab (Doc_t *doc)
{
	if	(doc && doc->cmd_stack)
	{
		DocTab_t *tab = doc->cmd_stack->data;
		return tab->var;
	}
	else	return NULL;
}
