/*
:*:	Tcl/Tk interface type
:de:	Tcl/Tk Schnittstellentype

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

#include <GUI/ETK.h>

#if	HAS_TCL

static void etk_clean (void *ptr)
{
	ETK *etk = ptr;
	Tcl_DeleteInterp(etk->interp);
	memfree(ptr);
}

static char *etk_ident (const void *ptr)
{
	return mstrcpy("Tcl/Tk interface");
}

static RefType etk_reftype = REFTYPE_INIT("TK", etk_ident, etk_clean);

static int proc_expr (ClientData data, Tcl_Interp *interp,
		int argc, char **argv)
{
	IO *io = io_mstr(listcat(" ", argv + 1, argc - 1));
	UnrefObj(UnaryTerm("tclset", NULL, EvalObj(Parse_term(io, 0), NULL)));
	io_close(io);
	return TCL_OK;
}

static int proc_eval (ClientData data, Tcl_Interp *interp,
		int argc, char **argv)
{
	IO *io = io_mstr(listcat(" ", argv + 1, argc - 1));
	CmdEval(io, NULL);
	io_close(io);
	return TCL_OK;
}
#endif

/*
:*:
The function |$1| creates a interface to the Tcl/Tk interpreter.
:de:
Die Funktion |$1| erzeugt eine Schnittstelle zu einem Tcl/Tk-Interpreter.
*/

ETK *ETK_create (void)
{
#if	HAS_TCL
	ETK *etk = memalloc(sizeof(ETK));

	etk->interp = Tcl_CreateInterp();
	etk->tkstat = 0;

#if	HAS_TCL_STUBS
	if	(Tcl_InitStubs(etk->interp, TCL_VERSION, 1) == NULL)
		abort();
#endif

	Tcl_FindExecutable(ProgName);
	Tcl_SetVar(etk->interp, "tcl_interactive",
		isatty(0) ? "1" : "0", TCL_GLOBAL_ONLY);

	if	(Tcl_Init(etk->interp) == TCL_ERROR)
#if	HAS_TCL_OBJ
		io_printf(ioerr, "tcl: %s\n", Tcl_GetStringResult(etk->interp));
#else
		io_printf(ioerr, "tcl: %s\n", etk->interp->result);
#endif

	Tcl_CreateCommand(etk->interp, "@expr", proc_expr, NULL, NULL);
	Tcl_CreateCommand(etk->interp, "@eval", proc_eval, NULL, NULL);
	return rd_init(&etk_reftype, etk);
#else
	fprintf(stderr, "sorry: tcl/tk interface not available.\n");
	exit(EXIT_FAILURE);
	return NULL;
#endif
}

/*
$SeeAlso
\mref{refdata(3)},
\mref{ETK_eval(3)},
\mref{ETK_func(3)},
\mref{ETK_parse(3)},
\mref{ETK_var(3)},
\mref{SetupETK(3)},
\mref{Tcl_CreateInterp(3)},
\mref{Tcl_CreateCommand(3)},
\mref{Tk_Init(3)},
\mref{ETK(7)}.
*/
