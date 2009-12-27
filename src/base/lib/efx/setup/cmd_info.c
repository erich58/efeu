/*
Standardfunktionen

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
#include <EFEU/cmdconfig.h>
#include <EFEU/printobj.h>
#include <EFEU/locale.h>
#include <EFEU/preproc.h>
#include <EFEU/pconfig.h>

#define	STR(n)	Val_str(arg[n])
#define	INT(n)	Val_int(arg[n])
#define	IO(n)	Val_io(arg[n])


static void f_info (EfiFunc *func, void *rval, void **arg)
{
	PrintInfo(IO(1), NULL, STR(0));
}

static void f_dump (EfiFunc *func, void *rval, void **arg)
{
	DumpInfo(IO(1), GetInfo(NULL, NULL), STR(0));
}

static void f_load (EfiFunc *func, void *rval, void **arg)
{
	LoadInfo(AddInfo(NULL, STR(0), NULL, NULL, NULL), STR(1));
}

static void f_getinfo (EfiFunc *func, void *rval, void **arg)
{
	StrBuf *sb = new_strbuf(0);
	IO *io = io_strbuf(sb);
	PrintInfo(io, NULL, STR(0));
	io_close(io);
	Val_str(rval) = sb2str(sb);
}

static void f_addinfo (EfiFunc *func, void *rval, void **arg)
{
	AddInfo(NULL, STR(0), STR(1), NULL, mstrcpy(STR(2)));
}


static EfiFuncDef fdef_info[] = {
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
