/*	Befehls/Makrotabelle abrufen
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
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
