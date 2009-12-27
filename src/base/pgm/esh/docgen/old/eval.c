/*	Einmischen von Daten in TeX-Datenfiles
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#include "eisdoc.h"
#include <ctype.h>

static int merge_put (int c, void *ptr)
{
	if	(c == EOF)	return 0;

	MergePutc(c);
	return 1;
}

static int ctrl_put (int c, void *ptr)
{
	if	(c == EOF)	return 0;

	MergeCtrl(c);
	return 1;
}

static io_t merge_out = IODATA (NULL, merge_put, NULL, NULL);
static io_t merge_ctrl = IODATA (NULL, ctrl_put, NULL, NULL);
io_t *MergeOut = &merge_out;
io_t *MergeCtrlOut = &merge_ctrl;


Cmd_t *cur_cmd = NULL;

Obj_t *cpar_name(void *par, const ObjList_t *list)
{
	return str2Obj(mstrcpy(cur_cmd ? cur_cmd->name : NULL));
}

static void merge_cmd (const char *name)
{
	Cmd_t *cmd;
	Cmd_t *save_cmd;
	ArgList_t list;
	int c;

	if	(name == NULL)	return;

	if	(MergeStat.eval)
	{
		MergeStat.eval(name);
		return;
	}

	if	((cmd = GetMergeCmd(name)) == NULL)
	{
		/*
		char *p = msprintf("%#s", name);
		io_error(MergeStat.in, NULL, 23, 1, p);
		memfree(p);
		*/
		MergePutCmd(name);
		return;
	}

	do	c = io_getc(MergeStat.in);
	while	(c == ' ' || c == '\t');

	io_ungetc(c, MergeStat.in);
	InitArgList(&list);
	LoadArg(&list, cmd->narg);
	SyncArg(&list, cmd->narg);
	save_cmd = cur_cmd;
	cur_cmd = cmd;

	if	(cmd->type)
		cmd->type->eval(cmd, &list);

	cur_cmd = save_cmd;
	ClearArgList(&list);
}

int EvalMergeCmd(int c)
{
	char *name;

	if	(c != MergeStat.cmdkey)
		return 0;

	if	((name = ParseName()) == NULL)
	{
		int c = io_getc(MergeStat.in);

		if	(c == MergeStat.cmdkey)
		{
			MergePutc(c);
			return 1;
		}

		io_ungetc(c, MergeStat.in);
		return 0;
	}

	merge_cmd(name);
	memfree(name);
	return 1;
}

void MergeEval (int beg, int end)
{
	int c, depth, flag;
	char *p;

	p = NULL;
	flag = MergeStat.linestart && MergeStat.ignorespace;
	depth = 0;

	while ((c = io_getc(MergeStat.in)) != EOF)
	{
		if	(EvalMergeCmd(c))
		{
			MergeStat.linestart = 0;
			flag = 0;
			continue;
		}
		else if	(SkipComment(c))
		{
			flag = MergeStat.linestart && MergeStat.ignorespace;
			continue;
		}
		else if	(end == 0 && isspace(c))
		{
			break;
		}
		else if	(c == end)
		{
			if	(depth > 0)	depth--;
			else			break;
		}
		else if	(c == beg)
		{
			if	(p == NULL)	p = io_ident(MergeStat.in);

			depth++;
		}
		else if	(flag && (c == ' ' || c == '\t'))
		{
			continue;
		}

		if	(MergeStat.buf && !isspace(c))
		{
			del_strbuf(MergeStat.buf);
			MergeStat.buf = NULL;
		}

		if	(c == '\n')
		{
			MergeNewline();

			if	(MergeStat.linestart)
				MergeCtrl('\n');

			MergeStat.linestart = 1;
			flag = MergeStat.ignorespace;
		}
		else
		{
			MergePutc(c);
			MergeStat.linestart = 0;
			flag = 0;
		}
	}

	if	(depth > 0)
	{
		reg_set(0, io_ident(MergeStat.in));
		reg_set(1, p);
		reg_fmt(2, "%c ... %c", beg, end);
		errmsg(NULL, 42);
	}
	else	memfree(p);
}


int LongArg (const char *fmt)
{
	if	(fmt == NULL || *fmt == 0)	return 0;

	while (fmt[1] != 0)
		fmt++;

	return (*fmt == '\n') ? 1 : 0;
}
