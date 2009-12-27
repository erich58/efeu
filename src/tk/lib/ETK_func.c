/*
:*:	define function as tcl command
:de:	Funktion als Tcl-Kommando definieren

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
#include <EFEU/stdtype.h>
#include <EFEU/parsedef.h>
#include <EFEU/printobj.h>
#include <ctype.h>

#define	PROMPT	"tclfunc% "

#if	HAS_TCL

#if	HAS_TCL_OBJ
#define	ARGDECL		Tcl_Obj * const argv[]
#define	ARGVAL(x)	ETK_obj(argv[x])
#define	CREATE		Tcl_CreateObjCommand
#else
#define	ARGDECL		char *argv[]
#define	ARGVAL(x)	str2Obj(argv[x])
#define	CREATE		Tcl_CreateCommand
#endif

static int proc_func (ClientData data, Tcl_Interp *interp, int argc, ARGDECL)
{
	EfiObjList *list, **ptr;
	EfiObj *obj;
	int i;

	list = NULL;
	ptr = &list;

	for (i = 1; i < argc; i++)
	{
		*ptr = NewObjList(ARGVAL(i));
		ptr = &(*ptr)->next;
	}

	obj = EvalFunc(data, list);

	if	(obj)
		UnrefObj(UnaryTerm("tclset", NULL, obj));

	DelObjList(list);
	return TCL_OK;
}
#endif

static EfiObj *parse_body (IO *io)
{
	char *prompt;
	EfiObj *obj;
	int c;

	prompt = io_prompt(io, PROMPT);
	c = io_eat(io, "%s");

	if	(c == '{')
	{
		io_getc(io);
		obj = Parse_block(io, '}');
	}
	else
	{
		obj = Parse_term(io, 0);

		if	(io_eat(io, " \t") == ';')
			io_getc(io);
	}

	io_prompt(io, prompt);
	return obj;
}

static void oclean (void *data)
{
	UnrefObj(data);
}

/*
:*:The function |$1| reads a function definition from <io> and registers it
as Tcl command.
:de:Die Funktion |$1| liest ein Funktionsdefinition aus <io> und
Registriert sie als Tcl-Kommando.
*/

void ETK_func (ETK *etk, IO *io)
{
	EfiName fname;
	EfiFunc *func;
	int c;

	c = io_eat(io, " \t");

	switch (c)
	{
	case EOF:
	case '\n':
		fprintf(stderr, "missing name\n");
		return;
	case '"':
		io_getc(io);
		fname.name = io_xgets(io, "\"");
		io_getc(io);
		break;
	default:
		fname.name = io_mgets(io, "%s");
		break;
	}
	
	fname.obj = NULL;
	func = MakePrototype(io, &Type_void, &fname, 0);

	if	(func)
	{
		func->par = parse_body(io);
		func->clean = oclean;
#if	HAS_TCL
		CREATE(etk->interp, func->name, proc_func, func, rd_clean);
#else
		rd_deref(func);
#endif
	}
}

/*
$SeeAlso
\mref{ETK(3)},
\mref{ETK_eval(3)},
\mref{ETK_parse(3)},
\mref{ETK_var(3)},
\mref{SetupETK(3)},
\mref{Tcl_CreateCommand(3)},
\mref{ETK(7)}.
*/
