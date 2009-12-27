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

static EfiVarTab *ManRoff_tab = NULL;

static EfiVarDef vardef[] = {
	{ NAME,	&Type_vtab, &ManRoff_tab },
};

char *mroff_par (const char *name)
{
	return Obj2str(GetVar(ManRoff_tab, name, NULL));
}

static void var_desc (IO *io, const char *desc)
{
	io = io_lmark(io_refer(io), "\t", NULL, 0);
	io_puts(desc, io);
	io_close(io);
}

static void par_info (IO *io, InfoNode *info)
{
	VarTabEntry *p;
	size_t n;

	for (p = ManRoff_tab->tab.data, n = ManRoff_tab->tab.used; n-- > 0; p++)
	{
		EfiObj *obj = p->get ? p->get(NULL, p->data) : RefObj(p->obj);

		io_xprintf(io, "\n%s %s = ", p->type->name, p->name);
		PrintObj(io, obj);
		UnrefObj(obj);
		io_putc('\n', io);
		var_desc(io, p->desc);
	}
}


void mroff_addpar (EfiVarDef *def, size_t dim)
{
	if	(ManRoff_tab == NULL)
		ManRoff_tab = VarTab(NAME, 64);

	AddVarDef(ManRoff_tab, def, dim);
}

void mroff_setup (void)
{
	static int need_setup = 1;

	if	(need_setup)
	{
		IO *in;

		ManRoff_tab = VarTab(NAME, 64);
		mroff_cmdpar(ManRoff_tab);
		mroff_envpar(ManRoff_tab);
		AddVarDef(NULL, vardef, tabsize(vardef));
		PushVarTab(RefVarTab(ManRoff_tab), NULL);
		AddInfo(NULL, NAME, INFO_LABEL, par_info, NULL);

		in = io_findopen(CFGPath, CFGNAME, CFGEXT, "rd");
		in = io_cmdpreproc(in);
		CmdEval(in, NULL);
		io_close(in);

		PopVarTab();
		need_setup = 0;
	}
}
