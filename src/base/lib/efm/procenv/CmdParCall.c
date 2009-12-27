/*
Parameteraufrufe

$Copyright (C) 2001 Erich Frühstück
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

#include <EFEU/CmdPar.h>
#include <EFEU/parsub.h>
#include <EFEU/mstring.h>
#include <EFEU/memalloc.h>

static ALLOCTAB(calltab, 100, sizeof(CmdParCall_t));

/*
Die Funktion |$1| liefert eine neue Aufrufdefinition
*/

CmdParCall_t *CmdParCall_alloc (void)
{
	return new_data(&calltab);
}

/*
Die Funktion |$1| gibt eine Aufrufdefinition frei.
*/

void CmdParCall_free (CmdParCall_t *call)
{
	if	(call == NULL)	return;

	memfree(call->name);
	memfree(call->par);
	CmdParCall_free(call->next);
	del_data(&calltab, call);
}

/*
Die Funktion |$1| wertet eine Aufrufdefinition aus.
*/

int CmdParCall_eval (CmdPar_t *par, CmdParCall_t *def, const char *arg)
{
	CmdParVar_t *var;
	
	par = CmdPar_ptr(par);
	var = CmdPar_var(par, def->name, 1);

	if	(def->eval)
	{
		CmdParVar_t buf;
		int rval;

		memset(&buf, 0, sizeof(CmdParVar_t));

		if	(var == NULL)	var = &buf;

		rval = def->eval->func(par, var, def->par, arg);
		memfree(buf.value);
		return rval;
	}
	else if	(var)
	{
		var->value = mstrcpy(def->par ? def->par : arg);
		return 0;
	}
	else	return CMDPAR_KEEP;
}

/*
$SeeAlso
\mref{CmdPar(3)},
\mref{CmdParDef(3)},
\mref{CmdParEval(3)},
\mref{CmdParExpand(3)},
\mref{CmdParKey(3)},
\mref{CmdParVar(3)},
\mref{CmdPar_eval(3)},
\mref{CmdPar_list(3)},
\mref{CmdPar_load(3)},
\mref{CmdPar_psub(3)},
\mref{CmdPar_usage(3)},
\mref{CmdPar_write(3)},
\mref{Resource(3)},
\mref{CmdPar(7)}.
*/

