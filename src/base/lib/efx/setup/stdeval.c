/*
Initialisierung des Befehlsinterpreters

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

#include <EFEU/object.h>
#include <EFEU/cmdsetup.h>
#include <EFEU/parsedef.h>
#include <EFEU/cmdeval.h>
#include <EFEU/ioctrl.h>
#include <EFEU/CmdPar.h>
#include <EFEU/pconfig.h>
#include <EFEU/MakeDepend.h>
#include <EFEU/LangType.h>
#include <EFEU/printobj.h>
#include <ctype.h>

#define LBL_VAR	":*:global variables" \
		":de:Globale Variablen"

#define LBL_FUNC ":*:global functions" \
		":de:Globale Funktionen"

static void var_desc (io_t *io, Var_t *var)
{
	io = io_lmark(io_refer(io), "\t", NULL, 0);
	io_puts(var->desc, io);
	io_close(io);
}

static void var_info (io_t *io, InfoNode_t *info)
{
	VarTab_t *tab = info->par;
	int i;

	for (i = 0; i < tab->tab.dim; i++)
	{
		Var_t *var = tab->tab.tab[i];

		if	(var->type == &Type_vfunc)	continue;
		if	(var->type == &Type_func)	continue;
		if	(var->type == &Type_vtab)	continue;

		io_printf(io, "\n%s %s\n", var->type->name, var->name);
		var_desc(io, var);
	}
}

static void func_info (io_t *io, InfoNode_t *info)
{
	VarTab_t *tab = info->par;
	int i;

	for (i = 0; i < tab->tab.dim; i++)
	{
		Var_t *var = tab->tab.tab[i];

		if	(var->type == &Type_vfunc)
		{
			io_printf(io, "%s\n", var->name);
			PrintData(io, var->type, var->data);
			io_printf(io, "%s\n", var->desc);
		}
		else if	(var->type == &Type_func)
		{
			io_printf(io, "%s\n", var->name);
			PrintData(io, var->type, var->data);
			io_printf(io, "%s\n", var->desc);
		}
	}
}

/*	Schlüsselwörter
*/

static Obj_t *pf_operator (io_t *io, void *data)
{
	void *p;

	if	(io_eat(io, " \t") == '"')
	{
		io_scan(io, SCAN_STR, &p);
	}
	else	p = io_mgets(io, "%s");

	return NewPtrObj(&Type_name, p);
}

static Obj_t *pf_func (io_t *io, void *data)
{
	return Parse_func(io, NULL, NULL, (int) (size_t) data);
}

static Obj_t *pf_expression (io_t *io, void *data)
{
	return expr2Obj(Parse_cmd(io));
}

static Obj_t *pf_string (io_t *io, void *data)
{
	return str2Obj(getstring(io));
}

static Obj_t *pf_msgtab (io_t *io, void *data)
{
	if	(io_eat(io, "%s") == '{')
	{
		io_getc(io);
		io_loadmsg(io, NULL, "}");
	}

	return NULL;
}


static Obj_t *do_psub(void *par, const ObjList_t *list)
{
	return str2Obj(parsub(Val_str(list->obj->data)));
}

static Obj_t *pf_parsub (io_t *io, void *data)
{
	return Obj_call(do_psub, NULL, NewObjList(str2Obj(getstring(io))));
}

static Obj_t *pf_const (io_t *io, void *data)
{
	return EvalObj(Parse_term(io, 0), NULL);
}

static Obj_t *pf_static (io_t *io, void *data)
{
	UnrefEval(Parse_term(io, 0)); return Obj_noop();
}

static Obj_t *p_interact (io_t *io, void *data)
{
	char *prompt;
	char *hist;
	io_t *in;

	prompt = Obj2str(Parse_term(io, 0));
	hist = Obj2str(Parse_term(io, 0));
	in = io_interact(prompt, hist);
	CmdEval(in, iomsg);
	io_close(in);
	memfree(prompt);
	memfree(hist);
	return NULL;
}

static Obj_t *p_local (io_t *io, void *data)
{
	return NewPtrObj(&Type_vtab, rd_refer(LocalVar));
}


static ParseDef_t pdef[] = {
	{ "for", PFunc_for, NULL },
	{ "while", PFunc_while, NULL },
	{ "do", PFunc_do, NULL },
	{ "if", PFunc_if, NULL },
	{ "switch", PFunc_switch, NULL },
	{ "case", PFunc_case, NULL },
	{ "default", PFunc_default, NULL },

	{ "operator", pf_operator, NULL },
	{ "function", pf_func, NULL },
	{ "virtual", pf_func, (void *) FUNC_VIRTUAL },
	{ "promotion", pf_func, (void *) FUNC_PROMOTION },
	{ "restricted", pf_func, (void *) FUNC_RESTRICTED },
	{ "inline", pf_func, (void *) FUNC_INLINE },

	{ "return", PFunc_return, NULL },
	{ "break", PFunc_break, (void *) 0 },
	{ "continue", PFunc_break, (void *) 1 },

	{ "expression", pf_expression, NULL },
	{ "string", pf_string, NULL },
	{ "msgtab", pf_msgtab, NULL },
	{ "parsub", pf_parsub, NULL },
	{ "static", pf_static, NULL },
	{ "const", pf_const, NULL },

	{ "struct", PFunc_struct, NULL },
	{ "typedef", PFunc_typedef, NULL },

	{ "interactive", p_interact, NULL },
	{ "local", p_local, NULL },
};


/*	Komponenten von Listen
*/

static Obj_t *list_obj (const Var_t *st, const Obj_t *obj)
{
	register ObjList_t *list = Val_list(obj->data);
	return list ? RefObj(list->obj) : ptr2Obj(NULL);
}

static Obj_t *list_next (const Var_t *st, const Obj_t *obj)
{
	register ObjList_t *list = Val_list(obj->data);

	list = list ? RefObjList(list->next) : NULL;
	return NewPtrObj(&Type_list, list);
}


static MemberDef_t list_member[] = {
	{ "next", &Type_list, list_next, NULL },
	{ "obj", NULL, list_obj, NULL },
};

/*	Komponenten von Typen
*/

static Obj_t *type_default (const Var_t *st, const Obj_t *obj)
{
	Type_t *type = Val_type(obj->data);

	if	(type == NULL)	return NULL;
	else if	(type->defval)	return LvalObj(&Lval_ptr, type, type->defval);
	else			return NewObj(type, NULL);
}

static MemberDef_t type_member[] = {
	{ "default", NULL, type_default, NULL },
};

/*	Komponenten von Referenzen
*/

static Obj_t *ref_refcount (const Var_t *st, const Obj_t *obj)
{
	refdata_t *rd = Val_ptr(obj->data);
	return int2Obj(rd ? rd->refcount : 0);
}

static Obj_t *ref_ident (const Var_t *st, const Obj_t *obj)
{
	return str2Obj(rd_ident(Val_ptr(obj->data)));
}

static MemberDef_t ref_member[] = {
	{ "ident", &Type_str, ref_ident, NULL },
	{ "refcount", &Type_int, ref_refcount, NULL },
};

/*	Parametersubstitution
*/

static int c_term(io_t *in, io_t *out, void *arg)
{
	return iocpy_term(in, out, '(', ")", 0);
}

static int c_eval(io_t *in, io_t *out, void *arg)
{
	return iocpy_eval(in, out, 0, "}", 0);
}

static int f_eval (CmdPar_t *cpar, CmdParVar_t *var,
	const char *par, const char *arg)
{
	char *p = CmdPar_psub(cpar, par, arg);
	streval(p);
	memfree(p);
	return 0;
}

static CmdParEval_t cpar_eval = { "eval", "Ausdruck auswerten", f_eval };


static void f_proto (CmdPar_t *par, io_t *io, const char *arg)
{
	char *ident;
	char *name;
	char *type;
	VirFunc_t *tab;
	Func_t **ftab;
	int i;

	if	(arg == NULL)	return;

	ident = NULL;
	name = (char *) arg;
	type = NULL;

	while (isspace(*name))
		name++;

	for (ident = name; *ident != 0; ident++)
	{
		if	(*ident == ':')
		{
			type = name;
			*ident = 0;
			ident++;

			if	(*ident == ':')	ident++;

			name = ident;
		}

		if	(isspace(*ident))
		{
			*ident = 0;
			ident++;
			break;
		}
	}

	if	(type)
	{
		tab = VirFunc(GetTypeFunc(GetType(type), name));
	}
	else	tab = VirFunc(GetGlobalFunc(name));

	if	(tab == NULL || tab->tab.used == 0)	return;

	ftab = tab->tab.data;

	for (i = 0; i < tab->tab.used; i++)
	{
		if	(i > 0)	io_puts("\\br\n", io);

		io_puts("<|", io);
		ListFunc(io, ftab[i]);
		io_puts("|>", io);
	}

	io_putc('\n', io);
}

static CmdParExpand_t cexp_proto = { "proto", "Prototype ausgeben", f_proto };

extern int float_align;

char *PS1 = "$!: ";
char *PS2 = "> ";
extern char *MessageHandler;

static VarDef_t vardef[] = {
	{ "MakeDepend",	&Type_bool, &MakeDepend,
		":*:flag for creating dependence lists\n"
		":de:Flag zur Generierung von Abhängigkeitslisten\n" },
	{ "ProgIdent",	&Type_str, &ProgIdent,
		":*:identification of command\n"
		":de:Identifikation des Kommandos\n" },
	{ "ProgName",	&Type_str, &ProgName,
		":*:call name of command\n"
		":de:Aufrufname des Kommandos\n" },
	{ "ApplPath",	&Type_str, &ApplPath,
		":*:search path for application files\n"
		":de:Suchpfad für Anwendungsdateien\n"},
	{ "InfoPath",	&Type_str, &InfoPath,
		":*:search path for info files\n"
		":de:Suchpfad für Informationsdateien\n"},
	{ "Shell",	&Type_str, &Shell,
		":*:shell for system calls\n"
		":de:Shell für Systemaufrufe\n" },
	{ "Pager",	&Type_str, &Pager,
		":*:default pager\n"
		":de:Standardpager\n" },
	{ "MessageHandler",	&Type_str, &MessageHandler,
		":*:default message haendler\n"
		":de:Standardhändler für Fehlermeldungen\n" },
	{ "PS1",	&Type_str, &PS1,
		":*:prompt 1\n"
		":de:Prompt 1\n" },
	{ "PS2",	&Type_str, &PS2,
		":*:prompt 2\n"
		":de:Prompt 2\n" },
	{ "float_align",	&Type_int, &float_align,
		":*:alignment of float exponent\n"
		":de:Ausrichtung des Gleitkommaexponenten\n" },
	{ "global",	&Type_vtab, &GlobalVar,
		":*:global variable tab\n"
		":de:Globale Variablentabelle\n" },
	{ "context",	&Type_vtab, &ContextVar,
		":*:context variable tab\n"
		":de:Kontextvariablentabelle\n" },
	{ "Lang", &Type_str, &LangType.language,
		":*:language code of LANG environment\n"
		":de:Sprachkode der Umgebungsvariable LANG\n" },
	{ "Territory", &Type_str, &LangType.territory,
		":*:territory code of LANG environment\n"
		":de:Regionalcode der Umgebungsvariable LANG\n" },
};

#if	0
static Var_t test_var[] = {
	{ "Test_str", &Type_str, NULL, 0, 0, ResourceVar, "XYZ" },
	{ "Test_int", &Type_int, NULL, 0, 0, ResourceVar, "XYZ" },
};
#endif

void SetupStd(void)
{
	static int init_done = 0;

	if	(init_done)	return;

	init_done = 1;

	GlobalVar = VarTab("global", 64);
	LocalVar = RefVarTab(GlobalVar);

	StdOpDef();

	AddParseDef(pdef, tabsize(pdef));
	CmdEval_setup();

	CmdSetup_const();
	CmdSetup_base();
	CmdSetup_int();
	CmdSetup_long();
	CmdSetup_uint();
	CmdSetup_size();
	CmdSetup_double();
	CmdSetup_str();
	CmdSetup_io();

	AddType(&Type_name);
	AddType(&Type_mname);
	AddType(&Type_sname);
	AddType(&Type_undef);
	AddType(&Type_vec);
	AddType(&Type_list);
	AddType(&Type_explist);
	AddType(&Type_vdef);

	CmdSetup_obj();
	CmdSetup_reg();
	CmdSetup_pctrl();
	CmdSetup_test();

	CmdSetup_konv();
	CmdSetup_print();
	CmdSetup_func();
	CmdSetup_op();
	CmdSetup_info();
	CmdSetup_strbuf();
	CmdSetup_date();
	CmdSetup_regex();
	CmdSetup_cmdpar();
	CmdSetup_unix();
	CmdSetup_dl();

	AddMember(Type_list.vtab, list_member, tabsize(list_member));
	AddMember(Type_type.vtab, type_member, tabsize(type_member));
	AddMember(Type_ref.vtab, ref_member, tabsize(ref_member));
	AddVarDef(NULL, vardef, tabsize(vardef));
#if	0
	AddVar(NULL, test_var, tabsize(test_var));
#endif

	psubfunc('(', c_term, ")");
	psubfunc('{', c_eval, "}");

	CmdParEval_add(&cpar_eval);
	CmdParExpand_add(&cexp_proto);
	AddInfo(NULL, "Var", LBL_VAR, var_info, RefVarTab(GlobalVar));
	AddInfo(NULL, "Func", LBL_FUNC, func_info, RefVarTab(GlobalVar));
}
