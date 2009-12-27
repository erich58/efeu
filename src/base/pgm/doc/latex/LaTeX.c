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

static VarTab_t *LaTeX_tab = NULL;

static VarDef_t vardef[] = {
	{ "latex",	&Type_vtab, &LaTeX_tab },
};

char *LaTeX_var (const char *name)
{
	return Obj2str(GetVar(LaTeX_tab, name, NULL));
}

void LaTeX_psub (LaTeX_t *ltx, const char *name)
{
	char *fmt = LaTeX_var(name);
	io_psub(ltx->out, fmt);
	memfree(fmt);
}

static void par_info (io_t *io, InfoNode_t *info)
{
	LaTeX_ShowCmd(io);
	LaTeX_ShowEnv(io);
}

void LaTeX_setup (void)
{
	static int need_setup = 1;

	if	(need_setup)
	{
		io_t *in;

		LaTeX_tab = VarTab("LaTeX", 64);
		AddVarDef(NULL, vardef, tabsize(vardef));
		PushVarTab(RefVarTab(LaTeX_tab), NULL);
		LaTeX_SetupEnv();
		LaTeX_SetupCmd();

		AddInfo(NULL, "latex", INFO_LABEL, par_info, NULL);

		in = io_findopen(CFGPATH, "latex", CFGEXT, "rd");
		in = io_cmdpreproc(in);
		CmdEval(in, NULL);
		io_close(in);

		PopVarTab();
		need_setup = 0;
	}
}
