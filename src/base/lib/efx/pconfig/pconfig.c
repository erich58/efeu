/*	Programmkonfiguration
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/ftools.h>
#include <EFEU/pconfig.h>
#include <EFEU/parsedef.h>

static char *Ident = "EFEU-Anwendungsprogramm";
static char *Version = "0.4 (Beta)";
static char *Copyright = "(c) 1999 Erich Frühstück";
static char *Welcome = "$! - Version $(Version:%s)\n$(Copyright:%s)\n";

static Var_t vardef[] = {
	{ "Ident",	&Type_str, &Ident },
	{ "Version",	&Type_str, &Version },
	{ "Copyright",	&Type_str, &Copyright },
	{ "Welcome",	&Type_str, &Welcome },
};


static Obj_t *p_msgtab (io_t *io, void *data)
{
	if	(io_eat(io, "%s") == '{')
	{
		io_getc(io);
		io_loadmsg(io, NULL, "}");
	}

	return NULL;
}

static Obj_t *p_pardef (io_t *io, void *data)
{
	pardef_t *pdef;
	ObjList_t *list;
	vecbuf_t *vb;
	char *name;

	if	(io_eat(io, " \t") == '(')
	{
		io_getc(io);
		list = Parse_list(io, ')');
	}
	else	return NULL;

	switch ((int) (size_t) data)
	{
	case P_ENV:	vb = &PgmDefTab.env; break;
	case P_OPT:	vb = &PgmDefTab.opt; break;
	case P_XOPT:	vb = &PgmDefTab.xopt; break;
	case P_XARG:	vb = &PgmDefTab.xarg; break;
	default:	vb = &PgmDefTab.arg; break;
	}

	pdef = vb_next(vb);
	pdef->flag = (int) (size_t) data;
	pdef->name = Obj2str(ReduceObjList(&list));
	pdef->cmd = Obj2str(ReduceObjList(&list));
	pdef->desc = Obj2str(ReduceObjList(&list));
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
	{ "msgtab", p_msgtab, NULL },
	{ "PgmEnv", p_pardef, (void *) P_ENV },
	{ "PgmOpt", p_pardef, (void *) P_OPT },
	{ "PgmXOpt", p_pardef, (void *) P_XOPT },
	{ "PgmArg", p_pardef, (void *) P_ARG },
	{ "PgmXArg", p_pardef, (void *) P_XARG },
	{ "PgmOptArg", p_pardef, (void *) P_OPTARG },
	{ "PgmVaArg", p_pardef, (void *) P_BREAK },
};

static void *parse_tab = NULL;

void pconfig_init(void)
{
	if	(parse_tab == NULL)
	{
		AddVar(NULL, vardef, tabsize(vardef));
		PushParseTab(NULL);
		AddParseDef(pdef, tabsize(pdef));
		parse_tab = PopParseTab();
	}

	PushParseTab(parse_tab);
}

void pconfig_exit(void)
{
	PopParseTab();
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
	info->label = mstrcpy(Ident);
	info->func = NULL;
	info->par = mstrcpy(Welcome);
	pconfig_exit();
}
