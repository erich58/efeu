/*
Dokumentation aus Sourcefile generieren

$Copyright (C) 2000 Erich Frühstück
This file is part of EFEU.

EFEU is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

EFEU is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public
License along with EFEU; see the file COPYING.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <EFEU/pconfig.h>
#include <EFEU/strbuf.h>
#include <EFEU/Resource.h>
#include <EFEU/nkt.h>
#include "src2doc.h"
#include <ctype.h>


/*	Globale Variablen
*/

char *DocName = NULL;
char *Secnum = NULL;
char *IncFmt = NULL;
int InsertCode = 0;
int ListAlias = 0;


EfiVarDef globvar[] = {
	{ "DocName", &Type_str, &DocName },
	{ "Secnum",	&Type_str, &Secnum },
	{ "IncFmt",	&Type_str, &IncFmt },
};


static void f_list (EfiFunc *func, void *rval, void **arg)
{
	S2DMode_list(Val_io(arg[0]));
}

static EfiFuncDef fdef[] = {
	{ FUNC_VIRTUAL, &Type_void, "ListMode (IO io = iostd)", f_list },
};

static int print_alias (const char *name, void *data, void *par)
{
	io_xprintf(par, "%s\n", name);
	return 0;
}

/*	Hauptprogramm
*/

int main (int narg, char **arg)
{
	IO *ein;
	IO *aus;
	char *Name;
	char *Mode;
	S2DEval eval;

	SetVersion(EFEU_VERSION);
	SetProgName(arg[0]);
	SetupStd();
	SetupReadline();
	AddVarDef(NULL, globvar, tabsize(globvar));
	AddFuncDef(fdef, tabsize(fdef));
	ParseCommand(&narg, arg);

	if	(DocName && DocName[0] == 0)
	{
		memfree(DocName);
		DocName = NULL;
	}

	Name = GetResource("Name", NULL);
	Mode = GetResource("Mode", NULL);
	InsertCode = GetFlagResource("InsertCode");
	ListAlias = GetFlagResource("ListAlias");
	eval = Mode ? S2DMode_get(Mode) : S2DName_get(DocName);

	if	(eval == NULL)
		log_error(NULL, "[1]", "s", Mode);

	if	(!Name) 
		Name = mbasename(DocName, NULL);

	ein = DocName ? io_fileopen(DocName, "rzd") :
		io_interact(NULL, "s2d_hist");
	ein = io_lnum(ein);
	aus = ListAlias ? NULL : io_fileopen(GetResource("Output", NULL), "wzd");
	eval(Name, ein, aus);

/*	Leerzeile am Ende verhindert das unerwartete Verschwinden von 
	nachfolgenden Texten beim Zusammenfügen mehrerer Ausgabedateien.
*/
	io_putc('\n', aus);
	io_close(ein);
	io_close(aus);

	if	(ListAlias)
	{
		nkt_walk(AliasTab, print_alias, iostd);
	}

	return 0;
}
