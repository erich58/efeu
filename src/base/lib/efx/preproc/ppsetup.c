/*
Eingabe - Preprozessor

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
#include <EFEU/preproc.h>
#include <EFEU/pconfig.h>
#include <EFEU/mactools.h>
#include <EFEU/ioctrl.h>

/*	Variablen
*/

char *IncPath = ".:" String(EFEUROOT) "/lib/esh";

static EfiVarDef pp_var[] = {
	{ "IncPath",	&Type_str, &IncPath,
		":*:search path for header files\n"
		":de:Suchpfad für Headerdateien\n"},
};

#define	RVIO	Val_io(rval)

#define	STR(n)	Val_str(arg[n])
#define	IO(n)	Val_io(arg[n])

static void f_preproc (EfiFunc *func, void *rval, void **arg)
{
	RVIO = io_cmdpreproc(io_refer(IO(0)));
}

static void f_ppopen (EfiFunc *func, void *rval, void **arg)
{
	RVIO = io_cmdpreproc(io_findopen(IncPath, STR(0), STR(1), "r"));
}

static void f_lastcomment (EfiFunc *func, void *rval, void **arg)
{
	io_ctrl(IO(0), IOPP_COMMENT, rval);
}

static void f_clearcomment (EfiFunc *func, void *rval, void **arg)
{
	io_ctrl(IO(0), IOPP_COMMENT, NULL);
}

static EfiFuncDef ppfdef[] = {
	{ 0, &Type_io, "preproc (IO io)", f_preproc },
	{ 0, &Type_io, "ppopen (str name, str type = NULL)",
		f_ppopen },
	{ 0, &Type_str, "lastcomment (IO io = cin)", f_lastcomment },
	{ 0, &Type_str, "clearcomment (IO io = cin)", f_clearcomment },
	{ 0, &Type_void, "target (str)", Func_target },
	{ 0, &Type_void, "depend (str)", Func_depend },
	{ 0, &Type_void, "makedepend (str, IO = iostd)", Func_makedepend },
	{ 0, &Type_list, "dependlist ()", Func_dependlist },
	{ 0, &Type_list, "targetlist ()", Func_targetlist },
};

void SetupPreproc()
{
	AddVarDef(GlobalVar, pp_var, tabsize(pp_var));
	AddFuncDef(ppfdef, tabsize(ppfdef));
}
