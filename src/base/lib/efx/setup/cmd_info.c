/*	Standardfunktionen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/ftools.h>
#include <EFEU/object.h>
#include <EFEU/cmdconfig.h>
#include <EFEU/printobj.h>
#include <EFEU/locale.h>
#include <EFEU/preproc.h>
#include <EFEU/pconfig.h>

#define	STR(n)	Val_str(arg[n])
#define	INT(n)	Val_int(arg[n])
#define	IO(n)	Val_io(arg[n])


static void f_info (Func_t *func, void *rval, void **arg)
{
	PrintInfo(IO(1), NULL, STR(0));
}

static void f_dump (Func_t *func, void *rval, void **arg)
{
	DumpInfo(IO(1), GetInfo(NULL, NULL), STR(0));
}

static void f_load (Func_t *func, void *rval, void **arg)
{
	LoadInfo(AddInfo(NULL, STR(0), NULL, NULL, NULL), STR(1));
}

static void f_getinfo (Func_t *func, void *rval, void **arg)
{
	strbuf_t *sb = new_strbuf(0);
	io_t *io = io_strbuf(sb);
	PrintInfo(io, NULL, STR(0));
	io_close(io);
	Val_str(rval) = sb2str(sb);
}

static void f_addinfo (Func_t *func, void *rval, void **arg)
{
	AddInfo(NULL, STR(0), mstrcpy(STR(1)), NULL, mstrcpy(STR(2)));
}


static FuncDef_t fdef_info[] = {
	{ 0, &Type_void, "Info (str name = NULL, IO io = cout)", f_info },
	{ 0, &Type_void, "InfoDump (str name = NULL, IO io = cout)", f_dump },
	{ 0, &Type_void, "AddInfo (str name, str label = NULL, str txt = NULL)",
		f_addinfo },
	{ 0, &Type_str, "GetInfo (str name)", f_getinfo },
	{ 0, &Type_void, "LoadInfo (str name, str file)", f_load },
};


void CmdSetup_info(void)
{
	AddFuncDef(fdef_info, tabsize(fdef_info));
}
