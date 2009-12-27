/*	Eingebaute Mischbefehle
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#include "eisdoc.h"

#ifndef	PATHSEP
#define	PATHSEP	":"
#endif

CmdTab_t *builtin = NULL;

static void builtin_eval (Cmd_t *cmd, ArgList_t *arg)
{
	((CmdEval_t) cmd->par) (cmd, arg);
}

static CmdType_t builtin_type = { "builtin", NULL, NULL, builtin_eval };


static void add_builtin(const char *name, int narg, void *par, const char *desc)
{
	Cmd_t *cmd = AddCmd(builtin, name);
	
	if	(cmd != NULL)
	{
		cmd->type = &builtin_type;
		cmd->narg = narg;
		cmd->par = par;
		cmd->desc = mstrcpy(desc);
	}
}

/*	Verarbeitungskontrolle
*/

static void ce_break(Cmd_t *cmd, ArgList_t *arg)
{
	MergeNewline();
	MergeInput(NULL);
}


static void ce_load(Cmd_t *cmd, ArgList_t *arg)
{
	Merge_t save;

	PushMerge(&save);
	MergeInput(OpenInput(IncPath,
		arg->arg[0] ? arg->arg[0] : CFG_NAME,
		arg->arg[1] ? arg->arg[1] : CFG_EXT));
	MergeOutput(NULL);
	MergeEval(EOF, EOF);
	PopMerge(&save);
}

static void ce_filter(Cmd_t *cmd, ArgList_t *arg)
{
	MergeFilter(arg->arg[0]);
}

static void ce_info(Cmd_t *cmd, ArgList_t *arg)
{
	MergeNewline();
	PrintInfo(MergeOut, NULL, arg->arg[0]);
	MergeNewline();
}

static void ce_list(Cmd_t *cmd, ArgList_t *arg)
{
	MergeNewline();
	ListMergeCmd(MergeOut);
}

/*	Abfragefunktionen
*/

static void ce_the(Cmd_t *cb, ArgList_t *arg)
{
	Cmd_t *cmd;
	char buf[20];
	char *str;

	if	((cmd = GetMergeCmd(arg->arg[0])) == NULL)
		return;

	switch (arg->arg[1] ? arg->arg[1][0] : 0)
	{
	case 't':	str = cmd->type->name; break;
	case 'n':	str = cmd->name; break;
	case 'd':	str = cmd->desc; break;
	case '#':	sprintf(buf, "%d", cmd->narg); str = buf; break;
	default:
		if	(cmd->type->show)
			cmd->type->show(cmd, MergeOut);
		return;
	}

/*
//	FormatString(str, MergeOut, arg->opt);
*/
	MergePuts(str);
}

static void ce_include(Cmd_t *cmd, ArgList_t *arg)
{
	if	(arg->arg[0])
	{
		Merge_t save;

		PushMerge(&save);
		MergeInput(OpenInput(arg->opt ? arg->opt : IncPath,
			arg->arg[0], arg->arg[1]));
		MergeEval(EOF, EOF);
		PopMerge(&save);
	}
}


static void ce_xexpr(Cmd_t *cmd, ArgList_t *arg)
{
	io_t *in = io_cmdpreproc(io_cstr(arg->arg[0]));
	CmdEval(in, MergeOut);
	io_close(in);
}

static void ce_noexpand(Cmd_t *cmd, ArgList_t *arg)
{
	MergeStat.cmdkey = EOF;
}

static void ce_keepcomment(Cmd_t *cmd, ArgList_t *arg)
{
	MergeStat.comkey = EOF;
}

static void ce_lastcomment(Cmd_t *cmd, ArgList_t *arg)
{
	char *p = MergeComment();
	MergePuts(p);
	memfree(p);
}

static void ce_clearcomment(Cmd_t *cmd, ArgList_t *arg)
{
	memfree(MergeComment());
}


void BuiltinCommand(void)
{
	builtin = NewCmdTab("builtin");

	add_builtin("def", 3, ce_def, "Makrobefehl definieren");
	add_builtin("undef", 1, ce_undef, "Makrobefehl löschen");

	add_builtin("the", 2, ce_the, "Definition eines Befehls abfragen");
	add_builtin("info", 1, ce_info, "Infodatenbank abfragen");
	add_builtin("list", 0, ce_list, "Befehle auflisten");

	add_builtin("opt", 1, ce_opt, "Option eines Mischbefehls");
	add_builtin("arg", 2, ce_arg, "Argument eines Mischbefehls");
	add_builtin("end", 0, ce_break,
		"Ende der Verarbeitung in der aktuellen Eingabe");
	add_builtin("eval", -1, ce_eval, "Befehlsinterpreter starten");
	add_builtin("expr", 1, ce_xexpr, "Argument auswerten");
	add_builtin("load", 1, ce_load, "Konfigurationsdatei laden");
	add_builtin("include", 2, ce_include, "Datei einbinden");
	add_builtin("noexpand", 0, ce_noexpand, "Befehle nicht expandieren");
	add_builtin("keepcomment", 0, ce_keepcomment,
		"Kommentare nicht entfernen");
	add_builtin("lastcomment", 0, ce_lastcomment,
		"Letztes Kommentar abfragen und löschen");
	add_builtin("clearcomment", 0, ce_clearcomment,
		"Letztes Kommentar löschen");
	add_builtin("begin", -1, ce_begin, "Befehlsumgebung");
	add_builtin("if", 2, ce_if, "Testblock");
	add_builtin("filter", 1, ce_filter, "Ausgabefilter setzen");
}
