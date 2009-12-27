/*
:*:	command interpreter
:de:	Befehlsinterpreter

$Copyright (C) 1994 Erich Frühstück
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

#include <EFEU/Resource.h>
#include <EFEU/CmdPar.h>
#include <EFEU/pconfig.h>
#include <EFEU/cmdeval.h>
#include <EFEU/preproc.h>
#include <EFEU/mdmat.h>
#include <EFEU/efutil.h>
#include <EFEU/database.h>
#include <EFEU/Random.h>
#include <EFEU/Pixmap.h>
#include <EFEU/Debug.h>
#include <Math/TimeSeries.h>
#include <Math/pnom.h>
#include <Math/mdmath.h>

#define	HNAME	"~/.esh_history"
#define	EFMAIN	"efmain"

#define	CMDTYPE	GetResource("CmdType", NULL)


int main(int narg, char **arg)
{
	CmdPar_t *par;
	io_t *in;

	SetProgName(arg[0]);

	SetupStd();
	SetupUtil();
	SetupPreproc();

	SetupDataBase();
	SetupTimeSeries();
	SetupRand48();
	SetupRandom();
	SetupMdMat();
	SetupMath();
	SetupMdMath();
	SetupPnom();
	SetupSC();
	SetupPixmap();
	SetupReadline();
	SetupDebug();

	par = CmdPar_alloc(ProgName);
	CmdPar_load(par, "efm", 0);
	CmdPar_load(par, par->name, 1);

	if	(CmdPar_eval(par, &narg, arg, 1) <= 0)
		exit(EXIT_FAILURE);

	DebugMode(CmdPar_getval(par, "Debug", NULL));

	pconfig_init();
	applfile(EFMAIN, APPL_APP);
	pconfig_exit();

	if	(!EshConfig(&narg, arg))
	{
		if	(narg > 1)
			skiparg(&narg, arg, 1);

		in = io_interact(NULL, HNAME);
	}
	else
	{
		char *type = CmdPar_getval(par, "CmdType", NULL);
		char *fname = fsearch(IncPath, NULL, arg[0], type);
		in = io_fileopen(fname ? fname : arg[0], "r");
		memfree(fname);
	}

	in = io_cmdpreproc(in);

	if	(atoi(CmdPar_getval(par, "PPOnly", "0")))
	{
		int c;

		while ((c = io_getc(in)) != EOF)
			io_putc(c, iomsg);
	}
	else
	{
		EshIdent(in);
		CmdEval(in, iomsg);
	}

	io_close(in);
	return 0;
}
