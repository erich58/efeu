/*
Parameterkennungen

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

static ALLOCTAB(keytab, "CmdParKey", 30, sizeof(CmdParKey));

/*
Die Funktion |$1| liefert eine neue Parameterkennung
*/

CmdParKey *CmdParKey_alloc (void)
{
	return new_data(&keytab);
}

/*
Die Funktion |$1| gibt den Inhalt einer Parameterkennung frei.
*/

void CmdParKey_free (CmdParKey *key)
{
	if	(key == NULL)	return;

	memfree(key->key);
	memfree(key->val);
	memfree(key->arg);
	CmdParKey_free(key->next);
	del_data(&keytab, key);
}

/*
$SeeAlso
\mref{CmdPar(3)},
\mref{CmdParCall(3)},
\mref{CmdParDef(3)},
\mref{CmdParEval(3)},
\mref{CmdParExpand(3)},
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

