/*	Mischbefehle ausführen/verwalten
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#include "texmerge.h"
#include <memory.h>

#define	BLKSIZE	128
#define	SIZE(n)	((n) * sizeof(Command_t))

static Command_t *CommandTab = NULL;
static int CommandSize = 0;
static int CommandUsed = 0;


/*	Mischbefehle auflisten
*/

char *CommandType(Command_t *cmd)
{
	switch (cmd->type)
	{
	case TM_BUILTIN:	return "builtin";
	case TM_CONTROL:	return "control";
	case TM_MACRO:		return "macro";
	case TM_VAR:		return "var";
	case TM_COMMAND:	return "command";
	default:		return "unknown";
	}
}

static void command_fmt(FILE *file, char *fmt, int limit)
{
	int n;

	if	(fmt == NULL)	return;

	for (n = 0; *fmt != 0; fmt++)
	{
		if	(n >= limit)
		{
			n += fprintf(file, "...");
			break;
		}
		else if	(*fmt == '\n')
		{
			n += fprintf(file, "\\n");
		}
		else if	((unsigned char) *fmt < ' ')
		{
			n += fprintf(file, "^%c", (unsigned char) *fmt + '@');
		}
		else if	((unsigned char) *fmt >= 127)
		{
			n += fprintf(file, "\\%03o", (unsigned char) *fmt);
		}
		else	n += fprintf(file, "%c", *fmt);
	}
}


static void ListCommandLine(FILE *file, Command_t *cmd)
{
	int n;

	n = fprintf(file, "%s[%d]\t", cmd->name, cmd->narg);

	if	(n < 9)	putc('\t', file);

	if	(cmd->desc)
	{
		fprintf(file, cmd->desc, cmd->fmt);
	}
	else
	{
		fprintf(file, "%s\t", CommandType(cmd));
		command_fmt(file, cmd->fmt, 48);
	}

	putc('\n', file);
}

void ListCommand(void)
{
	int i;

	for (i = 0; i < CommandUsed; i++)
		ListCommandLine(stdout, CommandTab + i);
}


/*	Mischbefehl in Datenbank eintragen
*/

static void add_cmd(char *name, int type, CMDEval_t eval,
	int narg, char *fmt, char *desc)
{
	if	(CommandUsed >= CommandSize)
	{
		Command_t *save;

		save = CommandTab;
		CommandSize += BLKSIZE;
		CommandTab = AllocData("CTAB", SIZE(CommandSize));

		if	(CommandUsed)
		{
			memcpy(CommandTab, save, SIZE(CommandUsed));
			FreeData("CTAB", save);
		}
	}

	CommandTab[CommandUsed].name = name;
	CommandTab[CommandUsed].type = type;
	CommandTab[CommandUsed].narg = narg;
	CommandTab[CommandUsed].eval = eval;
	CommandTab[CommandUsed].fmt = fmt;
	CommandTab[CommandUsed].desc = desc;

	if	(VerboseMode > 1)
		ListCommandLine(stderr, CommandTab + CommandUsed);

	CommandUsed++;
}

void CopyCommand(char *name, const char *old)
{
	Command_t *cmd;

	if	((cmd = SearchCommand(old)) != NULL)
		add_cmd(name, cmd->type, cmd->eval, cmd->narg,
			CopyString(cmd->fmt), CopyString(cmd->desc));
}

void AssignCommand(char *name, char *fmt)
{
	Command_t *cmd;

	if	((cmd = SearchCommand(name)) != NULL)
	{
		FreeData("STR", cmd->fmt);
		cmd->fmt = fmt;
	}
	else	FreeData("STR", fmt);
}

void AddSpecial(char *name, CMDEval_t eval, int narg, char *fmt, char *desc)
{
	if	(name == NULL)	return;

	if	(SearchCommand(name) != NULL)
	{
		Error(4, name);
	}
	else	add_cmd(name, TM_BUILTIN, eval, narg, fmt, desc);
}

void AddControl(char *name, CMDEval_t eval, int narg, char *fmt, char *desc)
{
	if	(name == NULL)	return;

	if	(SearchCommand(name) != NULL)
	{
		Error(4, name);
	}
	else	add_cmd(name, TM_CONTROL, eval, narg, fmt, desc);
}

void AddCommand(char *name, int type, int narg, char *fmt)
{
	CMDEval_t eval;
	Command_t *cmd;

	if	(name == NULL)	return;

	if	((cmd = SearchCommand(name)) != NULL)
	{
		if	(type != cmd->type || type == TM_COMMAND)
		{
			Error(4, name);
		}
		else if	(type == TM_MACRO)
		{
			if	(VerboseMode > 1)
				fprintf(stderr, "redef(%s)\n", name);

			cmd->narg = narg;
			cmd->fmt = fmt;
		}

		return;
	}
	
	switch (type)
	{
	case TM_MACRO:		eval = ce_macro; break;
	case TM_VAR:		eval = ce_var; narg = 0; break;
#if	UNIX_VERSION
	case TM_COMMAND:	eval = ce_expr; break;
#else
	case TM_COMMAND:	eval = ce_show; break;
#endif
	default:		return;
	}

	add_cmd(name, type, eval, narg, fmt, NULL);
}


/*	Befehlsdefinition suchen
*/

Command_t *SearchCommand(const char *name)
{
	int i;

	if	(name == NULL)	return NULL;

	for (i = 0; i < CommandUsed; i++)
		if (strcmp(name, CommandTab[i].name) == 0)
			return CommandTab + i;

	return NULL;
}

/*	Befehlsdefinition entfernen
*/

void DeleteCommand(const char *name)
{
	int i;

	if	(name == NULL)	return;

	for (i = 0; i < CommandUsed; i++)
	{
		if	(strcmp(name, CommandTab[i].name) == 0)
		{
			if	(CommandTab[i].type == TM_BUILTIN)
			{
				Error(5, name);
				return;
			}
			else	CommandUsed--;

			if	(VerboseMode > 1)
				fprintf(stderr, "delete(%s)\n", name);

			break;
		}
	}

	while (i < CommandUsed)
	{
		CommandTab[i] = CommandTab[i+1];
		i++;
	}
}
