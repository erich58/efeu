/*
Parameterdefinitionen

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

static ALLOCTAB(deftab, 1024, sizeof(CmdParDef));

/*
Die Funktion |$1| liefert eine neue Parameterdefinition.
*/

CmdParDef *CmdParDef_alloc (void)
{
	return new_data(&deftab);
}

/*
Die Funktion |$1| gibt den Inhalt einer Parameterdefinition frei.
*/

void CmdParDef_free (CmdParDef *def)
{
	if	(def == NULL)	return;

	CmdParKey_free(def->key);
	CmdParCall_free(def->call);
	memfree(def->desc);
	del_data(&deftab, def);
}

/*
Die Funktion |$1| erweitert die Kommandoparameter um eine Parameterdefinition.
*/

void CmdPar_add (CmdPar *par, CmdParDef *def)
{
	CmdParKey *key;

	if	(def == NULL)	return;

	par = CmdPar_ptr(par);

	((CmdParDef **) vb_next(&par->def))[0] = def;

	for (key = def->key; key != NULL; key = key->next)
	{
		key->def = def;

		switch (key->partype)
		{
		case PARTYPE_ENV:
			((CmdParKey **) vb_next(&par->env))[0] = key;
			break;
		case PARTYPE_OPT:
			((CmdParKey **) vb_next(&par->opt))[0] = key;
			break;
		case PARTYPE_ARG:
			((CmdParKey **) vb_next(&par->arg))[0] = key;
			break;
		}
	}
}

/*
Die Funktion |$1| wertet eine Parameterdefinition aus.
*/

int CmdParDef_eval (CmdPar *par, CmdParDef *def, const char *arg)
{
	int rval;
	CmdParCall *call;
	
	par = CmdPar_ptr(par);

	for (rval = 0, call = def->call; call != NULL; call = call->next)
	{
		rval |= CmdParCall_eval(par, call, arg);

		if	(rval & CMDPAR_END)	break;
	}

	return rval;
}

/*
$SeeAlso
\mref{CmdPar(3)},
\mref{CmdParCall(3)},
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

