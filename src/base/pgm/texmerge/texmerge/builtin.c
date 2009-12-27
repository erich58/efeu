/*	Eingebaute Mischbefehle
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#include "texmerge.h"

/*	Befehlsverwaltung
*/

static void ce_def(Command_t *cmd, Input_t *in, Output_t *out, ArgList_t *arg)
{
	AddCommand(CopyString(arg->arg[0]), cmd->fmt[0], 
		arg->opt ? atoi(arg->opt) : 0, CopyString(arg->arg[1]));
	SkipLine(in);
}

static void ce_let(Command_t *cmd, Input_t *in, Output_t *out, ArgList_t *arg)
{
	CopyCommand(CopyString(arg->arg[0]), arg->arg[1]);
	SkipLine(in);
}

static void ce_undef(Command_t *cmd, Input_t *in, Output_t *out, ArgList_t *arg)
{
	DeleteCommand(arg->arg[0]);
	SkipLine(in);
}


/*	Verarbeitungskontrolle
*/

static void ce_break(Command_t *cmd, Input_t *in, Output_t *out, ArgList_t *arg)
{
	out_newline(out);
	in->eof = 1;
}

static void ce_path(Command_t *cmd, Input_t *in, Output_t *out, ArgList_t *arg)
{
	char *p;

	p = NULL;

	if	(arg->arg[0] && arg->arg[0][0])
		p = PasteString(PATHSEP, arg->arg[0], IncPath);

	FreeData("STR", IncPath);
	IncPath = p;
}

static void ce_load(Command_t *cmd, Input_t *in, Output_t *out, ArgList_t *arg)
{
	LoadConfig(arg->arg[0]);
	SkipLine(in);
}


/*	Abfragefunktionen
*/

static void ce_the(Command_t *cb, Input_t *in, Output_t *out, ArgList_t *arg)
{
	Command_t *cmd;
	char buf[20];
	char *str;

	if	((cmd = SearchCommand(arg->arg[0])) == NULL)
		return;

	switch (arg->arg[1] ? arg->arg[1][0] : 0)
	{
	case 't':	str = CommandType(cmd); break;
	case 'n':	str = cmd->name; break;
	case 'd':	str = cmd->desc; break;
	case '#':	sprintf(buf, "%d", cmd->narg); str = buf; break;
	default:	str = cmd->fmt; break;
	}

	out_fputs(str, out, arg->opt);
}

static void ce_input(Command_t *cmd, Input_t *in, Output_t *out, ArgList_t *arg)
{
	char *save = arg->arg[1];

	if	(arg->arg[1] == NULL)	arg->arg[1] = arg->arg[0];

	TexMerge(arg->arg[0], arg->arg[1]);
	ce_macro(cmd, in, out, arg);
	arg->arg[1] = save;
}


static void ce_eval(Command_t *cmd, Input_t *in, Output_t *out, ArgList_t *arg)
{
	MergeEval(arg->arg[0], out);
}


void BuiltinCommand(void)
{
	AddSpecial("let", ce_let, 2, NULL,
		"Mischbefehl kopieren");
	AddSpecial("def", ce_def, 2, "macro",
		"Makrobefehl definieren");
	AddSpecial("newvar", ce_def, 2, "var",
		"Variable einrichten (falls noch nicht definiert)");
	AddSpecial("newcmd", ce_def, 2, "cmd",
		"Befehlsausdruck definieren");
	AddSpecial("undef", ce_undef, 1, NULL,
		"Befehl löschen");
	AddSpecial("the", ce_the, 2, NULL,
		"Definition eines Befehls abfragen");

	AddSpecial("opt", ce_opt, 1, NULL,
		"Option eines Mischbefehls");
	AddSpecial("arg", ce_arg, 2, NULL,
		"Argument eines Mischbefehls");
	AddSpecial("end", ce_break, 0, NULL,
		"Ende der Verarbeitung in der aktuellen Eingabe");
	AddSpecial("eval", ce_eval, 1, NULL,
		"Argument auswerten");
	AddSpecial("path", ce_path, 1, NULL,
		"Suchpfad abfragen/setzen");
	AddSpecial("load", ce_load, 1, NULL,
		"Konfigurationsdatei laden");
	AddSpecial("input", ce_input, 2, "\\input{@arg{1}}",
		"Datei einbinden %s");
	AddSpecial("include", ce_input, 2, "\\include{@arg{1}}",
		"Datei einbinden %s");
	AddControl("begin", ce_block, -1, "end",
		"Befehlsumgebung");
	AddControl("comment", ce_comment, -1, "end",
		"Kommentarblock");
	AddControl("if", ce_if, 2, "end",
		"Test auf Gleichheit der Argumente");
}
