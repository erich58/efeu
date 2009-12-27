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
#include <EFEU/mactools.h>
#include <EFEU/cmdsetup.h>

static char *LDPATH = ".:" String(_EFEU_TOP) "/lib";

static VarDef_t dl_var[] = {
	{ "LDPATH",	&Type_str, &LDPATH,
		":*:search path for shared object modules\n"
		":de:Suchpfad für shared object modules\n"},
};

#if	__linux__

#include <dlfcn.h>

static int libname (const char *name)
{
	if	(strchr(name, '/') || patcmp("lib*.so*", name, NULL))
		return 0;

	return 1;
}

static void f_loadlib (Func_t *func, void *rval, void **arg)
{
	char *lname = Val_str(arg[0]);
	char *fname = Val_str(arg[1]);
	char *path;
	void *handle;
	void (*init)(void);

	if	(lname == NULL)	return;

	path = fsearch(LDPATH, NULL, lname, "so");

	if	(!path && libname(lname))
	{
		char *p = msprintf("lib%s", lname);
		path = fsearch(LDPATH, NULL, p, "so");
		memfree(p);
	}

	if	(path)
	{
		handle = dlopen(path, RTLD_GLOBAL|RTLD_NOW);
		memfree(path);
	}
	else if	(libname(lname))
	{
		char *p = msprintf("lib%s.so", lname);
		handle = dlopen(p, RTLD_GLOBAL|RTLD_NOW);
		memfree(p);
	}
	else	handle = dlopen(lname, RTLD_GLOBAL|RTLD_NOW);

	if	(handle == NULL)
	{
		io_printf(ioerr, "dlopen: %s\n", dlerror());
	}
	else if	(fname == NULL)
	{
		;
	}
	else if	((init = dlsym(handle, fname)) == NULL)
	{
		io_printf(ioerr, "dlsym: %s\n", dlerror());
		dlclose(handle);
	}
	else	init();
}

#else

static void f_loadlib(Func_t *func, void *rval, void **arg)
{
	io_printf(ioerr, "loadlib: not implementet\n");
}

#endif


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
