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
	int (*eval) (IO *io);
} CmdDef;

static int f_help(IO *io);
static int f_print(IO *io);
static int f_quit(IO *io);
static int f_int(IO *io);
static int f_delete(IO *io);

static CmdDef CmdTab[] = {
	{ "?", "Befehle auflisten", f_help },
	{ "h", "Befehle auflisten", f_help },
	{ "help", "Befehle auflisten", f_help },
	{ "c", "Fortfahren", NULL },
	{ "cont", "Fortfahren", NULL },
	{ "p", "Ausdruck ausgeben", f_print },
	{ "print", "Ausdruck ausgeben", f_print },
	{ "quit", "Verarbeitung abbrechen", f_quit },
	{ "delete", "Beobachtungspunkt löschen", f_delete },
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

static int f_help(IO *io)
{
	int i;

	for (i = 0; i < tabsize(CmdTab); i++)
		io_xprintf(io, "%s\t%s\n", CmdTab[i].name, CmdTab[i].desc);

	return 0;
}

static int f_quit(IO *io)
{
	io_close(io);
	exit(0);
	return 0;
}


static int delete_flag = 0;

static int f_delete(IO *io)
{
	delete_flag = 1;
	return EOF;
}

static int f_int(IO *io)
{
	raise(SIGINT);
	return 0;
}

static int f_print(IO *io)
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
	return 0;
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
			io_xprintf(io, "Unbekanntes Befehlsword %#s\n", p);
		}
		else if	(cmd->eval)
		{
			c = cmd->eval(io);
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

	io_xprintf(ioerr, "Breakpoint %s\n", Val_str(list->obj->data));
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

/* WATCH */
typedef struct {
	LVALOBJ_VAR;
	int id;
} WATCH;


#define	WATCH_SIZE(type) (sizeof(WATCH) + type->size)

static int watch_id = 0;

static void watch_free (EfiObj *obj)
{
	WATCH *wp = (WATCH *) obj;
	io_xprintf(ioerr, "Watchpoint %d deleted\n", wp->id);
	CleanData(wp->type, wp->data, 1);
	UnrefObj(wp->base);
}

static void watch_update (EfiObj *obj)
{
	WATCH *wp;
	IO *io;

	wp = (WATCH *) obj;
	io_xprintf(ioerr, "Watchpoint %d\n", wp->id);
	io_puts("old value: ", ioerr);
	PrintData(ioerr, wp->type, wp->data);
	CopyData(wp->type, wp->data, wp->base->data);
	io_puts("\nnew value: ", ioerr);
	PrintData(ioerr, wp->type, wp->data);
	io_puts("\n", ioerr);

	io = io_interact("debug>> ", NULL);
	delete_flag = 0;
	CmdDebug(io);
	io_close(io);

	if	(delete_flag)
		rd_deref(obj);
}

static void watch_unlink (EfiObj *obj)
{
	((WATCH *) obj)->base = NULL;
	rd_deref(obj);
}

static EfiLval watch_lval = {
	"watch", NULL,
	NULL, watch_free,
	watch_update, NULL,
	watch_unlink, NULL,
};

static EfiObj *do_watch(void *ptr, const EfiObjList *list)
{
	EfiObj *base = EvalObj(RefObj(list->obj), NULL);

	if	(base && base->lval)
	{
		WATCH *wp = Lval_alloc(sizeof *wp + base->type->size);
		wp->type = base->type;
		wp->lval = &watch_lval;
		wp->base = base;
		wp->id = ++watch_id;
		wp->data = (wp + 1);
		CopyData(base->type, wp->data, base->data);
		AddUpdateObj(&base->list, (EfiObj *) wp);
	}
	else	fprintf(stderr, "kein L-Wert.\n");

	UnrefObj(base);
	return NULL;
}

static EfiObj *parse_watch(IO *io, void *data)
{
	EfiObj *obj = Parse_term(io, 0);
	return Obj_call(do_watch, NULL, MakeObjList(1, obj));
}

static EfiParseDef pdef[] = {
	{ "breakpoint", parse_bpoint, NULL },
	{ "sigint", parse_sigint, NULL },
	{ "watch", parse_watch, NULL },
};

void SetupDebug ()
{
	AddParseDef(pdef, tabsize(pdef));
}
