/*
Befehlsinterpreter für Zeitreihenanalysen initialisieren

$Copyright (C) 1997 Erich Frühstück
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
#include <Math/TimeSeries.h>


/*	Initialisieren
*/

#if	0
static ParseDef_t pdef[] = {
	{ "TS_YEAR", PFunc_int, (void *) TS_YEAR },
	{ "TS_QUART", PFunc_int, (void *) TS_QUART },
	{ "TS_MONTH", PFunc_int, (void *) TS_MONTH },
	{ "TS_WEEK", PFunc_int, (void *) TS_WEEK1900 },
	{ "TS_DAY", PFunc_int, (void *) TS_DAT1900 },
};
#endif

#if	0
static FuncDef_t func[] = {
};
#endif	


void SetupTimeSeries (void)
{
	CmdSetup_TimeIndex();
	CmdSetup_TimeSeries();
	CmdSetup_OLSKoef();
	CmdSetup_OLSPar();
#if	0
	AddFuncDef(ts_func, tabsize(ts_func));
	AddParseDef(pdef, tabsize(pdef));
#endif
}
