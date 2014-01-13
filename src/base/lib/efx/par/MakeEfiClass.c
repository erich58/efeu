/*
Klassifikationseintrag konstruieren

$Copyright (C) 2007 Erich Frühstück
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

#include <EFEU/EfiClass.h>
#include <EFEU/parsearg.h>
#include <ctype.h>

int MakeEfiClass (EfiType *type, const char *name, const char *def)
{
	AssignArg *x;
	EfiClass *cl;
	int stat;

	if	(def == NULL)
	{
		ListEfiPar(ioerr, type, &EfiPar_class, NULL, 0);
		return 0;
	}

	while (isspace((unsigned char) *def))
		def++;

	if	(*def == 0)
	{
		ListEfiPar(ioerr, type, &EfiPar_class, NULL, 0);
		return 0;
	}
	else if	(*def == '?')
	{
		ListEfiPar(ioerr, type, &EfiPar_class, NULL,
			def[1] == '?' ? 2 : 1);
		return 0;
	}

	x = assignarg(def, NULL, NULL);
	cl = SearchEfiPar(type, &EfiPar_class, x->name);
	stat = 0;

	if	(!cl)
	{
		;
	}
	else if	(x->arg && x->arg[0] == '?')
	{
		PrintEfiPar(ioerr, cl);
	}
	else if	(cl->create)
	{
		EfiClassArg arg;
		cl->create(&arg, type, x->opt, x->arg, cl->par);
		AddEfiClass(type->vtab, name, mstrcpy(def),
			arg.type, arg.update, arg.par);
		stat = 1;
	}

	memfree(x);
	return stat;
}
