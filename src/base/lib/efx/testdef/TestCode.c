/*
:*:	test code lists
:de:	Kodelisten testen

$Copyright (C) 1997, 2005 Erich Frühstück
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

#include <EFEU/efutil.h>
#include <EFEU/TestDef.h>

struct CLIST {
	struct CLIST *next;
	unsigned code;
};

struct CPAR {
	int flag;
	struct CLIST *list;
};

void *MakeCodePar (const char *opt, const char *arg, void *par)
{
	unsigned (*code)(const char *name);
	struct CPAR *cpar;
	struct CLIST **ptr;
	int i, dim;
	char **list;

	if	(arg == NULL || par == NULL)	return NULL;

	code = par;
	cpar = memalloc(sizeof(struct CLIST));

	switch (arg[0])
	{
	case '!':	cpar->flag = 0; arg++; break;
	case '^':	cpar->flag = 0; arg++; break;
	case '=':	cpar->flag = 1; arg++; break;
	default:	cpar->flag = 1; break;
	}

	dim = mstrsplit(arg, ",;", &list);
	cpar->list = NULL;
	ptr = &cpar->list;

	for (i = 0; i < dim; i++)
	{
		*ptr = memalloc(sizeof(struct CLIST));
		(*ptr)->code = code(list[i]);
		(*ptr)->next = NULL;
		ptr = &(*ptr)->next;
	}
	
	memfree(list);
	return cpar;
}


int TestCode (void *par, unsigned code)
{
	struct CPAR *ptr;
	struct CLIST *list;

	if	(par == NULL)	return 1;

	ptr = par;

	for (list = ptr->list; list != NULL; list = list->next)
		if (code == list->code) return ptr->flag;

	return !ptr->flag;
}


TestParDef TestPar_a37l = { "=[!^]codelist",
	MakeCodePar, memfree, a37l
};


void *MakeEnumPar (const char *opt, const char *arg, void *par)
{
	struct CPAR *cpar;
	struct CLIST **ptr;
	EfiType *type;
	EfiObj *obj;
	int i, dim;
	char **list;
	char *name;

	if	(!arg || !(type = GetType(opt)))
		return NULL;

	cpar = memalloc(sizeof(struct CLIST));

	switch (arg[0])
	{
	case '!':	cpar->flag = 0; arg++; break;
	case '^':	cpar->flag = 0; arg++; break;
	case '=':	cpar->flag = 1; arg++; break;
	default:	cpar->flag = 1; break;
	}

	dim = mstrsplit(arg, ",;", &list);
	cpar->list = NULL;
	ptr = &cpar->list;
	name = NULL;
	obj = LvalObj(&Lval_ptr, &Type_str, &name);

	for (i = 0; i < dim; i++)
	{
		*ptr = memalloc(sizeof(struct CLIST));
		name = list[i];
		(*ptr)->code = Obj2uint(EvalObj(RefObj(obj), type));
		(*ptr)->next = NULL;
		ptr = &(*ptr)->next;
	}
	
	memfree(list);
	name = NULL;
	UnrefObj(obj);
	return cpar;
}

TestParDef TestPar_enum = { "[type]=[!^]codelist",
	MakeEnumPar, memfree, NULL
};
