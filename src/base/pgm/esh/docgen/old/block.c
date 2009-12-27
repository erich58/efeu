/*	Blockstrucktur
	(c) 1997 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#include "eisdoc.h"
#include <ctype.h>

static int block_count = 0;
static char *block_name = NULL;

static void test_name (const char *name)
{
	char *p;

	if	(!ParseArg(&p, '{', '}'))
	{
		reg_set(0, io_ident(MergeStat.in));
		reg_cpy(1, name);
		errmsg(NULL, 44);
	}
	else if	(mstrcmp(p, name) != 0)
	{
		reg_set(0, io_ident(MergeStat.in));
		reg_cpy(1, name);
		reg_set(2, p);
		errmsg(NULL, 45);
	}
	else	memfree(p);
}

static void block_eval (const char *name)
{
	if	(mstrcmp("end", name) == 0)
	{
		if	(block_count <= 0)
		{
			if	(block_name)
				test_name(block_name);

			MergeInput(NULL);
			return;
		}

		block_count--;
	}
	else if	(mstrcmp("begin", name) == 0)
	{
		block_count++;
	}

	MergePutCmd(name);
}


void ce_begin(Cmd_t *cmd, ArgList_t *arg)
{
	Merge_t save;
	strbuf_t *sb;
	strbuf_t *sb2;
	char *fpos;
	int i;

	PushMerge(&save);
	sb = new_strbuf(0);
	MergeOutput(io_strbuf(sb));
	MergeFilter(NULL);
	MergeStat.eval = block_eval;
	block_count = 0;
	block_name = arg->dim > 0 ? arg->arg[0] : NULL;
	fpos = io_ident(MergeStat.in);
	MergeEval(EOF, EOF);

	if	(block_count > 0)
	{
		reg_set(0, io_ident(MergeStat.in));
		reg_set(1, fpos);
		errmsg(NULL, 43);
	}

	memfree(fpos);
	sb_begin(sb);
	MergeStat.eval = save.eval;
	MergeInput(io_strbuf(sb));

	cmd = block_name ? GetMergeCmd(block_name) : NULL;

	if	(cmd)
	{
		sb2 = new_strbuf(0);
		MergeOutput(io_strbuf(sb2));
	}

	MergeOutput(rd_refer(save.out));

	PushCmdTab(NULL);
	MergeEval(EOF, EOF);
	rd_deref(PopCmdTab());

	PopMerge(&save);
	del_strbuf(sb);

	if	(!cmd)	return;

	memfree(block_name);

	for (i = 1; i < arg->dim; i++)
		arg->arg[i - 1] = arg->arg[i];

	arg->arg[arg->dim - 1] = sb2str(sb2);
}
