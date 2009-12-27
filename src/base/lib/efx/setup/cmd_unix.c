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

#if	_AIX
#define	_ALL_SOURCE
#endif

#include <EFEU/object.h>
#include <EFEU/cmdsetup.h>
#include <EFEU/refdata.h>
#include <EFEU/calendar.h>
#include <EFEU/cmdeval.h>
#include <EFEU/vecbuf.h>
#include <time.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>

static void mode_putc (IO *out, int mode, const char *xkey)
{
	io_putc((mode & 04) ? 'r' : '-', out);
	io_putc((mode & 02) ? 'w' : '-', out);
	io_putc(xkey[(mode & 01) ? 1 : 0], out);
}

static int io_mode (IO *out, int mode)
{
	switch (mode & S_IFMT)
	{
	case S_IFREG:	io_putc('-', out); break;
	case S_IFDIR:	io_putc('d', out); break;
	case S_IFCHR:	io_putc('c', out); break;
	case S_IFBLK:	io_putc('b', out); break;
	case S_IFIFO:	io_putc('p', out); break;
	case S_IFLNK:	io_putc('l', out); break;
/*
	case S_IFSOCK:	io_putc('s', out); break;
*/
	default:	io_putc('?', out); break;
	}

	mode_putc(out, (mode >> 6) & 07, (mode & S_ISUID) ? "Ss" : "-x");
	mode_putc(out, (mode >> 3) & 07, (mode & S_ISGID) ? "Ss" : "-x");
	mode_putc(out, mode & 07, (mode & S_ISVTX) ? "Tt" : "-x");
	return 9;
}

static int print_mode (const EfiType *type, const void *data, IO *io)
{
	return io_mode(io, Val_uint(data));
}

EfiType Type_mode = SIMPLE_TYPE("mode_t", unsigned, &Type_uint, print_mode);

/*	Filestatus
*/

typedef struct {
	char *path;
	unsigned uid;
	unsigned gid;
	unsigned mode;
	int64_t size;
	CalTimeIndex mtime;
} FileStat;

#define	Val_fstat(x)	((FileStat *) (x))[0]

static void fstat_clean (const EfiType *st, void *data, int mode)
{
	memfree(Val_fstat(data).path);
	memset(data, 0, st->size);
}

static void fstat_copy (const EfiType *st, void *tg, const void *src)
{
	memcpy(tg, src, st->size);
	Val_fstat(tg).path = mstrcpy(Val_fstat(src).path);
}

EfiType Type_fstat = STD_TYPE("FileStat", FileStat, NULL,
	fstat_clean, fstat_copy);

static EfiObj *fstat_path (const EfiObj *base, void *data)
{
	return str2Obj(base ? mstrcpy(((FileStat *) base->data)->path) : NULL);
}

static EfiObj *fstat_uid (const EfiObj *base, void *data)
{
	return int2Obj(base ? ((FileStat *) base->data)->uid : 0);
}

static EfiObj *fstat_gid (const EfiObj *base, void *data)
{
	return int2Obj(base ? ((FileStat *) base->data)->gid : 0);
}

static EfiObj *fstat_mode (const EfiObj *base, void *data)
{
	unsigned mode = base ? ((FileStat *) base->data)->mode : 0;
	return NewObj(&Type_mode, &mode);
}

static EfiObj *fstat_type (const EfiObj *base, void *data)
{
	unsigned mode;
	unsigned char c;
	
	mode = base ? ((FileStat *) base->data)->mode : 0;

	if	(S_ISREG(mode))		c = 'f';
	else if	(S_ISDIR(mode))		c = 'd';
	else if	(S_ISCHR(mode))		c = 'c';
	else if	(S_ISBLK(mode))		c = 'b';
	else if	(S_ISFIFO(mode))	c = 'p';
#ifdef	S_ISLNK
	else if	(S_ISLNK(mode))		c = 'l';
#endif
#ifdef	S_ISSOCK
	else if	(S_ISSOCK(mode))	c = 's';
#endif
	else				c = '?';

	return NewObj(&Type_char, &c);
}

static EfiObj *fstat_size (const EfiObj *base, void *data)
{
	int64_t val = base ? ((FileStat *) base->data)->size : 0;
	return NewObj(&Type_int64, &val);
}

static EfiObj *fstat_mtime (const EfiObj *base, void *data)
{
	if	(base)
	{
		FileStat *fstat = base->data;
		return NewObj(&Type_Time, &fstat->mtime);
	}
	else	return NewObj(&Type_Time, NULL);
}

static EfiMember fstat_var[] = {
	{ "path", &Type_str, fstat_path, NULL },
	{ "uid", &Type_int, fstat_uid, NULL },
	{ "gid", &Type_int, fstat_gid, NULL },
	{ "mode", &Type_int, fstat_mode, NULL },
	{ "type", &Type_char, fstat_type, NULL },
	{ "size", &Type_int64, fstat_size, NULL },
	{ "mtime", &Type_Time, fstat_mtime, NULL },
};

static void str2fstat (EfiFunc *func, void *rval, void **arg)
{
	struct stat buf;
	struct tm *x;

	if	(stat(Val_str(arg[0]), &buf) != EOF)
	{
		FileStat *fstat = rval;
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
	else	memset(rval, 0, sizeof(FileStat));
}

static void f_mode2str (EfiFunc *func, void *rval, void **arg)
{
	StrBuf *buf = sb_acquire();
	IO *out = io_strbuf(buf);
	io_mode(out, Val_uint(arg[0]));
	io_close(out);
	Val_str(rval) = sb_cpyrelease(buf);
}

static void f_mode2int (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = Val_uint(arg[0]);
}

static void fprint_mode (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = io_mode(Val_io(arg[0]), Val_uint(arg[1]));
}

static void fprint_fstat (EfiFunc *func, void *rval, void **arg)
{
	IO *io = Val_io(arg[0]);
	FileStat *fstat = arg[1];

	if	(fstat->path)
	{
		int n = 0;
		n = io_xprintf(io, "(%s: ", fstat->path);
		n += PrintTime(io, "%_d. %b %Y %H:%M ", fstat->mtime);
		n += io_xprintf(io, "%#o ", fstat->mode);
		n += io_xprintf(io, "%d-", fstat->uid);
		n += io_xprintf(io, "%d ", fstat->gid);
		n += io_xprintf(io, "%lld)", fstat->size);
		Val_int(rval) = n;
	}
	else	Val_int(rval) = io_puts("(unknown)", io);
}

static void fstat2bool (EfiFunc *func, void *rval, void **arg)
{
	Val_bool(rval) = ((FileStat *) arg[0])->path != NULL;
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
} PWD;

static void pwd_clean (const EfiType *type, void *data, int mode)
{
	register PWD *pwd = data;
	memfree(pwd->name);
	memfree(pwd->gecos);
	memfree(pwd->home);
	memfree(pwd->shell);
	memset(data, 0, type->size);
}

static void pwd_copy (const EfiType *st, void *tp, const void *sp)
{
	register PWD *tg = tp;
	register const PWD *src = sp;
	tg->name = mstrcpy(src->name);
	tg->gecos = mstrcpy(src->gecos);
	tg->home = mstrcpy(src->home);
	tg->shell = mstrcpy(src->shell);
	tg->uid = src->uid;
	tg->gid = src->gid;
}

EfiType Type_pwd = STD_TYPE("passwd", PWD, NULL, pwd_clean, pwd_copy);

static EfiObj *pwd_name (const EfiObj *obj, void *data)
{
	PWD *pwd = obj->data;
	return ConstObj(&Type_str, &pwd->name);
}

static EfiObj *pwd_gecos (const EfiObj *obj, void *data)
{
	PWD *pwd = obj->data;
	return ConstObj(&Type_str, &pwd->gecos);
}

static EfiObj *pwd_home (const EfiObj *obj, void *data)
{
	PWD *pwd = obj->data;
	return ConstObj(&Type_str, &pwd->home);
}

static EfiObj *pwd_shell (const EfiObj *obj, void *data)
{
	PWD *pwd = obj->data;
	return ConstObj(&Type_str, &pwd->shell);
}

static EfiObj *pwd_uid (const EfiObj *obj, void *data)
{
	PWD *pwd = obj->data;
	return int2Obj(pwd->uid);
}

static EfiObj *pwd_gid (const EfiObj *obj, void *data)
{
	PWD *pwd = obj->data;
	return int2Obj(pwd->gid);
}

static EfiMember pwd_var[] = {
	{ "name", &Type_str, pwd_name, NULL },
	{ "gecos", &Type_str, pwd_gecos, NULL },
	{ "home", &Type_str, pwd_home, NULL },
	{ "shell", &Type_str, pwd_shell, NULL },
	{ "uid", &Type_int, pwd_uid, NULL },
	{ "gid", &Type_int, pwd_gid, NULL },
};


static void set_pwd (PWD *pwd, struct passwd *base)
{
	if	(!base)	return;

	pwd->name = mstrcpy(base->pw_name);
	pwd->gecos = mstrcpy(base->pw_gecos);
	pwd->home = mstrcpy(base->pw_dir);
	pwd->shell = mstrcpy(base->pw_shell);
	pwd->uid = base->pw_uid;
	pwd->gid = base->pw_gid;
}

static void f_pwd_str (EfiFunc *func, void *rval, void **arg)
{
	set_pwd(rval, getpwnam(Val_str(arg[0])));
}

static void f_pwd_int (EfiFunc *func, void *rval, void **arg)
{
	set_pwd(rval, getpwuid(Val_int(arg[0])));
}

static void f_pwd_void (EfiFunc *func, void *rval, void **arg)
{
	set_pwd(rval, getpwuid(getuid()));
}

static void fprint_pwd (EfiFunc *func, void *rval, void **arg)
{
	IO *io = Val_io(arg[0]);
	PWD *pwd = arg[1];

	Val_int(rval) = pwd->name ?
		io_xprintf(io, "%s (%s)", pwd->name, pwd->gecos) :
		io_puts("(unknown)", io);
}

static void pwd2bool (EfiFunc *func, void *rval, void **arg)
{
	Val_bool(rval) = ((PWD *) arg[0])->name != NULL;
}

static void f_getuid (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = getuid();
}

static void f_umask (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = umask(Val_int(arg[0]));
}

static int skip_name (const char *name)
{
	if	(name[0] != '.')	return 0;
	if	(name[1] == 0)		return 1;
	if	(name[1] != '.')	return 0;
	if	(name[2] == 0)		return 1;

	return 0;
}

#define	FTYPE_F	0x1
#define	FTYPE_D	0x2

static EfiObjList **do_flist (EfiObjList **ptr, size_t offset,
	const char *path, const char *pat, int type)
{
	DIR *dir;
	struct dirent *entry;
	struct stat statbuf;
	VecBuf *dir_buf;
	char *p, **dp;
	size_t n;
	
	if	((dir = opendir(path ? path : ".")) == NULL)
		return ptr;

	dir_buf = vb_create(1024, sizeof(char *));

	while ((entry = readdir(dir)) != NULL)
	{
		if	(skip_name(entry->d_name))	continue;

		p = mstrpaste("/", path, entry->d_name);

		if	(stat(p, &statbuf) != 0)
		{
			memfree(p);
			continue;
		}

		if	(S_ISDIR(statbuf.st_mode))
		{
			dp = vb_next(dir_buf);

			if	(type & FTYPE_D)
			{
				*dp = mstrcpy(p);
			}
			else
			{
				*dp = p;
				continue;
			}
		}
		else if	(!(type & FTYPE_F))
		{
			memfree(p);
			continue;
		}
		/*
		if (!S_ISREG(statbuf.st_mode))
		{
			memfree(p);
		}
		*/

		if	(!pat || patcmp(pat, entry->d_name, NULL))
		{
			if	(offset)
			{
				*ptr = NewObjList(str2Obj(mstrcpy(p + offset)));
				memfree(p);
			}
			else	*ptr = NewObjList(str2Obj(p));

			ptr = &(*ptr)->next;
		}
		else	memfree(p);
	}

	closedir(dir);

	for (dp = dir_buf->data, n = dir_buf->used; n--; dp++)
	{
		ptr = do_flist(ptr, offset, *dp, pat, type);
		memfree(*dp);
	}

	vb_destroy(dir_buf);
	return ptr;
}

static void f_flist (EfiFunc *func, void *rval, void **arg)
{
	EfiObjList **ptr = (EfiObjList **) rval;
	char *path = Val_str(arg[0]);
	char *p = Val_str(arg[3]);
	int type;

	if	(!p)	p = "f";

	for (type = 0; *p; p++)
	{
		switch (*p)
		{
		case 'f':	type |= FTYPE_F; break;
		case 'd':	type |= FTYPE_D; break;
		}
	}

	if	(!path)	path = ".";

	*ptr = NULL;

	if	(Val_bool(arg[2]))
	{
		do_flist(ptr, strlen(path) + 1, path, Val_str(arg[1]), type);
	}
	else	do_flist(ptr, 0, path, Val_str(arg[1]), type);
}

static void f_mkdir (EfiFunc *func, void *rval, void **arg)
{
	Val_bool(rval) = mkdir(Val_str(arg[0]), (mode_t) 0777) == 0;
}

static void f_getcwd (EfiFunc *func, void *rval, void **arg)
{
	char buf[PATH_MAX];
	char *p = getcwd(buf, sizeof(buf));
	Val_str(rval) = mstrcpy(p);
}

/*	Funktionstabelle
*/

static EfiFuncDef func_unix[] = {
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, mode_t)", fprint_mode },
	{ FUNC_RESTRICTED, &Type_str, "mode_t ()", f_mode2str },
	{ FUNC_RESTRICTED, &Type_int, "mode_t ()", f_mode2int },
	{ FUNC_PROMOTION, &Type_uint, "mode_t ()", CopyDataFunc },
	{ 0, &Type_fstat, "FileStat (str path)", str2fstat },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, FileStat)", fprint_fstat },
	{ FUNC_RESTRICTED, &Type_bool, "FileStat ()", fstat2bool },
	{ FUNC_VIRTUAL, &Type_pwd, "passwd (int uid)", f_pwd_int },
	{ FUNC_VIRTUAL, &Type_pwd, "passwd (str name)", f_pwd_str },
	{ FUNC_VIRTUAL, &Type_pwd, "passwd (void)", f_pwd_void },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, passwd)", fprint_pwd },
	{ FUNC_RESTRICTED, &Type_bool, "passwd ()", pwd2bool },
	{ 0, &Type_int, "getuid ()", f_getuid },
	{ 0, &Type_int, "umask (int mask)", f_umask },
	{ 0, &Type_list, "flist (str path, str pat = NULL,"
		" bool rel = false, str type = \"f\")", f_flist },
	{ 0, &Type_bool, "mkdir (str path)", f_mkdir },
	{ 0, &Type_str, "getcwd ()", f_getcwd },
};


void CmdSetup_unix(void)
{
	AddType(&Type_mode);
	AddType(&Type_fstat);
	AddEfiMember(Type_fstat.vtab, fstat_var, tabsize(fstat_var));
	AddType(&Type_pwd);
	AddEfiMember(Type_pwd.vtab, pwd_var, tabsize(pwd_var));
	AddFuncDef(func_unix, tabsize(func_unix));
}
