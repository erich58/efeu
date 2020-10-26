#include <EFEU/UnitTest.h>
#include <EFEU/mktype.h>
#include <EFEU/object.h>
#include <EFEU/Resource.h>
#include <stdtype.h>
#include "utest/mstr.tst.h"
#include "utest/out.tst.h"

TEST_DECL(show)
{
	EfiVarTab *tab = VarTab("test", 2);
	EfiVar_list(OUT(), NULL, tab);
	EXPECT_OUT("");
	
	int x = 1;
	EfiVarDef vdef[] = {
		{ "a", &Type_int, &x, "A-Desc" },
		{ "langer_name", &Type_int, &x, "B-Desc" },
		{ "sehr_langer_name", &Type_int, &x, "C-Desc" },
	};
	AddVarDef(tab, vdef, 3);
	EXPECT_MSTR(rd_ident(tab), "test[3]");

	EfiVar_list(OUT(), NULL, tab);
	EXPECT_OUT(
		"a\t\tA-Desc\n"
		"langer_name\tB-Desc\n"
		"sehr_langer_name\n\t\tC-Desc\n"
	);
	ShowVarTab(OUT(), NULL, tab);
	EXPECT_OUT(
		"test = {\n"
		"\ta langer_name sehr_langer_name\n"
		"}"
	);
	ShowVarTab(OUT(), "var", tab);
	EXPECT_OUT(
		"var = {\n"
		"\ta langer_name sehr_langer_name\n"
		"}"
	);
	DumpVarTab(OUT(), NULL, tab);
	MATCH_OUT(
		"test = \\{\n"
		"\tint a.*\n"
		"\tint langer_name.*\n"
		"\tint sehr_langer_name.*\n"
		"\\}\n"
	);

	DelVarTab(tab);
	tab = VarTab(NULL, 2);
	MATCH_MSTR(rd_ident(tab), "0x.*[0]");
	DelVarTab(tab);
}

TEST_DECL(resource)
{
	SetResource("RName", "42");
	EfiObj *obj = ResourceObj(&Type_int, "RName");
	EXPECT(*(int *)(obj->data) == 42);
	*(int *)(obj->data) = 10;
	SyncLval(obj);
	MATCH_MSTR(Obj_ident(obj), "(Resource: RName)");
	EXPECT_MSTR(GetResource("RName", NULL), "10");
	UnrefObj(obj);
}

static void dummy_clean(void *par)
{
}

TEST_DECL(var)
{
	EfiStruct *s;

	s = NewEfiStruct(&Type_int, "x", 0);
	s->clean = dummy_clean;
	EXPECT_MSTR(rd_ident(s), "int x");
	EXPECT(s->refcount == 1);
	RefEfiStruct(s);
	EXPECT(s->refcount == 2);
	DelEfiStruct(s);
	EXPECT(s->refcount == 1);
	DelEfiStruct(s);
}

TEST_DECL(stack)
{
	EfiVarTab *tab = VarTab("test", 0);
	EXPECT(ContextVar == NULL);
	PushContext(tab, NULL);
	EXPECT(ContextVar == tab);
	PopContext();
	EXPECT(ContextVar == NULL);

	EXPECT(LocalVar == GlobalVar);
	PushVarTab(tab, NULL);
	EXPECT(LocalVar == tab);
	EfiVarStack *vstack = SaveVarStack();
	EXPECT(LocalVar == NULL);
	RestoreVarStack(vstack);
	EXPECT(LocalVar == tab);
	PopVarTab();
	EXPECT(LocalVar == GlobalVar);
}

extern EfiType Type_StrPool;

TEST_DECL(strpool)
{
	StrPool *pool = NewStrPool();
	size_t idx = StrPool_add(pool, "data");
	EXPECT(idx == 8);
	EXPECT(StrPool_offset(pool) == 13);

	EfiObj *base = NewObj(&Type_StrPool, &pool);
	EfiObj *obj = StrPoolObj(base, pool, &idx);
	MATCH_MSTR(Obj_ident(obj), "str &StrPoolObj");
	EXPECT_STR(*((char **) obj->data), "data");
	const char *s = "abc";
	CopyData(&Type_str, obj->data, &s);
	EXPECT_STR(*((char **) obj->data), "abc");
	SyncLval(obj);
	EXPECT(idx == 13);
	EXPECT(StrPool_offset(pool) == 17);
	EXPECT_STR(StrPool_get(pool, 13), "abc");
	UnrefObj(obj);
}


TEST_DECL(addvar)
{
	EfiVarTab *tab;
	VarTabEntry *entry;

	tab = VarTab("test", 2);
	entry = VarTab_next(tab);
	entry->name = "x";
	entry->obj = int2Obj(10);
	entry = VarTab_next(tab);
	entry->name = "a";
	entry->obj = int2Obj(1);

	ShowVarTab(OUT(), NULL, tab);
	EXPECT_OUT("test = {\n\tx a\n}");

	VarTab_qsort(tab);
	ShowVarTab(OUT(), NULL, tab);
	EXPECT_OUT("test = {\n\ta x\n}");

	EXPECT(Val_int(VarTab_get(tab, "a")->obj->data) == 1);
	EXPECT(Val_int(VarTab_get(tab, "x")->obj->data) == 10);
	EXPECT(Val_int(GetVar(tab, "x", NULL)->data) == 10);
	EXPECT(Val_int(GetVar(NULL, "this", int2Obj(42))->data) == 42);

	EXPECT(VarTab_xget(tab, "z") != NULL);
	ShowVarTab(OUT(), NULL, tab);
	EXPECT_OUT("test = {\n\ta x z\n}");
	VarTab_del(tab, "a");
	ShowVarTab(OUT(), NULL, tab);
	EXPECT_OUT("test = {\n\tx z\n}");

	EXPECT(VarTab_get(NULL, "x") == NULL);
	EXPECT(GetVar(NULL, "x", NULL) == NULL);

	PushVarTab(RefVarTab(tab), NULL);
	EXPECT(VarTab_get(NULL, "x") != NULL);
	EXPECT(GetVar(NULL, "x", NULL) != NULL);
	PushVarTab(NULL, NULL);
	EXPECT(VarTab_get(NULL, "x") != NULL);
	EXPECT(GetVar(NULL, "x", NULL) != NULL);
	PopVarTab();
	PopVarTab();

	EfiStruct *s = NewEfiStruct(&Type_int, "b", 0);
	AddStruct(tab, s, 1);
	ShowVarTab(OUT(), NULL, tab);
	EXPECT_OUT("test = {\n\tb x z\n}");
	UnitTest_stderr("error");
	AddStruct(tab, s, 1);
	MATCH_CONTENT("error", "ATTENTION");

	VarTab_xadd(tab, "c", NULL, int2Obj(23));
	ShowVarTab(OUT(), NULL, tab);
	EXPECT_OUT("test = {\n\tb c x z\n}");
	UnitTest_stderr("error");
	VarTab_xadd(tab, "c", NULL, int2Obj(23));
	MATCH_CONTENT("error", "ATTENTION");

	DelVarTab(tab);
}

TEST_SUITE
{
	SetupStd();

	TEST_RUN(show);
	TEST_RUN(resource);
	TEST_RUN(var);
	TEST_RUN(stack);
	TEST_RUN(strpool);
	TEST_RUN(addvar);
	CLEANUP_MSTR;
	CLEANUP_OUT;
}
