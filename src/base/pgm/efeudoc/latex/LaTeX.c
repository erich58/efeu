/*
Initialisierung für LaTeX

$Copyright (C) 2001 Erich Frühstück
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

#include <EFEU/preproc.h>
#include <LaTeX.h>
#include "efeudoc.h"

#define	INFO_LABEL \
	":*:LaTeX configuration parameter" \
	":de:LaTeX-Konfigurationsparameter"

static EfiVarTab *LaTeX_tab = NULL;

static EfiVarDef vardef[] = {
	{ "latex",	&Type_vtab, &LaTeX_tab },
};

static void par_info (IO *io, InfoNode *info)
{
	LaTeX_ShowCmd(io);
	LaTeX_ShowEnv(io);
}

void LaTeX_setup (void)
{
	static int need_setup = 1;

	if	(need_setup)
	{
		IO *in;

		LaTeX_tab = VarTab("LaTeX", 64);
		AddVarDef(NULL, vardef, tabsize(vardef));
		PushVarTab(RefVarTab(LaTeX_tab), NULL);
		LaTeX_SetupEnv();
		LaTeX_SetupCmd();

		AddInfo(NULL, "latex", INFO_LABEL, par_info, NULL);

		in = io_findopen(CFGPath, "latexpar", CFGEXT, "rd");
		in = io_cmdpreproc(in);
		CmdEval(in, NULL);
		io_close(in);

		PopVarTab();
		need_setup = 0;
	}
}
