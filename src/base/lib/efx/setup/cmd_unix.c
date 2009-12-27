/*
UNIX-Spezifische Funktionen und Strukturen

$Copyright (C) 1999 Erich Frühstück
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
#include <EFEU/refdata.h>
#include <EFEU/calendar.h>
#include <EFEU/cmdeval.h>
#include <time.h>
#include <pwd.h>
#include <sys/stat.h>
#include <ftw.h>

/*	Filestatus
*/

typedef struct {
	char *path;
	unsigned uid;
	unsigned gid;
	unsigned mode;
	ulong_t size;
	Time_t mtime;
} fstat_t;

#define	Val_fstat(x)	((fstat_t *) (x))[0]

static void fstat_clean (const Type_t *st, void *data)
{
	memfree(Val_fstat(data).path);
	memset(data, 0, st->size);
}

static void fstat_copy (const Type_t *st, void *tg, const void *src)
{
	memcpy(tg, src, st->size);
	Val_fstat(tg).path = mstrcpy(Val_fstat(src).path);
}

Type_t Type_fstat = STD_TYPE("FileStat", fstat_t, NULL,
	fstat_clean, fstat_copy);

static char **fstat_path (fstat_t *fstat) { return &fstat->path; }
static int *fstat_uid (fstat_t *fstat) { return &fstat->uid; }
static int *fstat_gid (fstat_t *fstat) { return &fstat->gid; }
static int *fstat_mode (fstat_t *fstat) { return &fstat->mode; }
static long *fstat_size (fstat_t *fstat) { return &fstat->size; }
static Time_t *fstat_mtime (fstat_t *fstat) { return &fstat->mtime; }

static MemberDef_t fstat_var[] = {
	{ "path", &Type_str, ConstMember, fstat_path },
	{ "uid", &Type_int, ConstMember, fstat_uid },
	{ "gid", &Type_int, ConstMember, fstat_gid },
	{ "mode", &Type_int, ConstMember, fstat_mode },
	{ "size", &Type_long, ConstMember, fstat_size },
	{ "mtime", &Type_Time, ConstMember, fstat_mtime },
};

static void str2fstat (Func_t *func, void *rval, void **arg)
{
	struct stat buf;
	struct tm *x;

	if	(stat(Val_str(arg[0]), &buf) != EOF)
	{
		fstat_t *fstat = rval;
		fstat->path = mstrcpy(Val_str(arg[0]));
		fstat->uid = buf.st_uid;
		fstat->gid = buf.st_gid;
		fstat->mode = buf.st_mode;
		fstat->size = buf.st_size;

		if	((x = localtime(&buf.st_mtime)) != NULL)
		{
			fstat->mtime.date = CalendarIndex(x->tm_mday,
				1 + x->tm_mon, 1900 + x->tm_year);
			fstat->mtime.time = (x->tm_hour * 60 + x->tm_min) * 60
				+ x->tm_sec;
		}
		else	fstat->mtime = CurrentTime();
	}
	else	memset(rval, 0, sizeof(fstat_t));
}

static void fprint_fstat (Func_t *func, void *rval, void **arg)
{
	io_t *io = Val_io(arg[0]);
	fstat_t *fstat = arg[1];

	if	(fstat->path)
	{
		int n = 0;
		n = io_printf(io, "(%s: ", fstat->path);
		n += PrintTime(io, "%_d. %b %Y %H:%M ", fstat->mtime);
		n += io_printf(io, "%#o ", fstat->mode);
		n += io_printf(io, "%d-", fstat->uid);
		n += io_printf(io, "%d ", fstat->gid);
		n += io_printf(io, "%ld)", fstat->size);
		Val_int(rval) = n;
	}
	else	Val_int(rval) = io_puts("(unknown)", io);
}

static void fstat2bool (Func_t *func, void *rval, void **arg)
{
	Val_bool(rval) = ((fstat_t *) arg[0])->path != NULL;
}

/*	Paßworteintrag
*/

typedef struct {
	char *name;
	char *gecos;
	char *home;
	char *shell;
	unsigned uid;
	unsigned gid;
} pwd_t;

static void pwd_clean (const Type_t *type, void *data)
{
	register pwd_t *pwd = data;
	memfree(pwd->name);
	memfree(pwd->gecos);
	memfree(pwd->home);
	memfree(pwd->shell);
	memset(data, 0, type->size);
}

static void pwd_copy (const Type_t *st, void *tp, const void *sp)
{
	register pwd_t *tg = tp;
	register const pwd_t *src = sp;
	tg->name = mstrcpy(src->name);
	tg->gecos = mstrcpy(src->gecos);
	tg->home = mstrcpy(src->home);
	tg->shell = mstrcpy(src->shell);
	tg->uid = src->uid;
	tg->gid = src->gid;
}

Type_t Type_pwd = STD_TYPE("passwd", pwd_t, NULL, pwd_clean, pwd_copy);

static char **pwd_name (pwd_t *pwd) { return &pwd->name; }
static char **pwd_gecos (pwd_t *pwd) { return &pwd->gecos; }
static char **pwd_home (pwd_t *pwd) { return &pwd->home; }
static char **pwd_shell (pwd_t *pwd) { return &pwd->shell; }
static int *pwd_uid (pwd_t *pwd) { return &pwd->uid; }
static int *pwd_gid (pwd_t *pwd) { return &pwd->gid; }

static MemberDef_t pwd_var[] = {
	{ "name", &Type_str, ConstMember, pwd_name },
	{ "gecos", &Type_str, ConstMember, pwd_gecos },
	{ "home", &Type_str, ConstMember, pwd_home },
	{ "shell", &Type_str, ConstMember, pwd_shell },
	{ "uid", &Type_int, ConstMember, pwd_uid },
	{ "gid", &Type_int, ConstMember, pwd_gid },
};


static void set_pwd (pwd_t *pwd, struct passwd *base)
{
	if	(!base)	return;

	pwd->name = mstrcpy(base->pw_name);
	pwd->gecos = mstrcpy(base->pw_gecos);
	pwd->home = mstrcpy(base->pw_dir);
	pwd->shell = mstrcpy(base->pw_shell);
	pwd->uid = base->pw_uid;
	pwd->gid = base->pw_gid;
}

static void f_pwd_str (Func_t *func, void *rval, void **arg)
{
	set_pwd(rval, getpwnam(Val_str(arg[0])));
}

static void f_pwd_int (Func_t *func, void *rval, void **arg)
{
	set_pwd(rval, getpwuid(Val_int(arg[0])));
}

static void f_pwd_void (Func_t *func, void *rval, void **arg)
{
	set_pwd(rval, getpwuid(getuid()));
}

static void fprint_pwd (Func_t *func, void *rval, void **arg)
{
	io_t *io = Val_io(arg[0]);
	pwd_t *pwd = arg[1];

	Val_int(rval) = pwd->name ?
		io_printf(io, "%s (%s)", pwd->name, pwd->gecos) :
		io_puts("(unknown)", io);
}

static void pwd2bool (Func_t *func, void *rval, void **arg)
{
	Val_bool(rval) = ((pwd_t *) arg[0])->name != NULL;
}

static void f_getuid (Func_t *func, void *rval, void **arg)
{
	Val_int(rval) = getuid();
}

/*	Bibliothekbaum durchwandern
*/

static Obj_t *ftw_expr = NULL;

static int ftw_eval (const char *name, const struct stat *sb, int flag)
{
	int rval;

	if	(flag == FTW_F)
	{
		Obj_t *save = LocalObj;
		LocalObj = str2Obj(mstrcpy(name));

		UnrefObj(CmdEval_retval);
		CmdEval_retval = NULL;
		UnrefEval(RefObj(ftw_expr));
		rval = Obj2int(CmdEval_retval);
		CmdEval_retval = NULL;
		CmdEval_stat = 0;
		LocalObj = save;
	}
	else	rval = 0;

	return rval;
}

static void f_ftw (Func_t *func, void *rval, void **arg)
{
	Obj_t *save = ftw_expr;
	ftw_expr = Val_obj(arg[1]);
	Val_int(rval) = ftw(Val_str(arg[0]), ftw_eval, Val_int(arg[2]));
	ftw_expr = save;
}

static char *flist_pat = NULL;
static ObjList_t **flist_ptr = NULL;

static int flist_test (const char *name)
{
	char *p;

	if	(!flist_pat)	return 1;
	if	(!name)		return 0;

	p = strrchr(name, '/');
	return patcmp(flist_pat, p ? p + 1 : name, NULL);
}

static int flist_eval (const char *name, const struct stat *sb, int flag)
{
	if	(flag == FTW_F)
	{
		if	(flist_test(name))
		{
			*flist_ptr = NewObjList(str2Obj(mstrcpy(name)));
			flist_ptr = &(*flist_ptr)->next;
		}
	}

	return 0;
}

static void f_flist (Func_t *func, void *rval, void **arg)
{
	flist_ptr = (ObjList_t **) rval;
	*flist_ptr = NULL;
	flist_pat = Val_str(arg[1]);
	ftw(Val_str(arg[0]), flist_eval, Val_int(arg[2]));
}

/*	Funktionstabelle
*/

static FuncDef_t func_unix[] = {
	{ 0, &Type_fstat, "FileStat (str path)", str2fstat },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, FileStat)", fprint_fstat },
	{ FUNC_RESTRICTED, &Type_bool, "FileStat ()", fstat2bool },
	{ FUNC_VIRTUAL, &Type_pwd, "passwd (int uid)", f_pwd_int },
	{ FUNC_VIRTUAL, &Type_pwd, "passwd (str name)", f_pwd_str },
	{ FUNC_VIRTUAL, &Type_pwd, "passwd (void)", f_pwd_void },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, passwd)", fprint_pwd },
	{ FUNC_RESTRICTED, &Type_bool, "passwd ()", pwd2bool },
	{ 0, &Type_int, "getuid ()", f_getuid },
	{ 0, &Type_int, "ftw (str path, Expr_t expr = NULL, int depth = 1)", f_ftw },
	{ 0, &Type_list, "flist (str path, str pat = NULL, int depth = 1)", f_flist },
};


void CmdSetup_unix(void)
{
	AddType(&Type_fstat);
	AddMember(Type_fstat.vtab, fstat_var, tabsize(fstat_var));
	AddType(&Type_pwd);
	AddMember(Type_pwd.vtab, pwd_var, tabsize(pwd_var));
	AddFuncDef(func_unix, tabsize(func_unix));
}
