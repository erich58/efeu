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

Type_t Type_Doc = REF_TYPE("Doc", Doc_t *);

static VarTab_t **doc_var (Doc_t **ptr)
{
	if	(*ptr && (*ptr)->cmd_stack)
	{
		DocTab_t *tab = (*ptr)->cmd_stack->data;
		return &tab->var;
	}

	return NULL;
}

static io_t **doc_out (Doc_t **ptr)
{
	return *ptr ? &(*ptr)->out : NULL;
}

static int *doc_stat (Doc_t **ptr)
{
	Buf_int = *ptr ? (*ptr)->stat : 0;
	return &Buf_int;
}

static int *doc_vmode (Doc_t **ptr)
{
	Buf_int = *ptr && (*ptr)->env.hmode == 0 ? 1 : 0;
	return &Buf_int;
}

static int *doc_hmode (Doc_t **ptr)
{
	Buf_int = *ptr && (*ptr)->env.hmode ? 1 : 0;
	return &Buf_int;
}

static int *doc_rmode (Doc_t **ptr)
{
	Buf_int = *ptr && (*ptr)->env.hmode == 2 ? 1 : 0;
	return &Buf_int;
}

static MemberDef_t var_doc[] = {
	{ "var", &Type_vtab, LvalMember, doc_var },
	{ "out", &Type_io, ConstMember, doc_out },
	{ "stat", &Type_bool, ConstMember, doc_stat },
	{ "vmode", &Type_bool, ConstMember, doc_vmode },
	{ "hmode", &Type_bool, ConstMember, doc_hmode },
	{ "rmode", &Type_bool, ConstMember, doc_rmode },
};

static void f_push (Func_t *func, void *rval, void **arg)
{
	Doc_t *doc = Val_ptr(arg[0]);

	if	(doc)
		pushstack(&doc->cmd_stack, DocTab(Val_str(arg[1])));
}

static void f_pop (Func_t *func, void *rval, void **arg)
{
	Doc_t *doc = Val_ptr(arg[0]);

	if	(doc)
		rd_deref(popstack(&doc->cmd_stack, NULL));
}

static void f_load (Func_t *func, void *rval, void **arg)
{
	Doc_t *doc = Val_ptr(arg[0]);
	Doc_pushvar(doc);
	CmdEval(Val_io(arg[1]), Val_io(arg[2]));
	Doc_popvar(doc);
}

static void f_stack (Func_t *func, void *rval, void **arg)
{
	stack_t *ptr;
	ObjList_t **lp;
	Doc_t *doc;
	
	Val_list(rval) = NULL;
	doc = Val_ptr(arg[0]);

	if	(doc == NULL)	return;

	lp = &Val_list(rval);

	for (ptr = doc->cmd_stack; ptr != NULL; ptr = ptr->next)
	{
		DocTab_t *tab = ptr->data;
		*lp = NewObjList(str2Obj(tab ? mstrcpy(tab->name) : NULL));
		lp = &(*lp)->next;
	}
}

/*	Befehlsauflistung
*/

static void f_maclist (Func_t *func, void *rval, void **arg)
{
	DocTab_t *tab;
	ObjList_t **lp;
	DocMac_t *mp;
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

static void f_showmac (Func_t *func, void *rval, void **arg)
{
	DocMac_print(Val_io(arg[2]),
		Doc_getmac(Val_ptr(arg[0]), Val_str(arg[1])),
		Val_int(arg[3]));
}

static void doc2vtab (Func_t *func, void *rval, void **arg)
{
	Val_vtab(rval) = rd_refer(Doc_vartab(Val_ptr(arg[0])));
}

static void f_comment (Func_t *func, void *rval, void **arg)
{
	Doc_t *doc = Val_ptr(arg[0]);
	io_ctrl(doc->out, DOC_REM, Val_str(arg[1]));
}

static void f_def (Func_t *func, void *rval, void **arg)
{
	Doc_t *doc = Val_ptr(arg[0]);
	DocTab_def(doc->cmd_stack->data, Val_io(arg[1]), doc->buf);
	sb_clear(doc->buf);
}

static void f_include (Func_t *func, void *rval, void **arg)
{
	Doc_include(Val_ptr(arg[0]), Val_str(arg[1]), Val_str(arg[2]));
}

static void f_input (Func_t *func, void *rval, void **arg)
{
	Doc_input(Val_ptr(arg[0]), Val_str(arg[1]), io_refer(Val_io(arg[2])));
}

static void f_config (Func_t *func, void *rval, void **arg)
{
	DocTab_fload(GlobalDocTab, Val_str(arg[1]));
}

static void f_mode (Func_t *func, void *rval, void **arg)
{
	Doc_t *doc = Val_ptr(arg[0]);
	
	switch (Val_char(arg[1]))
	{
	case 'h':	Doc_hmode(doc); break;
	case 'v':	Doc_par(doc); break;
	default:	Doc_start(doc); break;
	}
}

static void f_cmd (Func_t *func, void *rval, void **arg)
{
	Doc_t *doc = Val_ptr(arg[0]);
	ObjList_t *list = Val_list(arg[2]);
	
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
	case 't':
		io_ctrl(doc->out, DOC_CMD, DOC_CMD_TOC);
		break;
	case 'f':
		io_ctrl(doc->out, DOC_CMD, DOC_CMD_LOF);
		break;
	case 'm':
		io_ctrl(doc->out, DOC_CMD, DOC_CMD_MARK, ListArg_int(list, 0));
		break;
	default:
		break;
	}
}

static void f_plaintex (Func_t *func, void *rval, void **arg)
{
	Doc_t *doc = Val_ptr(arg[0]);

	Val_int(rval) =	(io_ctrl(doc->out, DOC_CMD, DOC_CMD_TEX,
		Val_str(arg[1])) != EOF) ? 1 : 0;
}

static void f_plain (Func_t *func, void *rval, void **arg)
{
	Doc_t *doc = Val_ptr(arg[0]);

	io_ctrl(doc->out, DOC_BEG, DOC_MODE_PLAIN);
	Doc_str(doc, Val_str(arg[1]));
	io_ctrl(doc->out, DOC_END, DOC_MODE_PLAIN);
}

static void ref_std (Doc_t *doc, const char *name)
{
	Doc_char(doc, '[');
	Doc_str(doc, name);
	Doc_char(doc, ']');
}

static void ref_man (Doc_t *doc, const char *name)
{
	io_ctrl(doc->out, DOC_BEG, DOC_ATT_IT);
	Doc_str(doc, name);
	io_ctrl(doc->out, DOC_END, DOC_ATT_IT);
}

static void f_label (Func_t *func, void *rval, void **arg)
{
	static void (*put_ref) (Doc_t *doc, const char *name);
	Doc_t *doc;
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

static void f_url (Func_t *func, void *rval, void **arg)
{
	Doc_t *doc = Val_ptr(arg[0]);

	Doc_hmode(doc);
	io_ctrl(doc->out, DOC_BEG, DOC_ENV_URL, Val_str(arg[1]));
	Doc_str(doc, Val_str(arg[2]));
	io_ctrl(doc->out, DOC_END, DOC_ENV_URL);
}

static void f_index (Func_t *func, void *rval, void **arg)
{
	Doc_t *doc = Val_ptr(arg[0]);
	io_ctrl(doc->out, DOC_CMD, DOC_CMD_IDX,
		Val_str(arg[1]), Val_str(arg[2]));
}

static void f_newenv (Func_t *func, void *rval, void **arg)
{
	Doc_t *doc = Val_ptr(arg[0]);
	ObjList_t *list = Val_list(arg[2]);
	char *a1, *a2;
	
	switch (Val_char(arg[1]))
	{
	case 'c':	Doc_newenv(doc, 0, DOC_ENV_CODE); break;
	case 'h':	Doc_newenv(doc, 0, DOC_ENV_HANG); break;
	case 'q':	Doc_newenv(doc, 4, DOC_ENV_QUOTE); break;
	case 'i':	Doc_newenv(doc, 0, DOC_ENV_INTRO); break;
	case 's':	Doc_newenv(doc, 0, DOC_ENV_SLOPPY); break;
	case '$':
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
	case 'b':
		Doc_endall(doc, 0);
		Doc_newenv(doc, 0, DOC_ENV_BIB, ListArg_str(list, 0));
		break;
	default:	break;
	}
}

static void f_tab (Func_t *func, void *rval, void **arg)
{
	Doc_tab(Val_ptr(arg[0]), CmdEval_cin, Val_str(arg[1]), Val_str(arg[2]));
}

static void f_end (Func_t *func, void *rval, void **arg)
{
	Doc_t *doc = Val_ptr(arg[0]);
	
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

static void f_newline (Func_t *func, void *rval, void **arg)
{
	Doc_t *doc = Val_ptr(arg[0]);
	
	if	(doc->nl == 0)
	{
		if	(doc->env.cpos)
			io_putc('\n', doc->out);

		doc->env.cpos = 0;
	}

	doc->nl = 1;
	doc->indent = 0;
}


static void f_expand (Func_t *func, void *rval, void **arg)
{
	Val_str(rval) = Doc_expand(Val_ptr(arg[0]),
		io_refer(Val_io(arg[1])), Val_int(arg[2]));
}

static void f_xexpand (Func_t *func, void *rval, void **arg)
{
	Val_str(rval) = Doc_xexpand(Val_ptr(arg[0]),
		io_refer(Val_io(arg[1])));
}

static void f_eval (Func_t *func, void *rval, void **arg)
{
	/*
	io_t *io = io_cmdpreproc(io_refer(Val_io(arg[1])));
	CmdEvalFunc(io, CmdEval_cout, 0);
	io_close(io);
	*/
	Doc_eval(Val_ptr(arg[0]), CmdEval_cin, Val_ptr(arg[1]));
}

static void f_plainarg (Func_t *func, void *rval, void **arg)
{
	Val_str(rval) = DocParseArg(CmdEval_cin,
		Val_char(arg[1]), Val_char(arg[2]), Val_int(arg[3]));
}

static void f_stdarg (Func_t *func, void *rval, void **arg)
{
	io_t *io = io_mstr(DocParseArg(CmdEval_cin,
		Val_char(arg[1]), Val_char(arg[2]), Val_int(arg[3])));
	Val_str(rval) = Doc_expand(Val_ptr(arg[0]), io, 1);
}

static void f_longarg (Func_t *func, void *rval, void **arg)
{
	io_t *io = io_mstr(DocParseArg(CmdEval_cin,
		Val_char(arg[1]), Val_char(arg[2]), Val_int(arg[3])));
	Val_str(rval) = Doc_expand(Val_ptr(arg[0]), io, 0);
}

static void f_sync (Func_t *func, void *rval, void **arg)
{
	int c = io_skipcom(CmdEval_cin, NULL, 1);

	if	(c == '\n')
	{
		Doc_newline(Val_ptr(arg[0]));
	}
	else	io_ungetc(c, CmdEval_cin);
}

static void f_cfgname (Func_t *func, void *rval, void **arg)
{
	Val_str(rval) = fsearch(CFGPATH, NULL, Val_str(arg[0]), NULL);
	io_printf(ioerr, "%s\n", Val_str(rval));
	AddDepend(Val_str(rval));
}

/*	Parse - Funktionen
*/

static void SkipSpace (Func_t *func, void *rval, void **arg)
{
	Val_char(rval) = DocSkipSpace(Val_io(arg[0]), Val_bool(arg[1]));
}

static void SkipWhite (Func_t *func, void *rval, void **arg)
{
	Val_char(rval) = DocSkipWhite(Val_io(arg[0]));
}

static void ParseKey (Func_t *func, void *rval, void **arg)
{
	if	(io_peek(Val_io(arg[0])) == (int) Val_char(arg[1]))
	{
		io_getc(Val_io(arg[0]));
		Val_int(rval) = 1;
	}
	else	Val_int(rval) = 0;
}

static void ParseName (Func_t *func, void *rval, void **arg)
{
	Val_str(rval) = mstrcpy(DocParseName(Val_io(arg[0]), Val_char(arg[1])));
}

static void ParseLine (Func_t *func, void *rval, void **arg)
{
	Val_str(rval) = DocParseLine(Val_io(arg[0]), Val_int(arg[1]));
}

static void ParsePar (Func_t *func, void *rval, void **arg)
{
	Val_str(rval) = DocParsePar(Val_io(arg[0]));
}

static void ParseExpr (Func_t *func, void *rval, void **arg)
{
	Val_str(rval) = DocParseExpr(Val_io(arg[0]));
}

static void ParseRegion (Func_t *func, void *rval, void **arg)
{
	Val_str(rval) = DocParseRegion(Val_io(arg[0]), Val_str(arg[1]));
}

static void ParseBlock (Func_t *func, void *rval, void **arg)
{
	Val_str(rval) = DocParseBlock(Val_io(arg[0]), Val_bool(arg[1]),
		Val_str(arg[2]), Val_str(arg[3]), Val_str(arg[4]));
}

static void ParseOpt (Func_t *func, void *rval, void **arg)
{
	Val_str(rval) = DocParseArg(Val_io(arg[0]), '[', ']', Val_bool(arg[1]));
}

static void ParseArg (Func_t *func, void *rval, void **arg)
{
	Val_str(rval) = DocParseArg(Val_io(arg[0]), '{', '}', Val_bool(arg[1]));
}

/*	Funktionstabelle
*/

static FuncDef_t func_doc[] = {
	{ FUNC_RESTRICTED, &Type_vtab, "Doc ()", doc2vtab },
	{ 0, &Type_void, "Doc::push (str name)", f_push },
	{ 0, &Type_void, "Doc::pop (void)", f_pop },
	{ 0, &Type_list, "Doc::stack ()", f_stack },
	{ 0, &Type_void, "Doc::load (IO in, IO log = NULL)", f_load },
	{ 0, &Type_list, "Doc::maclist (str name = NULL)", f_maclist },
	{ 0, &Type_void, "Doc::showmac (str name, IO io = cout, int mode = 0)", f_showmac },

	{ 0, &Type_void, "Doc::mode (char type)", f_mode },
	{ 0, &Type_void, "Doc::cmd (char type, ...)", f_cmd },
	{ 0, &Type_bool, "Doc::plaintex (str cmd = NULL)", f_plaintex },
	{ 0, &Type_void, "Doc::plain (str arg = NULL)", f_plain },
	{ 0, &Type_void, "Doc::comment (str comment)", f_comment },
	{ 0, &Type_void, "Doc::label (int type, str name)", f_label },
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
	{ 0, &Type_str, "Doc::PlainArg (char beg = 0, char end = '\n', bool flag = false)",
		f_plainarg },
	{ 0, &Type_str, "Doc::StdArg (char beg = 0, char end = '\n', bool flag = false)",
		f_stdarg },
	{ 0, &Type_str, "Doc::LongArg (char beg = 0, char end = '\n', bool flag = false)",
		f_longarg },
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
	{ 0, &Type_str, "ParseBlock (IO io, bool mode, str beg = \"beg\", str end = \"end\", str toggle = NULL)", ParseBlock },
	{ 0, &Type_str, "ParseOpt (IO io, bool flag = false)", ParseOpt },
	{ 0, &Type_str, "ParseArg (IO io, bool flag = false)", ParseArg },
};


DocTab_t *GlobalDocTab = NULL;

void SetupDoc (void)
{
	static int setup_done = 0;

	if	(setup_done)	return;

	SetupStd();
	SetupUtil();
	SetupPreproc();
	SetupDataBase();
	SetupTimeSeries();
	SetupRand48();
	SetupRandom();
	SetupMdMat();
	SetupMath();
	SetupMdMath();
	SetupPnom();
	SetupSC();

	AddType(&Type_Doc);
	AddMember(Type_Doc.vtab, var_doc, tabsize(var_doc));
	AddFuncDef(func_doc, tabsize(func_doc));
	GlobalDocTab = DocTab("global");
}
