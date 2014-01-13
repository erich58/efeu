/*
:*:	dynamik loading of libraries
:de:	Dynamisches Laden von Bibliotheken

$Copyright (C) 2000, 2001 Erich Frühstück
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
#include <EFEU/procenv.h>
#include <EFEU/dl.h>

static EfiVarDef dl_var[] = {
	{ "LDPATH",	&Type_str, &so_path,
		":*:search path for shared object modules\n"
		":de:Suchpfad für shared object modules\n" },
};

static void f_loadlib (EfiFunc *func, void *rval, void **arg)
{
	loadlib(Val_str(arg[0]), Val_str(arg[1]));
}

/*
:*:
The function |$1| expands the esh-interpreter with |dlopen|, 
a function for dynamic loading libraries.
:de:
Die Funktion |$1| erweitert den esh-Interpreter mit einer Funktion
zum dynamischen laden von Programmbibliotheken.
*/

void CmdSetup_dl(void)
{
	AddVarDef(GlobalVar, dl_var, tabsize(dl_var));
	SetFunc(0, &Type_void, "loadlib (str name, str init = NULL)",
		f_loadlib);
}
