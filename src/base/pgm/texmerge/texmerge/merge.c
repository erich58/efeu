/*	Einmischen von Daten in TeX-Datenfiles
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#include "texmerge.h"


void MergeCommand(Input_t *in, Output_t *out, const char *name)
{
	Command_t *cmd;
	ArgList_t list;

	if	(name == NULL)	return;

	if	((cmd = SearchCommand(name)) == NULL)
	{
		InputError(in, 23, name);
		return;
	}

	InitArgList(&list);

	if	(LoadArg(&list, in, cmd->narg) == 0)
		SkipSpace(in);

	SyncArg(&list, cmd->narg);

	if	(MergeMode || cmd->type == TM_CONTROL)
		cmd->eval(cmd, in, out, &list);

	ClearArgList(&list);
}

void MergeBuf(Input_t *in, Buffer_t *buf)
{
	Output_t *out;
	char *name;

	out = CreateOutput(NULL, NULL, buf);
	name = GetCommand(in, out);
	MergeCommand(in, out, name);
	FreeData("STR", name);
	CloseOutput(out);
}


void Merge (Input_t *in, Output_t *out)
{
	char *cmd;
	int c;

	while ((c = in_getc(in)) != EOF)
	{
		if	(c == '@')
		{
			cmd = GetCommand(in, out);
			MergeCommand(in, out, cmd);
			FreeData("STR", cmd);
		}
		else	out_putc(c, out);
	}
}

void TexMerge (const char *name, const char *oname)
{
	Input_t *in;
	Output_t *out;

	in = OpenTemplate(name);
	out = MakeDepend ? OpenDummy() : OpenTexfile(oname);

#if	UNIX_VERSION
	AddDepend(in->name);
#else
	if	(MakeDepend && in->name)
		printf(" %s", in->name);
#endif

	if	(VerboseMode)
	{
		fprintf(stderr, "%s -> %s\n",
			in->name ? in->name : "NULL",
			out->name ? out->name : "NULL");
	}

	Merge(in, out);
	CloseInput(in);
	CloseOutput(out);
}

int LongArg (const char *fmt)
{
	if	(fmt == NULL || *fmt == 0)	return 0;

	while (fmt[1] != 0)
		fmt++;

	return (*fmt == '\n') ? 1 : 0;
}

void MergeMacro (const char *cmd, Output_t *output)
{
	Input_t *in;

	in = CreateInput(NULL, NULL, cmd);

	if	(LongArg(cmd))
	{
		out_newline(output);
		Merge(in, output);
		out_newline(output);
	}
	else	Merge(in, output);

	CloseInput(in);
}

#if	UNIX_VERSION

static io_t *current_cout = NULL;

Obj_t *Parse_cout (io_t *io, void *data)
{
	return io2Obj(io_refer(current_cout));
}

#endif

void MergeEval(const char *cmd, Output_t *output)
{
#if	UNIX_VERSION
	io_t *in;
	io_t *save;

	in = io_cmdpreproc(io_cstr(cmd));
	save = current_cout;
	current_cout = io_Output(output);

	if	(LongArg(cmd))
	{
		out_newline(output);
		CmdEval(in, current_cout);
		out_newline(output);
	}
	else	CmdEval(in, current_cout);

	io_close(current_cout);
	current_cout = save;
	io_close(in);
#else
	if	(!LongArg(cmd))
	{
		out_fputs("{\\tt ", output, NULL);
		out_fputs(cmd, output, "t");
		out_fputs("}", output, NULL);
	}
	else	out_fputs(cmd, output, "v");
#endif
}
