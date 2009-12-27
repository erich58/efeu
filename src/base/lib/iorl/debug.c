/*	Unterbrechungspunkt
	(c) 1997 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
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
	void (*eval) (io_t *io);
} CmdDef_t;

static void f_help(io_t *io);
static void f_print(io_t *io);
static void f_quit(io_t *io);
static void f_int(io_t *io);

static CmdDef_t CmdTab[] = {
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

static CmdDef_t *GetCmd(const char *name)
{
	int i;

	for (i = 0; i < tabsize(CmdTab); i++)
		if (mstrcmp(name, CmdTab[i].name) == 0)
			return CmdTab + i;

	return NULL;
}

static void f_help(io_t *io)
{
	int i;

	for (i = 0; i < tabsize(CmdTab); i++)
		io_printf(io, "%s\t%s\n", CmdTab[i].name, CmdTab[i].desc);
}

static void f_quit(io_t *io)
{
	io_close(io);
	libexit(0);
}

static void f_int(io_t *io)
{
	raise(SIGINT);
}

static void f_print(io_t *io)
{
	Obj_t *obj;

	obj = Parse_term(io, 0);
	obj = EvalObj(obj, NULL);

	if	(obj)
	{
		PrintObj(ioerr, obj);
		io_putc('\n', ioerr);
	}

	UnrefObj(obj);
}


static void CmdDebug(io_t *io)
{
	CmdDef_t *cmd;
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

static Obj_t *do_bpoint(void *ptr, const ObjList_t *list)
{
	io_t *io;
	Obj_t *obj;

	if	(list->next->obj)
	{
		int flag;

		obj = EvalObj(RefObj(list->next->obj), &Type_bool);
		flag = (obj && !Val_bool(obj->data));
		UnrefObj(obj);

		if	(flag)	return NULL;
	}

	io_printf(ioerr, "Breakpoint %s\n", Val_str(list->obj->data));
	io = io_readline("debug>> ", NULL);
	CmdDebug(io);
	io_close(io);
	return NULL;
}


static Obj_t *parse_bpoint(io_t *io, void *data)
{
	Obj_t *obj;

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
		MakeObjList(2, str2Obj(io_ident(io)), obj));
}

static ParseDef_t pdef[] = {
	{ "breakpoint", parse_bpoint, NULL },
};

void SetupDebug ()
{
	AddParseDef(pdef, tabsize(pdef));
}
