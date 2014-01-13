/*
:*:evaluate tcl expression
:de:Auswertung von Tcl-Ausdrücken

$Copyright (C) 2002 Erich Frühstück
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

#include <EFEU/object.h>
#include <GUI/ETK.h>

#if	HAS_TCL_OBJ
/*
:*:The function |$1| converts a object of Type |Tcl_Obj| to a object
of type |EfiObj|.
:de:Die Funktion |$1| konvertiert ein Objeckt of Typ |Tcl_Obj| in ein
Objekt des Types |EfiObj|.
*/

EfiObj *ETK_obj (Tcl_Obj *obj)
{
	char *type = (obj && obj->typePtr) ? obj->typePtr->name : NULL;

	if	(mstrcmp(type, "int") == 0)
	{
		int val = 0;
		Tcl_GetIntFromObj(NULL, obj, &val);
		return NewObj(&Type_int, &val);
	}
	else if	(mstrcmp(type, "double") == 0)
	{
		double val = 0;
		Tcl_GetDoubleFromObj(NULL, obj, &val);
		return NewObj(&Type_double, &val);
	}
	else if	(mstrcmp(type, "boolean") == 0)
	{
		int val = 0;
		Tcl_GetBooleanFromObj(NULL, obj, &val);
		return NewObj(&Type_int, &val);
	}
	else if	(obj)
	{
		return str2Obj(mstrcpy(Tcl_GetStringFromObj(obj, NULL)));
	}
	else	return str2Obj(NULL);
}
#endif

/*
:*:The function |$1| evaluates the tcl command <cmd> and returns 1
on success and 0 on faolure.
:de:Die Funktion |$1| wertet den Tcl-Ausdruck <cmd> aus und liefert 1
bei Erfolg und 0 bei einem Fehler.
*/

int ETK_eval (ETK *etk, char *cmd)
{
#if	HAS_TCL
	if	(Tcl_GlobalEval(etk->interp, cmd) == TCL_OK)
		return 1;

#if	HAS_TCL_OBJ
	io_xprintf(ioerr, "tcl: %s\n", Tcl_GetStringResult(etk->interp));
#else
	io_xprintf(ioerr, "tcl: %s\n", etk->interp->result);
#endif
	Tcl_ResetResult(etk->interp);
#endif
	return 0;
}

/*
:*:The function |$1| returns the result of the last tcl command.
:*:Die Funktion liefert das Resultat des letzten Tcl-Kommandos.
*/

EfiObj *ETK_result (ETK *etk)
{
#if	HAS_TCL
	if	(etk)
#if	HAS_TCL_OBJ
		return ETK_obj(Tcl_GetObjResult(etk->interp));
#else
		return str2Obj(mstrcpy(etk->interp->result));
#endif
#endif
	return NULL;
}

/*
$SeeAlso
\mref{ETK(3)},
\mref{ETK_eval(3)},
\mref{ETK_func(3)},
\mref{ETK_parse(3)},
\mref{SetupETK(3)},
\mref{Tcl_GetVar2Ex(3)},
\mref{Tcl_SetVar2Ex(3)},
\mref{ETK(7)}.
*/
