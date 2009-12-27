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
#include <EFEU/LangDef.h>
#include <EFEU/printobj.h>
#include <EFEU/Debug.h>
#include <EFEU/Op.h>
#include <EFEU/CmpDef.h>
#include <EFEU/TestDef.h>
#include <EFEU/tms.h>
#include <ctype.h>

#define	KEY_VAR	"Var"
#define LBL_VAR	":*:global variables" \
		":de:Globale Variablen"

#define	KEY_FUNC "Func"
#define LBL_FUNC ":*:global functions" \
		":de:Globale Funktionen"

static void var_desc (IO *io, VarTabEntry *var)
{
	io = io_lmark(io_refer(io), "\t", NULL, 0);
	io_puts(var->desc, io);
	io_close(io);
}

static void var_info (IO *io, InfoNode *info)
{
	EfiVarTab *tab = info->par;
	VarTabEntry *p = tab->tab.data;
	size_t n = tab->tab.used;

	for (; n-- > 0; p++)
	{
		if	(p->type == &Type_vfunc)	continue;
		if	(p->type == &Type_func)	continue;
		if	(p->type == &Type_vtab)	continue;

		io_printf(io, "\n%s %s\n", p->type->name, p->name);
		var_desc(io, p);
	}
}

static void func_info (IO *io, InfoNode *info)
{
	EfiVarTab *tab = info->par;
	VarTabEntry *p = tab->tab.data;
	size_t n = tab->tab.used;

	for (; n-- > 0; p++)
	{
		if	(p->obj == NULL)	continue;

		if	(p->obj->type == &Type_vfunc)
		{
			io_printf(io, "%s\n", p->name);
			PrintData(io, p->obj->type, p->obj->data);
			io_printf(io, "%s\n", p->desc);
		}
		else if	(p->obj->type == &Type_func)
		{
			io_printf(io, "%s\n", p->name);
			PrintData(io, p->obj->type, p->obj->data);
			io_printf(io, "%s\n", p->desc);
		}
	}
}

/*	Schlüsselwörter
*/

static EfiObj *pf_operator (IO *io, void *data)
{
	void *p;

	if	(io_eat(io, " \t") == '"')
	{
		io_scan(io, SCAN_STR, &p);
	}
	else	p = io_mgets(io, "%s");

	return NewPtrObj(&Type_name, p);
}

static EfiObj *pf_func (IO *io, void *data)
{
	return Parse_func(io, NULL, NULL, (int) (size_t) data);
}

static EfiObj *pf_expression (IO *io, void *data)
{
	dbg_note(NULL, "WARNING: expression obsolete, use [...] instead.\n",
		NULL);
	return expr2Obj(Parse_cmd(io));
}

static EfiObj *pf_string (IO *io, void *data)
{
	return str2Obj(getstring(io));
}

static EfiObj *do_psub(void *par, const EfiObjList *list)
{
	return str2Obj(mpsubvec(Val_str(list->obj->data), 0, NULL));
}

static EfiObj *pf_parsub (IO *io, void *data)
{
	return Obj_call(do_psub, NULL, NewObjList(str2Obj(getstring(io))));
}

static EfiObj *pf_const (IO *io, void *data)
{
	/*
	return EvalObj(Parse_term(io, 0), NULL);
	*/
	return EvalObj(Parse_vdef(io, NULL, VDEF_REPEAT|VDEF_CONST), NULL);
}

static EfiObj *pf_static (IO *io, void *data)
{
	UnrefEval(Parse_term(io, 0)); return Obj_noop();
}

static EfiObj *pf_typeof (IO *io, void *data)
{
	EfiObj *obj = NULL;
	EfiType *type = NULL;
	
	if	(io_eat(io, " \t") == '(')
	{
		Parse_fmt(io, "(T)", &obj);
	}
	else	obj = EvalObj(Parse_term(io, OpPrior_Unary), NULL);

	if	((obj = EvalObj(obj, NULL)))
	{
		type = obj->type;
		UnrefObj(obj);
	}

	return type2Obj(Parse_type(io, type));
}

static EfiObj *pf_provide (IO *io, void *data)
{
	return type2Obj(Parse_type(io, NULL));
}

static EfiObj *p_interact (IO *io, void *data)
{
	char *prompt;
	char *hist;
	IO *in;

	prompt = Obj2str(Parse_term(io, 0));
	hist = Obj2str(Parse_term(io, 0));
	in = io_interact(prompt, hist);
	CmdEval(in, iomsg);
	io_close(in);
	memfree(prompt);
	memfree(hist);
	return NULL;
}

static EfiObj *p_local (IO *io, void *data)
{
	return NewPtrObj(&Type_vtab, rd_refer(LocalVar));
}

static EfiObj *p_cinterp (IO *io, void *data)
{
	return NewPtrObj(&Type_efi, rd_refer(Efi_ptr(NULL)));
}

static EfiParseDef pdef[] = {
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
	{ "parsub", pf_parsub, NULL },
	{ "static", pf_static, NULL },
	{ "const", pf_const, NULL },

	{ "struct", PFunc_struct, NULL },
	{ "construct", PFunc_construct, NULL },
	{ "enum", PFunc_enum, NULL },
	{ "typedef", PFunc_typedef, NULL },
	{ "typeof", pf_typeof, NULL },
	{ "provide", pf_provide, NULL },

	{ "interactive", p_interact, NULL },
	{ "local", p_local, NULL },
	{ "cinterp", p_cinterp, NULL },
};


/*	Komponenten von Listen
*/

static EfiObj *list_obj (const EfiObj *obj, void *data)
{
	EfiObjList *list = obj ? Val_list(obj->data) : NULL;
	return list ? RefObj(list->obj) : ptr2Obj(NULL);
}

static EfiObj *list_next (const EfiObj *obj, void *data)
{
	EfiObjList *list = obj ? Val_list(obj->data) : NULL;
	list = list ? RefObjList(list->next) : NULL;
	return NewPtrObj(&Type_list, list);
}

static EfiMember list_member[] = {
	{ "next", &Type_list, list_next, NULL },
	{ "obj", NULL, list_obj, NULL },
};

/*	Komponenten von Referenzen
*/

static EfiObj *ref_refcount (const EfiObj *obj, void *data)
{
	RefData *rd = Val_ptr(obj->data);
	return int2Obj(rd ? rd->refcount : 0);
}

static EfiObj *ref_ident (const EfiObj *obj, void *data)
{
	return str2Obj(rd_ident(Val_ptr(obj->data)));
}

static EfiMember ref_member[] = {
	{ "ident", &Type_str, ref_ident, NULL },
	{ "refcount", &Type_int, ref_refcount, NULL },
};

/*	Parametersubstitution
*/

static int c_ptr (IO *in, IO *out, void *arg)
{
	EfiObj *base;
	char *p;
	int c;
	int n;

	p = Parse_name(in, 1);

	if	(!p)
		return iocpy_skip(in, NULL, 0, arg, 0);

	n = 0;
	base = GetVar(NULL, p, NULL);
	base = EvalObj(base, NULL);
	c = io_getc(in);

	if	(base && c == '.')
	{
		EfiObj *a;
		EfiObj *obj;

		obj = RefObj(base);
		io_ungetc(c, in);

		while ((a = Parse_op(in, OpPrior_Unary, obj)) != NULL)
			obj = a;

		c = io_getc(in);
		obj = EvalObj(obj, NULL);

		n += io_printf(out, "((%s *) ((char *) %s->data",
			obj && obj->type->cname ? obj->type->cname : "void", p);

		if	(base && obj)
			n += io_printf(out, " + %d",
				(char *) obj->data - (char *) base->data);

		UnrefObj(obj);

		n += io_puts("))", out);
	}
	else if	(base && base->type->cname)
	{
		n += io_printf(out, "((%s *) %s->data)",
			base->type->cname, p);
	}
	else	n += io_printf(out, "%s->data", p);

	UnrefObj(base);
	memfree(p);
	
	if	(!listcmp(arg, c))
		iocpy_skip(in, NULL, c, arg, 0);

	return n;
}

static int c_term(IO *in, IO *out, void *arg)
{
	EfiObj *obj;
	EfiObjList *list;
	char *fmt;
	int c;

	c = io_getc(in);

	if	(c == '&')
		return c_ptr(in, out, arg);

	io_ungetc(c, in);
	obj = EvalObj(Parse_term(in, OpPrior_Cond), NULL);
	c = io_getc(in);

	fmt = (c == ':') ? io_xgets(in, arg) : NULL;

	if	(!listcmp(arg, c))
		iocpy_skip(in, NULL, c, arg, 0);

	if	(obj && fmt)
	{
		list = NewObjList(obj);
		c = PrintFmtList(out, fmt, list);
		DelObjList(list);
	}
	else if	(obj)
	{
		c = ShowObj(out, obj);
		UnrefObj(obj);
	}
	else	c = 0;

	memfree(fmt);
	return c;
}

static int c_eval(IO *in, IO *out, void *arg)
{
	return iocpy_eval(in, out, 0, "}", 0);
}


static int f_eval (CmdPar *cpar, CmdParVar *var,
	const char *par, const char *arg)
{
	char *p = CmdPar_psub(cpar, par, arg);
	streval(p);
	memfree(p);
	return 0;
}

static CmdParEval cpar_eval = { "eval", "Ausdruck auswerten", f_eval };


static void f_proto (CmdPar *par, IO *io, const char *arg)
{
	char *ident;
	char *name;
	char *type;
	EfiVirFunc *tab;
	EfiFunc **ftab;
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

static CmdParExpand cexp_proto = { "proto", "Prototype ausgeben", f_proto };

extern int float_align;

char *PS1 = "$!: ";
char *PS2 = "> ";

static EfiVarDef vardef[] = {
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
	{ "Lang", &Type_str, &LangDef.language,
		":*:language code of LANG environment\n"
		":de:Sprachkode der Umgebungsvariable LANG\n" },
	{ "Territory", &Type_str, &LangDef.territory,
		":*:territory code of LANG environment\n"
		":de:Regionalcode der Umgebungsvariable LANG\n" },
	{ "memtrace", &Type_bool, &memtrace,
		":*:flag to control allocation tracing\n"
		":de:Flag zur Ablaufverfolgung der Speicherverwaltung\n" },
};


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
	CmdSetup_uint();
	CmdSetup_varint();
	CmdSetup_varsize();
	CmdSetup_float();
	CmdSetup_double();
	CmdSetup_str();
	CmdSetup_io();
	CmdSetup_stat();

	CmdSetup_int8();
	CmdSetup_int16();
	CmdSetup_int32();
	CmdSetup_int64();

	CmdSetup_uint8();
	CmdSetup_uint16();
	CmdSetup_uint32();
	CmdSetup_uint64();

	AddType(&Type_name);
	AddType(&Type_mname);
	AddType(&Type_sname);
	AddType(&Type_undef);
	AddType(&Type_list);
	AddType(&Type_explist);
	AddType(&Type_vdef);

	SetupCmpDef();
	SetupTestDef();

	CmdSetup_vec();
	CmdSetup_obj();
	CmdSetup_pctrl();
	CmdSetup_test();

	CmdSetup_konv();
	CmdSetup_print();
	CmdSetup_func();
	CmdSetup_range();
	CmdSetup_op();
	CmdSetup_info();
	CmdSetup_strbuf();
	CmdSetup_date();
	CmdSetup_regex();
	CmdSetup_cmdpar();
	CmdSetup_unix();
	CmdSetup_tms();
	CmdSetup_dl();
	CmdSetup_dbutil();

	AddEfiMember(Type_list.vtab, list_member, tabsize(list_member));
	AddEfiMember(Type_ref.vtab, ref_member, tabsize(ref_member));
	AddVarDef(NULL, vardef, tabsize(vardef));

	psubfunc('(', c_term, ")");
	psubfunc('{', c_eval, "}");

	CmdParEval_add(&cpar_eval);
	CmdParExpand_add(&cexp_proto);
	AddInfo(NULL, KEY_VAR, LBL_VAR, var_info, RefVarTab(GlobalVar));
	AddInfo(NULL, KEY_FUNC, LBL_FUNC, func_info, RefVarTab(GlobalVar));
	CmdSetup_type();
}
