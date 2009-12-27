/*	Initialisierung
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include <EFEU/Document.h>
#include <EFEU/DocOut.h>
#include <EFEU/efutil.h>
#include <EFEU/preproc.h>
#include <EFEU/database.h>
#include <EFEU/parsedef.h>
#include <EFEU/mdmat.h>
#include <EFEU/Random.h>
#include <EFEU/cmdeval.h>
#include <Math/TimeSeries.h>
#include <EFEU/stdtype.h>

stack_t *DocMacStack = NULL;

Type_t Type_Merge = REF_TYPE("Merge", Merge_t *);
Type_t Type_InputEnc = REF_TYPE("InputEnc", InputEnc_t *);

typedef void *(*member_t) (void *ptr);

static Obj_t *merge_member (const Var_t *st, const Obj_t *obj)
{
	void **ptr = obj->data;
	return *ptr ? LvalObj(st->type, NULL, ((member_t) st->par) (*ptr)) : NULL;
}


static void *mm_output (Merge_t *merge) { return &merge->out; }
static void *mm_obbeyline (Merge_t *merge) { return &merge->obbeyline; }
static void *mm_obbeyspace (Merge_t *merge) { return &merge->obbeyspace; }

static Var_t var_merge[] = {
	{ "out", &Type_DocOut, NULL, 0, 0, merge_member, mm_output },
	{ "obbeyline", &Type_bool, NULL, 0, 0, merge_member, mm_obbeyline },
	{ "obbeyspace", &Type_bool, NULL, 0, 0, merge_member, mm_obbeyspace },
	{ "cmd_debug", &Type_int, &DocMac_reftype.debug },
	{ "tab_debug", &Type_int, &DocMacTab_reftype.debug },
};

void MergeEval_init (Merge_t *merge)
{
	PushVarTab(RefVarTab(Type_Merge.vtab),
		NewPtrObj(&Type_Merge, rd_refer(merge)));
}

void MergeEval_exit (Merge_t *merge)
{
	PopVarTab();
}

static void f_create (Func_t *func, void *rval, void **arg)
{
	Val_ptr(rval) = Merge_open(rd_refer(Val_ptr(arg[0])));
}

static void f_merge (Func_t *func, void *rval, void **arg)
{
	Merge_t *merge;

	if	((merge = Val_ptr(arg[0])) != NULL)
	{
		io_t *save_in = merge->input;
		merge->input = rd_refer(Val_io(arg[1]));
		Merge_eval(merge);
		io_close(merge->input);
		merge->input = save_in;
	}
}


static void f_cmdlist (Func_t *func, void *rval, void **arg)
{
	DocMacTab_t *tab;
	ObjList_t **lp;
	DocMac_t **cp;
	size_t n;
	
	Val_list(rval) = NULL;
	tab = DocMacStack_tab(Val_str(arg[0]));

	if	(tab == NULL)	return;

	cp = tab->tab.data;
	lp = &Val_list(rval);

	for (n = tab->tab.used; n-- > 0; cp++)
	{
		*lp = NewObjList(str2Obj(mstrcpy((*cp)->name)));
		lp = &(*lp)->next;
	}
}

static void f_tabstack (Func_t *func, void *rval, void **arg)
{
	stack_t *ptr;
	ObjList_t **lp;
	
	Val_list(rval) = NULL;
	lp = &Val_list(rval);

	for (ptr = DocMacStack; ptr != NULL; ptr = ptr->next)
	{
		DocMacTab_t *tab = ptr->data;
		*lp = NewObjList(str2Obj(tab ? mstrcpy(tab->name) : NULL));
		lp = &(*lp)->next;
	}
}


static FuncDef_t stdfunc[] = {
	{ 0, &Type_Merge, "Merge (DocOut out)", f_create },
	{ 0, &Type_void, "Merge::merge (IO in)", f_merge },
	{ 0, &Type_list, "Merge::cmdlist (str name = NULL)", f_cmdlist },
	{ 0, &Type_list, "Merge::tabstack ()", f_tabstack },
};

void SetupMerge (void)
{
	static int setup_done = 0;

	if	(setup_done)	return;

	AddType(&Type_Merge);
	AddVar(Type_Merge.vtab, var_merge, tabsize(var_merge));
	AddFuncDef(stdfunc, tabsize(stdfunc));
}
