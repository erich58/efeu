/*
Unterbrechungspunkt

$Copyright (C) 1997 Erich Frühstück
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

#include <EFEU/pconfig.h>
#include <EFEU/object.h>
#include <EFEU/printobj.h>
#include <EFEU/parsedef.h>
#include <EFEU/ioctrl.h>
#include <signal.h>


typedef struct {
	char *name;
	char *desc;
	void (*eval) (IO *io);
} CmdDef;

static void f_help(IO *io);
static void f_print(IO *io);
static void f_quit(IO *io);
static void f_int(IO *io);

static CmdDef CmdTab[] = {
	{ "?", "Befehle auflisten", f_help },
	{ "h", "Befehle auflisten", f_help },
	{ "help", "Befehle auflisten", f_help },
	{ "c", "Fortfahren", NULL },
	{ "cont", "Fortfahren", NULL },
	{ "p", "Ausdruck ausgeben", f_print },
	{ "print", "Ausdruck ausgeben", f_print },
	{ "quit", "Verarbeitung abbrechen", f_quit },
	{ "sigint", "Interrupt an Programm senden (gdb-Schnittstelle)", f_int },
};

static CmdDef *GetCmd(const char *name)
{
	int i;

	for (i = 0; i < tabsize(CmdTab); i++)
		if (mstrcmp(name, CmdTab[i].name) == 0)
			return CmdTab + i;

	return NULL;
}

static void f_help(IO *io)
{
	int i;

	for (i = 0; i < tabsize(CmdTab); i++)
		io_printf(io, "%s\t%s\n", CmdTab[i].name, CmdTab[i].desc);
}

static void f_quit(IO *io)
{
	io_close(io);
	exit(0);
}

static void f_int(IO *io)
{
	raise(SIGINT);
}

static void f_print(IO *io)
{
	EfiObj *obj;

	obj = Parse_term(io, 0);
	obj = EvalObj(obj, NULL);

	if	(obj)
	{
		ShowObj(ioerr, obj);
		io_putc('\n', ioerr);
	}

	UnrefObj(obj);
}


static void CmdDebug(IO *io)
{
	CmdDef *cmd;
	char *p;
	int c;

	while ((c = io_eat(io, "%s")) != EOF)
	{
		p = io_mgets(io, "%s");
		cmd = GetCmd(p);

		if	(cmd == NULL)
		{
			io_printf(io, "Unbekanntes Befehlsword %#s\n", p);
		}
		else if	(cmd->eval)
		{
			cmd->eval(io);
		}
		else	c = EOF;

		io_mcopy(io, NULL, "\n");

		if	(c == EOF)	break;
	}
}


/*	Abbruch von Schleifen
*/

static EfiObj *do_bpoint(void *ptr, const EfiObjList *list)
{
	IO *io;
	EfiObj *obj;

	if	(list->next->obj)
	{
		int flag;

		obj = EvalObj(RefObj(list->next->obj), &Type_bool);
		flag = (obj && !Val_bool(obj->data));
		UnrefObj(obj);

		if	(flag)	return NULL;
	}

	io_printf(ioerr, "Breakpoint %s\n", Val_str(list->obj->data));
	io = io_interact("debug>> ", NULL);
	CmdDebug(io);
	io_close(io);
	return NULL;
}


static EfiObj *parse_bpoint(IO *io, void *data)
{
	EfiObj *obj;

	switch (io_eat(io, " \t"))
	{
	case EOF:
	case '\n':
	case ';':
		obj = NULL;
		break;
	default:
		obj = Parse_term(io, 0);
		break;
	}

	return Obj_call(do_bpoint, NULL,
		MakeObjList(2, str2Obj(rd_ident(io)), obj));
}

static EfiObj *parse_sigint(IO *io, void *data)
{
	raise(SIGINT);
	return NULL;
}

static EfiParseDef pdef[] = {
	{ "breakpoint", parse_bpoint, NULL },
	{ "sigint", parse_sigint, NULL },
};

void SetupDebug ()
{
	AddParseDef(pdef, tabsize(pdef));
}
