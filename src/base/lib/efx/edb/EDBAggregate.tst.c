#include <EFEU/UnitTest.h>
#include <EFEU/object.h>
#include <EFEU/EDB.h>
#include "utest/eval.tst.h"

TEST_DECL(func)
{
	EfiType *src, *tg;
	EfiStruct *s, *t;
	EDBAggregate *aggr = NULL;
	TEST_EXPR(streval("struct SRC { str label; int a; double b; };"));
	TEST_EXPR(streval("struct TG { double b; int a; };"));
	EXPECT_PTR(src = GetType("SRC"));
	EXPECT_PTR(tg = GetType("TG"));

	EXPECT_PTR(s = src->list->next);
	EXPECT_PTR(t = tg->list->next);
	EXPECT_PTR(aggr = NewEDBAggregate(NULL, "+=", t, s->offset));
	rd_deref(aggr);
	EXPECT_PTR(aggr = NewEDBAggregate(NULL, "+=", t, s->offset));

	EXPECT_PTR(s = src->list->next->next);
	EXPECT_PTR(t = tg->list);
	EXPECT_PTR(aggr = NewEDBAggregate(aggr, "+=", t, s->offset));

	EDBAggregateFunc(aggr, "foo", tg, src);
	EXPECT_EVAL("TG::foo", "(void) virtual TG & TG::foo (SRC)\n");
	EXPECT_EVAL("SRC src", "{NULL, 0, 0.}");
	EXPECT_EVAL("TG tg", "{0., 0}");
	EXPECT_EVAL("src = { \"a\", 1, 5.}", "{\"a\", 1, 5.0000}");
	EXPECT_EVAL("tg.foo(src)", "{5.0000, 1}");
	EXPECT_EVAL("tg.foo(src)", "{10.000, 2}");
}

TEST_SUITE
{
	SetupStd();
	TEST_EXEC(func);
	CLEANUP_EVAL;
}

