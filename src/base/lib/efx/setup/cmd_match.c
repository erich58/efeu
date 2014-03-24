/*
:*:     pattern matching
:de:    Mustervergleich

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

#include <EFEU/object.h>
#include <EFEU/cmdsetup.h>
#include <EFEU/MatchPar.h>
#include <EFEU/Op.h>

static EfiType Type_match = REF_TYPE("MatchPar", MatchPar *);

static void str2match (EfiFunc *func, void *rval, void **arg)
{
	Val_ptr(rval) = MatchPar_list(Val_str(arg[0]), Val_int(arg[1]));
}

static void f_exec (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = MatchPar_exec(Val_ptr(arg[0]),
		Val_str(arg[1]), Val_int(arg[2]));
}

/*	Funktionstabelle
*/

static EfiFuncDef func_match[] = {
	{ 0, &Type_match, "MatchPar (str def, int dim = 0)", str2match },
	{ 0, &Type_bool, "MatchPar::exec (str s, int idx = 0)", f_exec },
};

void CmdSetup_match (void)
{
	AddType(&Type_match);
	AddFuncDef(func_match, tabsize(func_match));
}
