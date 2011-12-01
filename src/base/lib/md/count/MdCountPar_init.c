/*
:*:count parameters
:de:Zählparameter initialisieren

$Copyright (C) 2007 Erich Frühstück
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

#include <EFEU/mdcount.h>
#include <EFEU/parsearg.h>
#include <EFEU/EfiCount.h>

#define	ERR_METHOD	"$!: $1: Unbekannte Auswertungsdefinition \"$2\".\n"

static void list (IO *io, EfiType *type, const char *pfx)
{
	EfiStruct *st;

	io_xprintf(io, "type=%s\n", type->name);
	ListEfiPar(io, type, &EfiPar_EfiCount, pfx, 1);

	for (st = type->list; st; st = st->next)
	{
		char *p = mstrpaste(".", pfx, st->name);
		list(io, st->type, p);
		memfree(p);
	}
}

static void list_member (IO *io, EfiType *type, const char *pfx)
{
	EfiStruct *st;

	for (st = type->list; st; st = st->next)
	{
		char *p = mstrpaste(".", pfx, st->name);
		io_xprintf(ioerr, "%s: %s\n", p, st->desc);
		list_member(io, st->type, p);
		memfree(p);
	}
}


static EfiObj *cnt_obj (EfiObj *base, const char *name)
{
	/*
	char *p;
	*/

	if	(*name == 0 || *name == '?')
	{
		list_member(ioerr, base->type, NULL);
		exit(EXIT_SUCCESS);
	}

	base = RefObj(base);

#if	0
	p = strchr(name, '.');

	if	(p)
	{

	}

	while (*name)
	{
		char *p;

		if	((p = strchr(name, '.'))
		*p = 0;

		name = p + 1;
		}
		else	name = NULL;
	}
#endif

	return base;
}

void MdCountPar_init (MdCountPar *par, EfiObj *obj, const char *method)
{
	AssignArg *xarg;
	EfiCount *cdef;
	EfiType *type;
	EfiObj *cobj;
	char *p;

	type = obj->type;
	xarg = assignarg(method, NULL, NULL);
	par->obj = RefObj(obj);

	if	(!xarg || !xarg->name[0] || xarg->name[0] == '?')
	{
		list(iostd, type, NULL);
		exit(EXIT_SUCCESS);
	}

	cobj = RefObj(obj);
	method = xarg->name;

	while ((p = strchr(method, '.')))
	{
		*p = 0;
		cobj = cnt_obj(cobj, method);
		method = p + 1;
	}

	fprintf(stderr, "method=%s\n", method);
	type = cobj->type;
	cdef = SearchEfiPar(type, &EfiPar_EfiCount, method);

	if	(!cdef)
		log_error(NULL, ERR_METHOD, "ms", Type2str(type), method);

/*	Klassen und Zähltypen initialisieren
*/
	if	(mstrcmp(xarg->arg, "?") == 0)
	{
		PrintEfiPar(iostd, cdef);
		exit(EXIT_SUCCESS);
	}
	
	if	(cdef->create)
		cdef->create(par, cdef, par->obj, xarg->opt, xarg->arg);
}
