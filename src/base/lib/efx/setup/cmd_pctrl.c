/*
Programmsteuerung

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

#include <EFEU/ftools.h>
#include <EFEU/object.h>
#include <EFEU/pconfig.h>
#include <EFEU/cmdsetup.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>

#define	RVSTR	Val_str(rval)
#define	STR(n)	Val_str(arg[n])
#define	INT(n)	Val_int(arg[n])


static void f_exit (EfiFunc *func, void *rval, void **arg)
{
	exit(INT(0));
}

static void f_system (EfiFunc *func, void *rval, void **arg) \
{
	Val_int(rval) = callproc(STR(0));
}

static void f_rename (EfiFunc *func, void *rval, void **arg) \
{
	Val_bool(rval) = rename(STR(0), STR(1)) == 0;
}

static void f_remove (EfiFunc *func, void *rval, void **arg) \
{
	Val_bool(rval) = remove(STR(0)) == 0;
}

static void f_perror (EfiFunc *func, void *rval, void **arg) \
{
	char *pfx = STR(0);
	perror(pfx ? pfx : ProgName);
}

static void f_memstat (EfiFunc *func, void *rval, void **arg)
{
	memstat(STR(0));
}

static void f_allocstat (EfiFunc *func, void *rval, void **arg)
{
	Obj_stat(STR(0));
}

static void f_memcheck (EfiFunc *func, void *rval, void **arg)
{
	memcheck();
}

static void f_getenv (EfiFunc *func, void *rval, void **arg)
{
	char *p = getenv(STR(0));
	RVSTR = mstrcpy(p ? p : STR(1));
}

/*	Zerlegung von Filenamen
*/

static void f_dirname (EfiFunc *func, void *rval, void **arg)
{
	RVSTR = mdirname(STR(0), Val_bool(arg[1]));
}

static void f_basename (EfiFunc *func, void *rval, void **arg)
{
	RVSTR = mbasename(STR(0), NULL);
}

static void f_basename2 (EfiFunc *func, void *rval, void **arg)
{
	char *p;
	RVSTR = mbasename(STR(0), &p);
	memfree(STR(1));
	STR(1) = mstrcpy(p);
}

static void f_filename (EfiFunc *func, void *rval, void **arg)
{
	char *suffix;
	RVSTR = mbasename(STR(0), &suffix);
}

static void f_filetype (EfiFunc *func, void *rval, void **arg)
{
	char *name, *suffix;
	name = mbasename(STR(0), &suffix);
	RVSTR = mstrcpy(suffix);
	memfree(name);
}

/*	Temporäre Filenamen
*/

char *tempnam(const char *path, const char *pfx);

static void f_tmpnam (EfiFunc *func, void *rval, void **arg)
{
	RVSTR = mstrcpy(tempnam(NULL, STR(0)));
}

static void f_tempnam (EfiFunc *func, void *rval, void **arg)
{
	RVSTR = mstrcpy(tempnam(STR(0), STR(1)));
}

#if	__linux__
extern int getpid (void);
#endif

static void f_getpid (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = getpid();
}

static void f_time(EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = time(NULL);
}

static void f_expand(EfiFunc *func, void *rval, void **arg)
{
	Val_str(rval) = ExpandPath(Val_str(arg[0]));
}


/*	Funktionstabelle
*/

static EfiFuncDef fdef_pctrl[] = {
	{ 0, &Type_void, "exit (int rval = 0)", f_exit },
	{ 0, &Type_int, "system (str cmd = NULL)", f_system },
	{ 0, &Type_bool, "rename (str oldpath, str newpath)", f_rename },
	{ 0, &Type_bool, "remove (str path)", f_remove },
	{ 0, &Type_void, "perror (str pfx = NULL)", f_perror },
	{ 0, &Type_void, "allocstat (str mark = NULL)", f_allocstat },
	{ 0, &Type_void, "memstat (str mark = NULL)", f_memstat },
	{ 0, &Type_void, "memcheck ()", f_memcheck },
	{ 0, &Type_str, "getenv (str name, str def = NULL)", f_getenv },
	{ 0, &Type_str, "dirname (str name, bool flag = false)", f_dirname },
	{ FUNC_VIRTUAL, &Type_str, "basename (str path)", f_basename },
	{ FUNC_VIRTUAL, &Type_str, "basename (str path, str & suffix)",
		f_basename2 },
	{ 0, &Type_str, "filename (str name)", f_filename },
	{ 0, &Type_str, "filetype (str name)", f_filetype },
	{ 0, &Type_str, "tmpnam (str pfx = NULL)", f_tmpnam },
	{ 0, &Type_str, "tempnam (str path = NULL, str pfx = NULL)",
		f_tempnam },
	{ 0, &Type_int, "getpid ()", f_getpid },
	{ 0, &Type_int, "time ()", f_time },
	{ 0, &Type_str,  "ExpandPath (str = NULL)", f_expand },
};


/*	Initialisierung
*/

void CmdSetup_pctrl(void)
{
	AddFuncDef(fdef_pctrl, tabsize(fdef_pctrl));
}
