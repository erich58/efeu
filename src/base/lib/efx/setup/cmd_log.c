/*
:*:     logging control
:de:    Protokollsteuerung

$Copyright (C) 2009 Erich Frühstück
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
#include <EFEU/parsedef.h>
#include <EFEU/cmdsetup.h>
#include <EFEU/LogConfig.h>
#include <EFEU/Op.h>

extern EfiType Type_ArgList;

static int lc_print (const EfiType *st, const void *data, IO *io)
{
	const LogControl *lc = Val_ptr(data);
	char *delim = "=";
	const LogControlEntry *entry;
	int n;

	n = io_printf(io, "%s.%s", lc->name, LogLevelName(lc->level));

	for (entry = lc->entry; entry; entry = entry->next)
	{
		n += io_printf(io, "%s%s", delim, entry->out->name);
		delim = ",";
	}

	return n;
}

static EfiType Type_LogControl = IOREF_TYPE("LogControl", LogControl *,
	NULL, NULL, lc_print);

/*	Variablen
*/

static EfiObj *p_log (IO *io, void *data)
{
	return ExtObj(&Type_LogControl, data);
}

static EfiParseDef pdef_log[] = {
	{ "Error", p_log, &ErrLog },
	{ "Warning", p_log, &WarnLog },
	{ "Notice", p_log, &NoteLog },
	{ "Info", p_log, &InfoLog },
	{ "Debug", p_log, &DebugLog },
	{ "Trace", p_log, &TraceLog },
};

/*	Funktionstabelle
*/

static void str2lc (EfiFunc *func, void *rval, void **arg)
{
	Val_ptr(rval) = LogControl_create(Val_str(arg[0]));
}

static void lc2io (EfiFunc *func, void *rval, void **arg)
{
	Val_io(rval) = LogOpen(Val_ptr(arg[0]));
}

static void f_log_file (EfiFunc *func, void *rval, void **arg)
{
	Val_io(rval) = io_stream(NULL, log_file(Val_ptr(arg[0])), fileclose);
}

static void f_LogConfig (EfiFunc *func, void *rval, void **arg)
{
	LogConfig(Val_str(arg[0]));
}

static void f_LogOutputStat (EfiFunc *func, void *rval, void **arg)
{
	LogOutputStat();
}

static EfiFuncDef func_log[] = {
	{ 0, &Type_LogControl, "LogControl (str name)", str2lc },
	{ 0, &Type_io, "LogControl ()", lc2io },
	{ 0, &Type_io, "LogControl::open ()", f_log_file },
	{ 0, &Type_void, "LogConfig (str def)", f_LogConfig },
	{ 0, &Type_void, "LogOutputStat ()", f_LogOutputStat },
};

void CmdSetup_log (void)
{
	AddType(&Type_LogControl);
	AddParseDef(pdef_log, tabsize(pdef_log));
	AddFuncDef(func_log, tabsize(func_log));
}
