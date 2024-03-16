/*
Initialisierung

$Copyright (C) 1999 Erich Frühstück
This file is part of EFEU.

EFEU is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

EFEU is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public
License along with EFEU; see the file COPYING.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <EFEU/efutil.h>
#include <EFEU/pconfig.h>
#include <EFEU/cmdeval.h>
#include <EFEU/preproc.h>
#include <EFEU/database.h>
#include <EFEU/parsedef.h>
#include <EFEU/mdmat.h>
#include <EFEU/Random.h>
#include <EFEU/cmdeval.h>
#include <Math/TimeSeries.h>
#include <Math/pnom.h>
#include <Math/mdmath.h>
#include <EFEU/stdtype.h>
#include <EFEU/cmdconfig.h>
#include "LaTeX.h"
#include "efeudoc.h"

EfiType Type_Doc = REF_TYPE("Doc", Doc *);

static EfiObj *doc_var (const EfiObj *obj, void *data)
{
	if	(obj)
	{
		Doc *doc = Val_ptr(obj->data);

		if	(doc && doc->cmd_stack)
		{
			DocTab *tab = doc->cmd_stack->data;
			return LvalObj(&Lval_ref, &Type_vtab, tab, &tab->var);
		}
	}

	return NULL;
}

static EfiObj *doc_out (const EfiObj *obj, void *data)
{
	Doc *doc = obj ? Val_ptr(obj->data) : NULL;
	return NewPtrObj(&Type_io, doc ? rd_refer(doc->out) : NULL);
}

static EfiObj *doc_stat (const EfiObj *obj, void *data)
{
	int x = obj && ((Doc **) obj->data)[0]->stat ? 1 : 0;
	return NewObj(&Type_bool, &x);
}

static EfiObj *doc_vmode (const EfiObj *obj, void *data)
{
	int x = obj && ((Doc **) obj->data)[0]->env.hmode == 0 ? 1 : 0;
	return NewObj(&Type_bool, &x);
}

static EfiObj *doc_hmode (const EfiObj *obj, void *data)
{
	int x = obj && ((Doc **) obj->data)[0]->env.hmode == 1 ? 1 : 0;
	return NewObj(&Type_bool, &x);
}

static EfiObj *doc_rmode (const EfiObj *obj, void *data)
{
	int x = obj && ((Doc **) obj->data)[0]->env.hmode == 2 ? 1 : 0;
	return NewObj(&Type_bool, &x);
}

static EfiMember var_doc[] = {
	{ "var", &Type_vtab, doc_var, NULL },
	{ "out", &Type_io, doc_out, NULL },
	{ "stat", &Type_bool, doc_stat, NULL },
	{ "vmode", &Type_bool, doc_vmode, NULL },
	{ "hmode", &Type_bool, doc_hmode, NULL },
	{ "rmode", &Type_bool, doc_rmode, NULL },
};

static void f_push (EfiFunc *func, void *rval, void **arg)
{
	Doc *doc = Val_ptr(arg[0]);

	if	(doc)
		pushstack(&doc->cmd_stack, DocTab_create(Val_str(arg[1])));
}

static void f_pop (EfiFunc *func, void *rval, void **arg)
{
	Doc *doc = Val_ptr(arg[0]);

	if	(doc)
		rd_deref(popstack(&doc->cmd_stack, NULL));
}

static void f_load (EfiFunc *func, void *rval, void **arg)
{
	Doc *doc = Val_ptr(arg[0]);
	Doc_pushvar(doc);
	CmdEval(Val_io(arg[1]), Val_io(arg[2]));
	Doc_popvar(doc);
}

static void f_stack (EfiFunc *func, void *rval, void **arg)
{
	Stack *ptr;
	EfiObjList **lp;
	Doc *doc;
	
	Val_list(rval) = NULL;
	doc = Val_ptr(arg[0]);

	if	(doc == NULL)	return;

	lp = &Val_list(rval);

	for (ptr = doc->cmd_stack; ptr != NULL; ptr = ptr->next)
	{
		DocTab *tab = ptr->data;
		*lp = NewObjList(str2Obj(tab ? mstrcpy(tab->name) : NULL));
		lp = &(*lp)->next;
	}
}

/*	Befehlsauflistung
*/

static void f_maclist (EfiFunc *func, void *rval, void **arg)
{
	DocTab *tab;
	EfiObjList **lp;
	DocMac *mp;
	size_t n;
	
	Val_list(rval) = NULL;
	tab = Doc_gettab(Val_ptr(arg[0]), Val_str(arg[1]));

	if	(tab == NULL)	return;

	lp = &Val_list(rval);

	for (mp = tab->mtab.data, n = tab->mtab.used; n-- > 0; mp++)
	{
		*lp = NewObjList(str2Obj(mstrcpy(mp->name)));
		lp = &(*lp)->next;
	}
}

static void f_showmac (EfiFunc *func, void *rval, void **arg)
{
	DocMac_print(Val_io(arg[2]),
		Doc_getmac(Val_ptr(arg[0]), Val_str(arg[1])),
		Val_int(arg[3]));
}

static void f_testmac (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = Doc_getmac(Val_ptr(arg[0]), Val_str(arg[1])) != NULL;
}

static void doc2vtab (EfiFunc *func, void *rval, void **arg)
{
	Val_vtab(rval) = rd_refer(Doc_vartab(Val_ptr(arg[0])));
}

static void f_comment (EfiFunc *func, void *rval, void **arg)
{
	Doc *doc = Val_ptr(arg[0]);
	io_ctrl(doc->out, DOC_REM, Val_str(arg[1]));
}

static void f_def (EfiFunc *func, void *rval, void **arg)
{
	Doc *doc = Val_ptr(arg[0]);
	DocTab_def(doc->cmd_stack->data, Val_io(arg[1]), &doc->buf);
	sb_trunc(&doc->buf);
}

static void f_include (EfiFunc *func, void *rval, void **arg)
{
	Doc_include(Val_ptr(arg[0]), Val_str(arg[1]), Val_str(arg[2]));
}

static void f_input (EfiFunc *func, void *rval, void **arg)
{
	Doc_input(Val_ptr(arg[0]), Val_str(arg[1]), io_refer(Val_io(arg[2])));
}

static void f_config (EfiFunc *func, void *rval, void **arg)
{
	DocTab_fload(GlobalDocTab, Val_str(arg[1]));
}

static void f_mode (EfiFunc *func, void *rval, void **arg)
{
	Doc *doc = Val_ptr(arg[0]);
	
	switch (Val_char(arg[1]))
	{
	case 'h':	Doc_hmode(doc); break;
	case 'v':	Doc_par(doc); break;
	default:	Doc_start(doc); break;
	}
}

static void f_cmd (EfiFunc *func, void *rval, void **arg)
{
	Doc *doc = Val_ptr(arg[0]);
	EfiObjList *list = Val_list(arg[2]);
	
	switch (Val_char(arg[1]))
	{
	case '\n':
		io_ctrl(doc->out, DOC_CMD, DOC_CMD_BREAK);
		doc->nl = 0;
		Doc_newline(doc);
		doc->nl = 0;
		break;
	case '\f':
		io_ctrl(doc->out, DOC_CMD, DOC_CMD_NPAGE);
		doc->nl = 0;
		Doc_newline(doc);
		break;
	case 'a':
		io_ctrl(doc->out, DOC_CMD, DOC_CMD_APP, ListArg_str(list, 0));
		break;
	case 'c':
		io_ctrl(doc->out, DOC_CMD, DOC_CMD_TOC);
		break;
	case 'f':
		io_ctrl(doc->out, DOC_CMD, DOC_CMD_LOF);
		break;
	case 't':
		io_ctrl(doc->out, DOC_CMD, DOC_CMD_LOT);
		break;
	case 'm':
		io_ctrl(doc->out, DOC_CMD, DOC_CMD_MARK, ListArg_int(list, 0));
		break;
	default:
		break;
	}
}

static void f_plaintex (EfiFunc *func, void *rval, void **arg)
{
	Doc *doc = Val_ptr(arg[0]);

	Val_int(rval) =	(io_ctrl(doc->out, DOC_CMD, DOC_CMD_TEX,
		Val_str(arg[1])) != EOF) ? 1 : 0;
}

static void f_plain (EfiFunc *func, void *rval, void **arg)
{
	Doc *doc = Val_ptr(arg[0]);

	io_ctrl(doc->out, DOC_BEG, DOC_MODE_PLAIN);
	Doc_str(doc, Val_str(arg[1]));
	io_ctrl(doc->out, DOC_END, DOC_MODE_PLAIN);
}

static void ref_std (Doc *doc, const char *name)
{
	Doc_char(doc, '[');
	Doc_str(doc, name);
	Doc_char(doc, ']');
}

static void ref_man (Doc *doc, const char *name)
{
	io_ctrl(doc->out, DOC_BEG, DOC_ATT_IT);
	Doc_str(doc, name);
	io_ctrl(doc->out, DOC_END, DOC_ATT_IT);
}

static void f_label (EfiFunc *func, void *rval, void **arg)
{
	static void (*put_ref) (Doc *doc, const char *name);
	Doc *doc;
	char *name;
	int type;
	
	name = Val_str(arg[2]);

	if	(name == NULL)	return;

	doc = Val_ptr(arg[0]);
	type = DOC_REF_STD;
	put_ref = ref_std;
	Doc_hmode(doc);
	
	switch (Val_char(arg[1]))
	{
	case 'l':	type = DOC_REF_LABEL; break;
	case 'p':	type = DOC_REF_PAGE; break;
	case 'v':	type = DOC_REF_VAR; break;
	case 'm':	type = DOC_REF_MAN; put_ref = ref_man; break;
	default:	break;
	}

	if	(io_ctrl(doc->out, DOC_CMD, type, name) == EOF)
		put_ref(doc, name);
}

static void f_url (EfiFunc *func, void *rval, void **arg)
{
	Doc *doc = Val_ptr(arg[0]);

	Doc_hmode(doc);
	io_ctrl(doc->out, DOC_BEG, DOC_ENV_URL, Val_str(arg[1]));
	Doc_str(doc, Val_str(arg[2]));
	io_ctrl(doc->out, DOC_END, DOC_ENV_URL);
}

static void f_index (EfiFunc *func, void *rval, void **arg)
{
	Doc *doc = Val_ptr(arg[0]);
	io_ctrl(doc->out, DOC_CMD, DOC_CMD_IDX,
		Val_str(arg[1]), Val_str(arg[2]));
}

static void f_newenv (EfiFunc *func, void *rval, void **arg)
{
	Doc *doc = Val_ptr(arg[0]);
	EfiObjList *list = Val_list(arg[2]);
	char *a1, *a2;
	
	switch (Val_char(arg[1]))
	{
	case 'c':	Doc_newenv(doc, 0, DOC_ENV_CODE); break;
	case 'h':	Doc_newenv(doc, 0, DOC_ENV_HANG); break;
	case 'q':	Doc_newenv(doc, 4, DOC_ENV_QUOTE); break;
	case 'i':	Doc_newenv(doc, 0, DOC_ENV_INTRO); break;
	case 's':	Doc_newenv(doc, 0, DOC_ENV_SLOPPY); break;
	case '$':
		Doc_hmode(doc);
		io_ctrl(doc->out, DOC_BEG, DOC_ENV_FORMULA);
		break;
	case 'm':
		Doc_endall(doc, 0);
		a1 = ListArg_str(list, 0);
		a2 = ListArg_str(list, 1);
		Doc_newenv(doc, 0, DOC_ENV_MPAGE, a1, a2);
		break;
	case 'p':
		Doc_endall(doc, 1);
		a1 = ListArg_str(list, 0);
		a2 = ListArg_str(list, 1);
		Doc_newenv(doc, 0, DOC_ENV_SPAGE, a1, a2);
		break;
	case 'f':
		Doc_endall(doc, 1);
		Doc_newenv(doc, 0, DOC_ENV_FIG, ListArg_str(list, 0));
		break;
	case 't':
		Doc_endall(doc, 1);
		Doc_newenv(doc, 0, DOC_ENV_TABLE, ListArg_str(list, 0));
		break;
	case 'b':
		Doc_endall(doc, 0);
		Doc_newenv(doc, 0, DOC_ENV_BIB, ListArg_str(list, 0));
		break;
	default:	break;
	}
}

static void f_tab (EfiFunc *func, void *rval, void **arg)
{
	Doc_tab(Val_ptr(arg[0]), CmdEval_cin, Val_str(arg[1]), Val_str(arg[2]));
}

static void f_end (EfiFunc *func, void *rval, void **arg)
{
	Doc *doc = Val_ptr(arg[0]);
	
	switch (Val_char(arg[1]))
	{
	case 'a':	Doc_endall(doc, 0); break;
	case 'x':	Doc_endall(doc, 1); break;
	case 'l':	Doc_endlist(doc); break;
	case '$':
		io_ctrl(doc->out, DOC_END, DOC_ENV_FORMULA);
		break;
	default:
		Doc_endenv(doc); break;
	}
}

static void f_newline (EfiFunc *func, void *rval, void **arg)
{
	Doc *doc = Val_ptr(arg[0]);
	
	if	(doc->nl == 0)
	{
		if	(doc->env.cpos)
			io_putc('\n', doc->out);

		doc->env.cpos = 0;
	}

	doc->nl = 1;
	doc->indent = 0;
}


static void f_expand (EfiFunc *func, void *rval, void **arg)
{
	Val_str(rval) = Doc_expand(Val_ptr(arg[0]),
		io_refer(Val_io(arg[1])), Val_int(arg[2]));
}

static void f_xexpand (EfiFunc *func, void *rval, void **arg)
{
	Val_str(rval) = Doc_xexpand(Val_ptr(arg[0]),
		io_refer(Val_io(arg[1])));
}

static void f_eval (EfiFunc *func, void *rval, void **arg)
{
	Doc_eval(Val_ptr(arg[0]), CmdEval_cin, Val_ptr(arg[1]));
}

static void f_plainarg (EfiFunc *func, void *rval, void **arg)
{
	Val_str(rval) = DocParseArg(CmdEval_cin,
		Val_char(arg[1]), Val_char(arg[2]), Val_int(arg[3]));
}

static void f_stdarg (EfiFunc *func, void *rval, void **arg)
{
	IO *io = io_mstr(DocParseArg(CmdEval_cin,
		Val_char(arg[1]), Val_char(arg[2]), Val_int(arg[3])));
	Val_str(rval) = Doc_expand(Val_ptr(arg[0]), io, 1);
}

static void f_longarg (EfiFunc *func, void *rval, void **arg)
{
	IO *io = io_mstr(DocParseArg(CmdEval_cin,
		Val_char(arg[1]), Val_char(arg[2]), Val_int(arg[3])));
	Val_str(rval) = Doc_expand(Val_ptr(arg[0]), io, 0);
}

static void f_sync (EfiFunc *func, void *rval, void **arg)
{
	int c = io_skipcom(CmdEval_cin, NULL, 1);

	if	(c == '\n')
	{
		Doc_newline(Val_ptr(arg[0]));
	}
	else	io_ungetc(c, CmdEval_cin);
}

static void f_cfgname (EfiFunc *func, void *rval, void **arg)
{
	Val_str(rval) = fsearch(CFGPath, NULL, Val_str(arg[0]), NULL);
	/*
	io_xprintf(ioerr, "%s\n", Val_str(rval));
	*/
	AddDepend(Val_str(rval));
}

/*	Parse - Funktionen
*/

static void SkipSpace (EfiFunc *func, void *rval, void **arg)
{
	Val_char(rval) = DocSkipSpace(Val_io(arg[0]), Val_bool(arg[1]));
}

static void SkipWhite (EfiFunc *func, void *rval, void **arg)
{
	Val_char(rval) = DocSkipWhite(Val_io(arg[0]));
}

static void ParseKey (EfiFunc *func, void *rval, void **arg)
{
	if	(io_peek(Val_io(arg[0])) == (int) Val_char(arg[1]))
	{
		io_getc(Val_io(arg[0]));
		Val_int(rval) = 1;
	}
	else	Val_int(rval) = 0;
}

static void ParseName (EfiFunc *func, void *rval, void **arg)
{
	Val_str(rval) = mstrcpy(DocParseName(Val_io(arg[0]), Val_char(arg[1])));
}

static void ParseLine (EfiFunc *func, void *rval, void **arg)
{
	Val_str(rval) = DocParseLine(Val_io(arg[0]), Val_int(arg[1]));
}

static void ParsePar (EfiFunc *func, void *rval, void **arg)
{
	Val_str(rval) = DocParsePar(Val_io(arg[0]));
}

static void ParseExpr (EfiFunc *func, void *rval, void **arg)
{
	Val_str(rval) = DocParseExpr(Val_io(arg[0]));
}

static void ParseRegion (EfiFunc *func, void *rval, void **arg)
{
	Val_str(rval) = DocParseRegion(Val_io(arg[0]), Val_str(arg[1]));
}

static void ParseBlock (EfiFunc *func, void *rval, void **arg)
{
	Val_str(rval) = DocParseBlock(Val_io(arg[0]), Val_bool(arg[1]),
		Val_str(arg[2]), Val_str(arg[3]), Val_str(arg[4]));
}

static void ParseVerb (EfiFunc *func, void *rval, void **arg)
{
	Val_str(rval) = DocParseArg(Val_io(arg[0]), '|', '|', Val_bool(arg[1]));
}

static void ParseOpt (EfiFunc *func, void *rval, void **arg)
{
	Val_str(rval) = DocParseArg(Val_io(arg[0]), '[', ']', Val_bool(arg[1]));
}

static void ParseArg (EfiFunc *func, void *rval, void **arg)
{
	Val_str(rval) = DocParseArg(Val_io(arg[0]), '{', '}', Val_bool(arg[1]));
}

/*	Funktionstabelle
*/

static EfiFuncDef func_doc[] = {
	{ FUNC_RESTRICTED, &Type_vtab, "Doc ()", doc2vtab },
	{ 0, &Type_void, "Doc::push (str name)", f_push },
	{ 0, &Type_void, "Doc::pop (void)", f_pop },
	{ 0, &Type_list, "Doc::stack ()", f_stack },
	{ 0, &Type_void, "Doc::load (IO in, IO log = NULL)", f_load },
	{ 0, &Type_list, "Doc::maclist (str name = NULL)", f_maclist },
	{ 0, &Type_void, "Doc::showmac (str name, "
		"IO io = cout, int mode = 0)", f_showmac },
	{ 0, &Type_bool, "Doc::testmac (str name)", f_testmac },

	{ 0, &Type_void, "Doc::mode (char type)", f_mode },
	{ 0, &Type_void, "Doc::cmd (char type, ...)", f_cmd },
	{ 0, &Type_bool, "Doc::plaintex (str cmd = NULL)", f_plaintex },
	{ 0, &Type_void, "Doc::plain (str arg = NULL)", f_plain },
	{ 0, &Type_void, "Doc::comment (str comment)", f_comment },
	{ 0, &Type_void, "Doc::label (char type, str name)", f_label },
	{ 0, &Type_void, "Doc::url (str name, str label)", f_url },
	{ 0, &Type_void, "Doc::index (str name, str label)", f_index },
	{ 0, &Type_void, "Doc::newenv (char type, ...)", f_newenv },
	{ 0, &Type_void, "Doc::endenv (char type)", f_end },
	{ 0, &Type_void, "Doc::newline ()", f_newline },
	{ 0, &Type_void, "Doc::def (IO in = cin)", f_def },
	{ 0, &Type_void, "Doc::include (str opt, str name)", f_include },
	{ 0, &Type_void, "Doc::input (str opt, IO in)", f_input },
	{ 0, &Type_void, "Doc::config (str name)", f_config },
	{ 0, &Type_void, "Doc::tab (str opt, str arg)", f_tab },
	{ 0, &Type_void, "Doc::section (char type, str toc, str head)",
		DocFunc_section },
	{ 0, &Type_void, "Doc::eval (str expr)", f_eval },
	{ 0, &Type_str, "Doc::expand (IO in, bool flag = true)", f_expand },
	{ 0, &Type_str, "Doc::xexpand (IO in)", f_xexpand },
	{ 0, &Type_str, "Doc::PlainArg (char beg = 0, char end = '\n', "
		"bool flag = false)", f_plainarg },
	{ 0, &Type_str, "Doc::StdArg (char beg = 0, char end = '\n', "
		"bool flag = false)", f_stdarg },
	{ 0, &Type_str, "Doc::LongArg (char beg = 0, char end = '\n', "
		"bool flag = false)", f_longarg },
	{ 0, &Type_void, "Doc::sync ()", f_sync },
	{ 0, &Type_str, "cfgname (str name)", f_cfgname },

	{ 0, &Type_char, "SkipSpace (IO io, bool nl = false)", SkipSpace },
	{ 0, &Type_char, "SkipWhite (IO io)", SkipWhite },
	{ 0, &Type_bool, "ParseKey (IO io, int key)", ParseKey },
	{ 0, &Type_str, "ParseName (IO io, int key = 0)", ParseName },
	{ 0, &Type_str, "ParseLine (IO io, bool flag = false)", ParseLine },
	{ 0, &Type_str, "ParsePar (IO io)", ParsePar },
	{ 0, &Type_str, "ParseExpr (IO io)", ParseExpr },
	{ 0, &Type_str, "ParseRegion (IO io, str delim = NULL)", ParseRegion },
	{ 0, &Type_str, "ParseBlock (IO io, bool mode, str beg = \"beg\", "
		"str end = \"end\", str toggle = NULL)", ParseBlock },
	{ 0, &Type_str, "ParseVerb (IO io, bool flag = false)", ParseVerb },
	{ 0, &Type_str, "ParseOpt (IO io, bool flag = false)", ParseOpt },
	{ 0, &Type_str, "ParseArg (IO io, bool flag = false)", ParseArg },
};


DocTab *GlobalDocTab = NULL;

void SetupDoc (void)
{
	static int setup_done = 0;

	if	(setup_done)	return;

	SetupStd();
	SetupUtil();
	SetupPreproc();
	SetupDataBase();
	SetupEDB();
	SetupTimeSeries();
	SetupRandom();
	SetupMdMat();
	SetupMath();
	SetupMdMath();
	SetupPnom();

	AddType(&Type_Doc);
	AddEfiMember(Type_Doc.vtab, var_doc, tabsize(var_doc));
	AddFuncDef(func_doc, tabsize(func_doc));
	GlobalDocTab = DocTab_create("global");
}
