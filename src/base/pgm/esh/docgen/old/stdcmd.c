/*	Standardmischbefehle
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#include "eisdoc.h"
#include <memory.h>

static ArgList_t cur_arg = { 0, 0, NULL, NULL };

static char *getarg(int n, char *def)
{
	if	(n < 0 || n >= cur_arg.dim)
		return NULL;

	return cur_arg.arg[n] ? cur_arg.arg[n] : def;
}

/*	Schnittstellenfunktionen zum Interpreter
*/

Obj_t *cpar_narg(void *par, const ObjList_t *list)
{
	return int2Obj(cur_arg.dim);
}

Obj_t *cpar_opt(void *par, const ObjList_t *list)
{
	return str2Obj(mstrcpy(cur_arg.opt));
}

Obj_t *cpar_arg(void *par, const ObjList_t *list)
{
	Buf_vec.type = &Type_str;
	Buf_vec.dim = cur_arg.dim;
	Buf_vec.data = cur_arg.arg;
	return NewObj(&Type_vec, &Buf_vec);
}


/*	Mischbefehle
*/

void ce_opt(Cmd_t *cmd, ArgList_t *arg)
{
/*
//	FormatString(cur_arg.opt ? cur_arg.opt : arg->arg[0], MergeOut, arg->opt);
*/
	MergePuts(cur_arg.opt);
}

void ce_arg(Cmd_t *cmd, ArgList_t *arg)
{
	int n = arg->arg[0] ? atoi(arg->arg[0]) : 0;
/*
//	FormatString(getarg(n, arg->arg[1]), MergeOut, arg->opt);
*/
	MergePuts(getarg(n, arg->arg[1]));
}

void ce_eval(Cmd_t *cmd, ArgList_t *arg)
{
	ArgList_t save_arg;
	save_arg = cur_arg;
	cur_arg = *arg;
	CmdEval(MergeStat.in, MergeOut);
	cur_arg = save_arg;
}


void ce_macro(Cmd_t *cmd, ArgList_t *arg)
{
	Merge_t save;
	ArgList_t save_arg;

/*
	if	(LongArg(cmd->par))
		MergeNewline();
*/
		
	save_arg = cur_arg;
	cur_arg = *arg;
	PushMerge(&save);
	MergeInput(io_lnum(io_cstr(cmd->par)));
	MergeFilter(NULL);
	MergeEval(EOF, EOF);
	PopMerge(&save);
	cur_arg = save_arg;
/*
	if	(LongArg(cmd->par))
		MergeNewline();
*/
}

void ce_expr(Cmd_t *cmd, ArgList_t *arg)
{
	ArgList_t save_arg;
	io_t *in;
	
	save_arg = cur_arg;
	cur_arg = *arg;
	in = io_cmdpreproc(io_cstr(cmd->par));

/*
	if	(LongArg(cmd->par))
	{
		MergeNewline();
		CmdEval(in, MergeOut);
		MergeNewline();
	}
	else	CmdEval(in, MergeOut);
*/
	CmdEval(in, MergeOut);

	io_close(in);
	cur_arg = save_arg;
}

void ce_var(Cmd_t *cmd, ArgList_t *arg)
{
/*
//	FormatString(cmd->par, MergeOut, arg->opt);
*/
	MergePuts(cmd->par);
}
