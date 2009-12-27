/*
:*:	interface to Tcl/Tk
:de:	Schnittstelle zu Tcl/Tk

$Header <GUI/ETK.h>
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

#ifndef	GUI_ETK_h
#define	GUI_ETK_h	1

#include <EFEU/object.h>
#include <GUI/tk_config.h>

#if	HAS_TCL

#include <tcl.h>
#include <tk.h>

/*
:de:
Der Datentyp |$1| repräsentiert eine Verbindung zum Tcl/Tk Interpreter.
:*:
The structure |$1| represents a connection to the Tcl/Tk interpreter.
*/

typedef struct {
	REFVAR;
	Tcl_Interp *interp;
	int tkstat;
} ETK;

#else

#define	ETK	RefData

#endif

extern ETK *ETK_create (void);
extern EfiObj *ETK_var (ETK *etk, const char *name);
extern EfiObj *ETK_parse (ETK *etk, IO *io);
extern void ETK_func (ETK *etk, IO *io);

#if	HAS_TCL_OBJ
extern EfiObj *ETK_obj (Tcl_Obj *obj);
#endif

extern int ETK_eval (ETK *etk, char *cmd);
extern EfiObj *ETK_result (ETK *etk);

extern void ETKInfo (const char *name);
extern void SetupETK (void);

/*
$SeeAlso
\mref{ETK(3)},
\mref{ETK_eval(3)},
\mref{ETK_func(3)},
\mref{ETK_parse(3)},
\mref{ETK_var(3)},
\mref{SetupETK(3)}.
*/

#endif	/* GUI/ETK.h */
