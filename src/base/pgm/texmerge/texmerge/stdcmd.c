/*	Standardmischbefehle
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#include "texmerge.h"
#include <memory.h>

static ArgList_t cur_arg = { 0, 0, NULL, NULL };

static char *getarg(int n, char *def)
{
	if	(n < 0 || n >= cur_arg.dim)
		return NULL;

	return cur_arg.arg[n] ? cur_arg.arg[n] : def;
}

#if	UNIX_VERSION

Obj_t *Parse_narg (io_t *io, void *data)
{
	return int2Obj(cur_arg.dim);
}

Obj_t *Parse_opt (io_t *io, void *data)
{
	return PFunc_str(io, cur_arg.opt);
}

static Obj_t *get_arg(void *par, const ObjList_t *list)
{
	int n = Obj2int(RefObj(list->obj));
	return str2Obj(mstrcpy(getarg(n, NULL)));
}

Obj_t *Parse_arg (io_t *io, void *data)
{
	int c;
	Obj_t *obj;

	c = io_eat(io, " \t");

	if	(c == '[')
	{
		io_getc(io);
		obj = Parse_index(io);
	}
	else	obj = int2Obj(0);

	return Obj_call(get_arg, data, NewObjList(obj));
}

#endif


void ce_opt(Command_t *cmd, Input_t *in, Output_t *out, ArgList_t *arg)
{
	out_fputs(cur_arg.opt ? cur_arg.opt : arg->arg[0], out, arg->opt);
}

void ce_arg(Command_t *cmd, Input_t *in, Output_t *out, ArgList_t *arg)
{
	int n = arg->arg[0] ? atoi(arg->arg[0]) : 0;
	out_fputs(getarg(n, arg->arg[1]), out, arg->opt);
}

void ce_macro(Command_t *cmd, Input_t *in, Output_t *out, ArgList_t *arg)
{
	ArgList_t save_arg;

	save_arg = cur_arg;
	cur_arg = *arg;
	MergeMacro(cmd->fmt, out);
	cur_arg = save_arg;
}

void ce_expr(Command_t *cmd, Input_t *in, Output_t *out, ArgList_t *arg)
{
	ArgList_t save_arg;

	save_arg = cur_arg;
	cur_arg = *arg;
	MergeEval(cmd->fmt, out);
	cur_arg = save_arg;
}

void ce_var(Command_t *cmd, Input_t *in, Output_t *out, ArgList_t *arg)
{
	out_fputs(cmd->fmt, out, arg->opt);
}


void ce_show(Command_t *cmd, Input_t *in, Output_t *out, ArgList_t *arg)
{
	int i, save, flag;

	flag = (arg->dim > 0 && LongArg(arg->arg[arg->dim - 1]));
	save = set_mode(out, 0);

	if	(flag)
	{
		out_puts("\\begin{verbatim}\n@begin{", out);
	}
	else
	{
		out_puts("{\\tt ", out);
		set_mode(out, OUT_TEXMODE|OUT_EXPAND);
		out_putc('@', out);
	}

	out_puts(cmd->name, out);

	if	(flag)	out_putc('}', out);

	if	(arg->opt)
	{
		out_putc('[', out);
		out_puts(arg->opt, out);
		out_putc(']', out);
	}

	for (i = 0; i < arg->dim - flag; i++)
	{
		out_putc('{', out);
		out_puts(arg->arg[i], out);
		out_putc('}', out);
	}

	if	(flag)
	{
		out_newline(out);
		out_puts(arg->arg[arg->dim - 1], out);
		out_newline(out);
		out_puts("@end{", out);
		out_puts(cmd->name, out);
		out_puts("}\n\\end{verbatim}\n", out);
	}
	else
	{
		set_mode(out, 0);
		out_putc('}', out);
	}

	set_mode(out, save);
}
