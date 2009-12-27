/*	Mischbefehl definieren
	(c) 1997 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#include "eisdoc.h"


static CmdType_t stdtype[] = {
	{ "var", memfree, ShowMacro, ce_var },
	{ "cmd", memfree, ShowMacro, ce_expr },
	{ "macro", memfree, ShowMacro, ce_macro },
};


static CmdType_t *get_type(const char *name)
{
	int i;
	
	for (i = 0; i < tabsize(stdtype); i++)
	{
		if	(mstrcmp(stdtype[i].name, name) == 0)
			return stdtype + i;
	}

	return stdtype;
}

void SetMacro (Cmd_t *cmd, const char *type, int narg, const char *fmt)
{
	if	(cmd)
	{
		memfree(cmd->desc);

		if	(cmd->type && cmd->type->clean)
			cmd->type->clean(cmd->par);

		cmd->type = get_type(type);
		cmd->narg = narg;
		cmd->par = mstrcpy(fmt);
		cmd->desc = MergeComment();
	}
}


void ce_def(Cmd_t *cmd, ArgList_t *arg)
{
	cmd = AddMergeCmd(arg->arg[0]);

	if	(cmd)
	{
		int narg = arg->arg[1] ? atoi(arg->arg[1]) : 0;
		SetMacro(cmd, arg->opt, narg, arg->arg[2]);
	}
}


void ce_undef(Cmd_t *cmd, ArgList_t *arg)
{
	DelMergeCmd(arg->arg[0]);
}
