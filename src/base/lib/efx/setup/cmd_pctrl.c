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
#include <EFEU/procenv.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>

#define	RVSTR	Val_str(rval)
#define	STR(n)	Val_str(arg[n])
#define	INT(n)	Val_int(arg[n])


static void f_exit (EfiFunc *func, void *rval, void **arg)
{
	int n = INT(0);
	proc_doclean();
	exit(n);
}

static void do_atexit (void *par)
{
	UnrefEval(par);
}

static void f_atexit (EfiFunc *func, void *rval, void **arg)
{
	proc_clean(do_atexit, RefObj(Val_obj(arg[0])));
}

static void f_system (EfiFunc *func, void *rval, void **arg) \
{
	Val_int(rval) = callproc(STR(0));
}

static void f_xsystem (EfiFunc *func, void *rval, void **arg) \
{
	StrBuf *sb;
	EfiObjList *l;
	char *s;

	sb = sb_create(0);

	for (l = Val_list(arg[0]); l != NULL; l = l->next)
	{
		if	(sb->pos)
			sb_putc(' ', sb);

		s = Obj2str(RefObj(l->obj));
		sb_puts(s, sb);
		memfree(s);
	}

	sb_putc(0, sb);
	Val_int(rval) = callproc((char *) sb->data);
	rd_deref(sb);
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

static void f_meminfo (EfiFunc *func, void *rval, void **arg)
{
	meminfo(STR(0));
}

static void f_memchange (EfiFunc *func, void *rval, void **arg)
{
	memchange(STR(0));
}

static void f_allocstat (EfiFunc *func, void *rval, void **arg)
{
	Obj_stat(STR(0));
}

static void f_memcheck (EfiFunc *func, void *rval, void **arg)
{
	memcheck();
}

static void f_lcheckall (EfiFunc *func, void *rval, void **arg)
{
	lcheckall();
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

/*	Temporäre Dateien und Verzeichnisse
*/

static void f_tempstat (EfiFunc *func, void *rval, void **arg)
{
	tempstat();
}

static void f_newtemp (EfiFunc *func, void *rval, void **arg)
{
	RVSTR = mstrcpy(newtemp(STR(0), STR(1)));
}

static void f_newtempdir (EfiFunc *func, void *rval, void **arg)
{
	RVSTR = mstrcpy(newtempdir(STR(0), STR(1)));
}

static void f_deltemp (EfiFunc *func, void *rval, void **arg)
{
	Val_bool(rval) = deltemp(STR(0));
}

static void f_deltempdir (EfiFunc *func, void *rval, void **arg)
{
	Val_bool(rval) = deltempdir(STR(0));
}

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
	{ 0, &Type_void, "atexit (Expr_t expr)", f_atexit },
	{ FUNC_VIRTUAL, &Type_int, "system (str cmd = NULL)", f_system },
	{ FUNC_VIRTUAL, &Type_int, "system (...)", f_xsystem },
	{ 0, &Type_bool, "rename (str oldpath, str newpath)", f_rename },
	{ 0, &Type_bool, "remove (str path)", f_remove },
	{ 0, &Type_void, "perror (str pfx = NULL)", f_perror },
	{ 0, &Type_void, "allocstat (str mark = NULL)", f_allocstat },
	{ 0, &Type_void, "memstat (str mark = NULL)", f_memstat },
	{ 0, &Type_void, "meminfo (str mark = NULL)", f_meminfo },
	{ 0, &Type_void, "memchange (str mark = NULL)", f_memchange },
	{ 0, &Type_void, "memcheck ()", f_memcheck },
	{ 0, &Type_void, "lcheckall ()", f_lcheckall },
	{ 0, &Type_str, "getenv (str name, str def = NULL)", f_getenv },
	{ 0, &Type_str, "dirname (str name, bool flag = false)", f_dirname },
	{ FUNC_VIRTUAL, &Type_str, "basename (str path)", f_basename },
	{ FUNC_VIRTUAL, &Type_str, "basename (str path, str & suffix)",
		f_basename2 },
	{ 0, &Type_str, "filename (str name)", f_filename },
	{ 0, &Type_str, "filetype (str name)", f_filetype },
	{ 0, &Type_str, "newtemp (str path = NULL, str pfx = NULL)",
		f_newtemp },
	{ 0, &Type_str, "newtempdir (str path = NULL, str pfx = NULL)",
		f_newtempdir },
	{ 0, &Type_bool, "deltemp (str path)", f_deltemp },
	{ 0, &Type_bool, "deltempdir (str path)", f_deltempdir },
	{ 0, &Type_void, "tempstat (void)", f_tempstat },
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
