/*	Testblock
	(c) 1998 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5

	Version 1.0
*/

#include "eisdoc.h"
#include <ctype.h>

static int test_depth = 0;
static io_t *else_out = NULL;


static int test_elif(void)
{
	char *p;

	if	(ParseArg(&p, '{', '}'))
	{
		int rval = Obj2bool(strterm(p));
		memfree(p);
		return rval;
	}
	else	return 0;
}


static void test_eval (const char *name)
{
	if	(mstrcmp("endif", name) == 0)
	{
		if	(test_depth <= 0)
		{
			MergeInput(NULL);
			return;
		}

		test_depth--;
	}
	else if	(mstrcmp("if", name) == 0)
	{
		test_depth++;
	}
	else if	(mstrcmp("ifdef", name) == 0)
	{
		test_depth++;
	}
	else if	(mstrcmp("ifndef", name) == 0)
	{
		test_depth++;
	}
	else if	(test_depth == 0)
	{
		if	(mstrcmp("elif", name) == 0)
		{
			if	(else_out && test_elif())
			{
				MergeOutput(else_out);
				else_out = NULL;
			}
			else	MergeOutput(NULL);

			return;
		}
		else if	(mstrcmp("else", name) == 0)
		{
			MergeOutput(else_out);
			else_out = NULL;
			return;
		}
	}

	MergePutCmd(name);
}

void ce_if (Cmd_t *cmd, ArgList_t *arg)
{
	Merge_t save;
	strbuf_t *sb;

/*	Mischstatus zwischenspeichern
*/
	PushMerge(&save);
	MergeFilter(NULL);

/*	Testblock zwischenspeichern
*/
	sb = new_strbuf(0);
	else_out = io_strbuf(sb);

	if	(Obj2bool(strterm(arg->arg[0])))
	{
		MergeOutput(else_out);
		else_out = NULL;
	}

	MergeStat.eval = test_eval;
	MergeEval(EOF, EOF);
	rd_deref(else_out);

/*	Testblock ausführen
*/
	sb_begin(sb);
	MergeInput(io_strbuf(sb));
	MergeOutput(rd_refer(save.out));
	MergeEval(EOF, EOF);
	
/*	Aufräumen und Mischstatus wiederherstellen
*/
	del_strbuf(sb);
	PopMerge(&save);
}
