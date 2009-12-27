/*
:*:mroff configuration parameters
:de:mroff Konfigurationsparameter

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
#include <EFEU/printobj.h>
#include <mroff.h>
#include "efeudoc.h"

#define	NAME	"ManPar"
#define	CFGNAME	"mroff"

#define	INFO_LABEL \
	":*:mroff configuration parameter" \
	":de:mroff-Konfigurationsparameter"

static VarTab_t *mroff_tab = NULL;

static VarDef_t vardef[] = {
	{ NAME,	&Type_vtab, &mroff_tab },
};

char *mroff_par (const char *name)
{
	return Obj2str(GetVar(mroff_tab, name, NULL));
}

static void var_desc (io_t *io, Var_t *var)
{
	io = io_lmark(io_refer(io), "\t", NULL, 0);
	io_puts(var->desc, io);
	io_close(io);
}

static void par_info (io_t *io, InfoNode_t *info)
{
	int i;

	for (i = 0; i < mroff_tab->tab.dim; i++)
	{
		Var_t *var = mroff_tab->tab.tab[i];

		io_printf(io, "\n%s %s = ", var->type->name, var->name);
		PrintObj(io, Var2Obj(var, NULL));
		io_putc('\n', io);
		var_desc(io, var);
	}
}


void mroff_addpar (VarDef_t *def, size_t dim)
{
	if	(mroff_tab == NULL)
		mroff_tab = VarTab(NAME, 64);

	AddVarDef(mroff_tab, def, dim);
}

void mroff_setup (void)
{
	static int need_setup = 1;

	if	(need_setup)
	{
		io_t *in;

		mroff_tab = VarTab(NAME, 64);
		mroff_cmdpar(mroff_tab);
		mroff_envpar(mroff_tab);
		AddVarDef(NULL, vardef, tabsize(vardef));
		PushVarTab(RefVarTab(mroff_tab), NULL);
		AddInfo(NULL, NAME, INFO_LABEL, par_info, NULL);

		in = io_findopen(CFGPATH, CFGNAME, CFGEXT, "rd");
		in = io_cmdpreproc(in);
		CmdEval(in, NULL);
		io_close(in);

		PopVarTab();
		need_setup = 0;
	}
}
