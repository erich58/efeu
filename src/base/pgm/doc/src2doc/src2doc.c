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
#include "src2doc.h"
#include <ctype.h>


/*	Globale Variablen
*/

char *DocName = NULL;
char *Secnum = NULL;
char *IncFmt = NULL;

VarDef_t globvar[] = {
	{ "DocName", &Type_str, &DocName },
	{ "Secnum",	&Type_str, &Secnum },
	{ "IncFmt",	&Type_str, &IncFmt },
};


static void f_list (Func_t *func, void *rval, void **arg)
{
	S2DMode_list(Val_io(arg[0]));
}

static FuncDef_t fdef[] = {
	{ FUNC_VIRTUAL, &Type_void, "ListMode (IO io = iostd)", f_list },
};


/*	Hauptprogramm
*/

int main (int narg, char **arg)
{
	io_t *ein;
	io_t *aus;
	char *Name;
	char *Mode;
	S2DEval_t eval;

	SetVersion("$Id: src2doc.c,v 1.11 2001-11-06 13:21:20 ef Exp $");
	SetProgName(arg[0]);
	SetupStd();
	SetupReadline();
	AddVarDef(NULL, globvar, tabsize(globvar));
	AddFuncDef(fdef, tabsize(fdef));
	ParseCommand(&narg, arg);

	Name = GetResource("Name", NULL);
	Mode = GetResource("Mode", NULL);
	eval = Mode ? S2DMode_get(Mode) : S2DName_get(DocName);

	if	(eval == NULL)
	{
		message(NULL, NULL, 1, 1, Mode);
		exit(EXIT_FAILURE);
	}

	if	(!Name && DocName)
	{
		fname_t *fn = strtofn(DocName);

		if	(fn)
		{
			fn->path = NULL;
			Name = fntostr(fn);
			memfree(fn);
		}
	}

	ein = DocName ? io_fileopen(DocName, "rzd") :
		io_interact(NULL, "s2d_hist");
	ein = io_lnum(ein);
	aus = io_fileopen(GetResource("Output", NULL), "wzd");
	eval(Name, ein, aus);

/*	Leerzeile am Ende verhindert das unerwartete Verschwinden von 
	nachfolgenden Texten beim Zusammenfügen mehrerer Ausgabedateien.
*/
	io_putc('\n', aus);
	io_close(ein);
	io_close(aus);
	return 0;
}
