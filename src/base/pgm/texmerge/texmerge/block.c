/*	Blockstrucktur
	(c) 1997 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#include "texmerge.h"
#include <ctype.h>

#define	ELSEKEY		"else"	/* Schlüssel für Alternative */

#define	BLOCK_SKIP	0x1	/* Zeichen ignorieren */
#define	BLOCK_TEST	0x2	/* Testblock */


void SkipLine(Input_t *in)
{
	int c;

	if	(in->nl)
	{
		if	(in->save == '\n')
			in->save = EOF;

		return;
	}

	c = in_getc(in);

	while (c == ' ' || c == '\t')
		c = in_getc(in);

	if	(c != '\n')
		in_ungetc(c, in);
}

static int ElseMergeMode = 0;

static void CopyBlock(Input_t *in, Output_t *out,
	const char *name, const char *key, const char *elsekey)
{
	char *x;
	int c;

	SkipLine(in);

	while ((c = in_getc(in)) != EOF)
	{
		if	(c == '@')
		{
			x = GetCommand(in, out);

			if	(x == NULL)
			{
				;
			}
			else if	(elsekey && strcmp(x, elsekey) == 0)
			{
				MergeMode = ElseMergeMode;
				elsekey = NULL;
				SkipLine(in);
			}
			else if	(strcmp(x, key) == 0)
			{
				FreeData("STR", x);
				x = name ? GetArg(in, '{', '}') : NULL;

				if	(name == NULL)
					;
				else if	(x == NULL)
					InputError(in, 21, name, x);
				else if	(strcmp(x, name) != 0)
					InputError(in, 22, name, x);
					
				FreeData("STR", x);
				SkipLine(in);
				break;
			}
			else	MergeCommand(in, out, x);

			FreeData("STR", x);
		}
		else if	(MergeMode)
		{
			out_putc(c, out);
		}
	}
}

static char *GetBlock(Input_t *in, const char *name, const char *key)
{
	Buffer_t *sb;
	Output_t *out;

	sb = CreateBuffer();
	out = CreateOutput(NULL, NULL, sb);
	CopyBlock(in, out, name, key, NULL);
	CloseOutput(out);
	return CloseBuffer(sb);
}

void ce_block(Command_t *cb, Input_t *in, Output_t *out, ArgList_t *arg)
{
	Command_t *cmd;
	ArgList_t list;
	char *cmdname;
	char *str;
	int i;

	cmdname = arg->dim ? arg->arg[0] : NULL;

	if	(cmdname == NULL)
	{
		str = GetBlock(in, NULL, cb->fmt);

		if	(MergeMode)
			MergeMacro(str, out);

		FreeData("STR", str);
		return;
	}

	if	((cmd = SearchCommand(cmdname)) == NULL)
	{
		InputError(in, 23, cmdname);
		return;
	}

	InitArgList(&list);

	for (i = 1; i < arg->dim; i++)
		AddArg(&list, CopyString(arg->arg[i]));

	LoadArg(&list, in, -1);

	if	(cmd->narg > 0)
	{
		if	((i = SyncArg(&list, cmd->narg - 1)) != 0)
			InputError(in, 25, cmd->name, i);
	}

	str = GetBlock(in, cmdname, cb->fmt);

	if	(cmd->narg == 0)
	{
		InputError(in, 24, cmdname);
		FreeData("STR", str);
	}
	else	AddArg(&list, str);

	if	(MergeMode || cmd->type == TM_CONTROL)
		cmd->eval(cmd, in, out, &list);

	ClearArgList(&list);
}

void ce_comment(Command_t *cmd, Input_t *in, Output_t *out, ArgList_t *arg)
{
	int save = MergeMode;
	MergeMode = 0;
	out = OpenDummy();
	CopyBlock(in, out, arg->dim ? arg->arg[0] : NULL, cmd->fmt, NULL);
	CloseOutput(out);
	MergeMode = save;
}

#define	NUM(arg)	(arg ? atoi(arg) : 0)

static int test_arg(ArgList_t *arg)
{
	if	(arg->opt && arg->opt[0] == 'n')
	{
		return NUM(arg->arg[0]) == NUM(arg->arg[1]); 
	}

	if	(arg->arg[0] == NULL)
	{
		return (arg->arg[1] == NULL) ? 1 : 0;
	}
	else if	(arg->arg[1] == NULL)	return 0;

	return (strcmp(arg->arg[0], arg->arg[1]) == 0); 
}

void ce_if(Command_t *cmd, Input_t *in, Output_t *out, ArgList_t *arg)
{
	int save = MergeMode;
	int elsesave = ElseMergeMode;

	if	(MergeMode)
	{
		MergeMode = test_arg(arg);
		ElseMergeMode = !MergeMode;
	}
	else	ElseMergeMode = 0;

	CopyBlock(in, out, NULL, cmd->fmt, ELSEKEY);
	MergeMode = save;
	ElseMergeMode = elsesave;
}
