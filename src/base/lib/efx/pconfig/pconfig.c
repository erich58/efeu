/*
Programmkonfiguration

$Copyright (C) 1994 Erich Frühstück
This file is part of EFEU.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; see the file COPYING.Library.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <EFEU/ftools.h>
#include <EFEU/pconfig.h>
#include <EFEU/parsedef.h>
#include <EFEU/CmdPar.h>
#include <EFEU/Resource.h>

#define	TYPEMASK	0x0f
#define	KEYFLAG		0x10
#define	WELCOME		"$! - Version $(Version:%s)\n$(Copyright:%s)\n"

deftab_t PgmDefTab = {
	NULL,
	VB_DATA(8, sizeof(pardef_t)),
	VB_DATA(8, sizeof(pardef_t)),
	VB_DATA(8, sizeof(pardef_t)),
	VB_DATA(8, sizeof(pardef_t)),
	VB_DATA(8, sizeof(pardef_t)),
};

static VarDef_t vardef[] = {
	{ "HelpFmt",	&Type_str, &HelpFmt },
	{ "UsageFmt",	&Type_str, &UsageFmt },
};



static Obj_t *p_pardef (io_t *io, void *data)
{
	pardef_t *pdef;
	ObjList_t *list;
	vecbuf_t *vb;
	char *desc;
	char *name;
	int type;

	if	(io_eat(io, " \t") == '(')
	{
		io_getc(io);
		list = Parse_list(io, ')');
	}
	else	return NULL;

	type = (size_t) data;

	switch (type & TYPEMASK)
	{
	case P_ENV:	vb = &PgmDefTab.env; break;
	case P_OPT:	vb = &PgmDefTab.opt; break;
	case P_XOPT:	vb = &PgmDefTab.xopt; break;
	case P_XARG:	vb = &PgmDefTab.xarg; break;
	default:	vb = &PgmDefTab.arg; break;
	}

	pdef = vb_next(vb);
	pdef->flag = (type & TYPEMASK);

	pdef->key = (type & KEYFLAG) ? Obj2str(ReduceObjList(&list)) : NULL;
	pdef->name = Obj2str(ReduceObjList(&list));
	pdef->cmd = Obj2str(ReduceObjList(&list));
	desc = Obj2str(ReduceObjList(&list));
	pdef->desc = mlangcpy(desc, NULL);
	memfree(desc);
	DelObjList(list);

	if	(pdef->flag == P_ENV)
	{
		if	((name = getenv(pdef->name)) != NULL)
		{
			reg_cpy(1, name);
			streval(pdef->cmd);
		}
	}

	return NULL;
}


static ParseDef_t pdef[] = {
	{ "PgmEnv", p_pardef, (void *) P_ENV },
	{ "PgmOpt", p_pardef, (void *) P_OPT },
	{ "PgmArg", p_pardef, (void *) P_ARG },
	{ "PgmRegExp", p_pardef, (void *) (P_REGEX|KEYFLAG) },
	{ "PgmOptArg", p_pardef, (void *) P_OPTARG },
	{ "PgmVaArg", p_pardef, (void *) P_BREAK },
};

static void f_usage(Func_t *func, void *rval, void **arg)
{
	io_t *out;

	out = io_fileopen(*((char **) arg[1]), "w");
	cp_usage(Val_io(arg[0]), out);
	io_close(out);
}


static void f_iousage(Func_t *func, void *rval, void **arg)
{
	cp_usage(Val_io(arg[0]), Val_io(arg[1]));
}


static FuncDef_t fdef[] = {
	{ FUNC_VIRTUAL, &Type_void, "usage (IO def, str name)", f_usage },
	{ FUNC_VIRTUAL, &Type_void,  "usage (IO def, IO out)", f_iousage },
};

static void *parse_tab = NULL;

static int pconfig_stat = 0;

void pconfig_init(void)
{
	if	(pconfig_stat)	return;

	if	(parse_tab == NULL)
	{
		AddFuncDef(fdef, tabsize(fdef));
		AddVarDef(NULL, vardef, tabsize(vardef));
		PushParseTab(NULL);
		AddParseDef(pdef, tabsize(pdef));
		parse_tab = PopParseTab();
	}

	PushParseTab(parse_tab);
	pconfig_stat = 1;
}

void pconfig_exit(void)
{
	if	(pconfig_stat)
	{
		PopParseTab();
		pconfig_stat = 0;
	}
}

void pconfig (const char *name, Var_t *var, size_t dim)
{
	InfoNode_t *info;

	pconfig_init();
	AddVar(LocalVar, var, dim);

	if	(name == NULL)	name = ProgName;

	if	(!applfile(name, APPL_APP))
	{
		reg_cpy(1, name);
		liberror(MSG_EFMAIN, 6);
	}

	info = GetInfo(NULL, NULL);
	info->label = mstrcpy(GetResource("Ident", name));
	info->func = NULL;
	info->par = mstrcpy(WELCOME);
	pconfig_exit();
}
