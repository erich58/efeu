/*
Steuerbefehle

$Copyright (C) 1999 Erich Frühstück
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

#include <EFEU/mstring.h>
#include <EFEU/procenv.h>
#include <EFEU/parsub.h>
#include <EFEU/ioctrl.h>
#include <DocCtrl.h>
#include <mroff.h>
#include <efeudoc.h>

static char *dochead = NULL;

static VarDef_t vardef[] = {
	{ "dochead", &Type_str, &dochead },
};

extern void mroff_cmdpar (VarTab_t *tab)
{
	AddVarDef(tab, vardef, tabsize(vardef));
}

void mroff_hdr (mroff_t *mr, int mode)
{
	mroff_newline(mr);

	if	(mode)
	{
		io_psub(mr->out, dochead);
		mr->nlignore = 1;
	}
}


int mroff_cmd (mroff_t *mr, va_list list)
{
	int cmd;
	
	cmd = va_arg(list, int);

	switch (cmd)
	{
	case DOC_CMD_TOC:
		mroff_newline(mr);
		mroff_rem(mr, "Inhaltsverzeichnis");
		break;
	case DOC_CMD_BREAK:
		mroff_cmdline(mr, mr->item ? ".TP" : ".br");
		break;
	case DOC_CMD_ITEM:
		mroff_cmdline(mr, ".IP *");
		break;
	default:
		return EOF;
	}

	return 0;
}
