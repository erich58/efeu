/*	Programmsteuerung
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
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
	libexit(INT(0));
}

static void f_system (Func_t *func, void *rval, void **arg) \
{
	callproc(STR(0));
}

static void f_memstat (Func_t *func, void *rval, void **arg)
{
	memstat(STR(0));
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
	RVSTR = fn ? mstrcpy(fn->path) : NULL;
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


/*	Funktionstabelle
*/

static FuncDef_t fdef_pctrl[] = {
	{ 0, &Type_void, "exit (int rval = 0)", f_exit },
	{ 0, &Type_void, "system (str cmd = NULL)", f_system },
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
};


/*	Initialisierung
*/

void CmdSetup_pctrl(void)
{
	AddFuncDef(fdef_pctrl, tabsize(fdef_pctrl));
}
