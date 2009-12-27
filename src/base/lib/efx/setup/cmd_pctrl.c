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
#include <time.h>


#define	RVSTR	Val_str(rval)
#define	STR(n)	Val_str(arg[n])
#define	INT(n)	Val_int(arg[n])


static void f_exit (Func_t *func, void *rval, void **arg)
{
	exit(INT(0));
}

static void f_system (Func_t *func, void *rval, void **arg) \
{
	Val_int(rval) = callproc(STR(0));
}

static void f_memstat (Func_t *func, void *rval, void **arg)
{
	memstat(STR(0));
}

static void f_allocstat (Func_t *func, void *rval, void **arg)
{
	Obj_stat(STR(0));
}

static void f_memcheck (Func_t *func, void *rval, void **arg)
{
	memcheck();
}

static void f_getenv (Func_t *func, void *rval, void **arg)
{
	char *p = getenv(STR(0));
	RVSTR = mstrcpy(p ? p : STR(1));
}

/*	Zerlegung von Filenamen
*/

static void f_pathname (Func_t *func, void *rval, void **arg)
{
	fname_t *fn = strtofn(STR(0));
	RVSTR = fn ? mstrcpy(fn->path ? fn->path : ".") : NULL;
	memfree(fn);
}

static void f_basename (Func_t *func, void *rval, void **arg)
{
	fname_t *fn = strtofn(STR(0));
	RVSTR = fn ? mstrpaste(".", fn->name, fn->type) : NULL;
	memfree(fn);
}

static void f_filename (Func_t *func, void *rval, void **arg)
{
	fname_t *fn = strtofn(STR(0));
	RVSTR = fn ? mstrcpy(fn->name) : NULL;
	memfree(fn);
}

static void f_filetype (Func_t *func, void *rval, void **arg)
{
	fname_t *fn = strtofn(STR(0));
	RVSTR = fn ? mstrcpy(fn->type) : NULL;
	memfree(fn);
}

/*	Temporäre Filenamen
*/

char *tempnam(const char *path, const char *pfx);

static void f_tmpnam (Func_t *func, void *rval, void **arg)
{
	RVSTR = mstrcpy(tempnam(NULL, STR(0)));
}

static void f_tempnam (Func_t *func, void *rval, void **arg)
{
	RVSTR = mstrcpy(tempnam(STR(0), STR(1)));
}

#if	__linux__
extern int getpid (void);
#endif

static void f_getpid (Func_t *func, void *rval, void **arg)
{
	Val_int(rval) = getpid();
}

static void f_time(Func_t *func, void *rval, void **arg)
{
	Val_int(rval) = time(NULL);
}

static void f_expand(Func_t *func, void *rval, void **arg)
{
	Val_str(rval) = ExpandPath(Val_str(arg[0]));
}


/*	Funktionstabelle
*/

static FuncDef_t fdef_pctrl[] = {
	{ 0, &Type_void, "exit (int rval = 0)", f_exit },
	{ 0, &Type_int, "system (str cmd = NULL)", f_system },
	{ 0, &Type_void, "allocstat (str mark = NULL)", f_allocstat },
	{ 0, &Type_void, "memstat (str mark = NULL)", f_memstat },
	{ 0, &Type_void, "memcheck ()", f_memcheck },
	{ 0, &Type_str, "getenv (str name, str def = NULL)", f_getenv },
	{ 0, &Type_str, "pathname (str name)", f_pathname },
	{ 0, &Type_str, "basename (str name)", f_basename },
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
