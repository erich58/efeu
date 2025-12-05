#include <EFEU/UnitTest.h>
#include <Math/TimeSeries.h>
#include <EFEU/utest/out.tst.h>

static const char *ts_out(TimeSeries *ts)
{
	ts_print(OUT(), ts, "%.2f");
	rd_deref(ts);
	return utest_out();
}

#define	EXPECT_TS(ts, s)	\
UnitTest_diff(__FILE__, __LINE__, ts_out(ts), s, # ts, # s)

TEST_DECL(setup)
{
	SetupStd();
	SetupMdMat();
	SetupMath();
	SetProgName("ID");
	SetApplPath(UnitTest_tmp(NULL));
	TEST_FILE("TimeSeries.msg",
		"21	$!: function for time series is not defined.\n"
		"22	$!: right argument is not defined.\n"
		"23	$!: time series not compatible.\n"
		"24	$!: operator not defined.\n"
	);
}

TEST_DECL(func)
{
	EfiPolyFunc *func = GetGlobalFunc("exp");
	EXPECT(func != NULL);

	TimeSeries *base = ts_create("base", tindex_create(TS_INDEX, 0), 0);	
	ts_append(base, 2, 0., 1.);

	EXPECT(ts_func(NULL, NULL) == NULL);
	UnitTest_stderr("stderr");
	EXPECT(ts_func(NULL, base) == NULL);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr",
		"MSG ID: function for time series is not defined.\n");

	EXPECT_TS(ts_func(func, base),
		"TimeSeries(NULL, \"#0\",\n"
		"\t1.00, 2.72\n"
		")\n"
	);
	EXPECT_TS(base,
		"TimeSeries(\"base\", \"#0\",\n"
		"\t0.00, 1.00\n"
		")\n"
	);
}

TEST_DECL(assign)
{
	EfiPolyFunc *func = GetTypeFunc(&Type_double, "+=");
	EXPECT(func != NULL);
	EfiObj *i1 = int2Obj(1);
	EfiObj *d1 = double2Obj(1.);
	TimeSeries *ts = NULL;

	TEST_EXPR(ts_assign(NULL, NULL, NULL, NULL));

	TimeSeries *base = ts_create("base", tindex_create(TS_INDEX, 10), 5);	

	TEST_EXPR(ts_assign(NULL, base, i1->type, i1->data));
	UnitTest_stderr("stderr");
	TEST_EXPR(ts_assign(NULL, base, d1->type, d1->data));
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr",
		"MSG ID: operator not defined.\n");

	UnitTest_stderr("stderr");
	TEST_EXPR(ts = NULL);
	TEST_EXPR(ts_assign(NULL, base, &Type_TimeSeries, &ts));
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr",
		"MSG ID: right argument is not defined.\n");

	UnitTest_stderr("stderr");
	TEST_EXPR(ts = ts_create(NULL, tindex_create(TS_YEAR, 2000), 0));
	TEST_EXPR(ts_assign(NULL, base, &Type_TimeSeries, &ts));
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr",
		"MSG ID: time series not compatible.\n");

	EXPECT_TS(rd_refer(base),
		"TimeSeries(\"base\", \"#10\",\n"
		"\t0.00, 0.00, 0.00, 0.00, 0.00\n"
		")\n"
	);
	TEST_EXPR(ts_assign(func, base, d1->type, d1->data));
	EXPECT_TS(rd_refer(base),
		"TimeSeries(\"base\", \"#10\",\n"
		"\t1.00, 1.00, 1.00, 1.00, 1.00\n"
		")\n"
	);

	TEST_EXPR(ts_resize(ts, 5, 1.));
	TEST_EXPR(ts->base = tindex_create(TS_INDEX, 8));
	TEST_EXPR(ts_assign(func, base, &Type_TimeSeries, &ts));
	EXPECT_TS(rd_refer(base),
		"TimeSeries(\"base\", \"#10\",\n"
		"\t2.00, 2.00, 2.00, 1.00, 1.00\n"
		")\n"
	);
	TEST_EXPR(ts->base = tindex_create(TS_INDEX, 12));
	TEST_EXPR(ts_assign(func, base, &Type_TimeSeries, &ts));
	EXPECT_TS(base,
		"TimeSeries(\"base\", \"#10\",\n"
		"\t2.00, 2.00, 3.00, 2.00, 2.00\n"
		")\n"
	);
}

TEST_DECL(term)
{
	EfiPolyFunc *func = GetGlobalFunc("+");
	EXPECT(func != NULL);
	EfiObj *i1 = int2Obj(1);
	EfiObj *d1 = double2Obj(1.);
	EfiObj *d2 = double2Obj(0.2);
	TimeSeries *a = NULL;
	TimeSeries *b = NULL;
	EXPECT(a == NULL);
	EXPECT(b == NULL);
	EXPECT(ts_term(NULL, i1->type, i1->data, NULL, 0) == NULL); 
	EXPECT(ts_term(NULL, d1->type, d1->data, NULL, 0) == NULL); 
	EXPECT(ts_term(NULL, &Type_TimeSeries, &a, NULL, 0) == NULL); 

	UnitTest_stderr("stderr");
	EXPECT(ts_term(func, d1->type, d1->data, &Type_TimeSeries, &b) == NULL); 
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr",
		"MSG ID: right argument is not defined.\n");

	UnitTest_stderr("stderr");
	EXPECT(ts_term(NULL, d1->type, d1->data, d2->type, d2->data) == NULL); 
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr",
		"MSG ID: operator not defined.\n");

	EXPECT_TS(ts_term(func, d1->type, d1->data, d2->type, d2->data),
		"TimeSeries(NULL, \"#0\",\n"
		"\t1.20\n"
		")\n"
	);

	TEST_EXPR(a = ts_create("a", tindex_create(TS_INDEX, 10), 0));
	TEST_EXPR(ts_resize(a, 5, 1.));
	TEST_EXPR(b = ts_create("a", tindex_create(TS_YEAR, 10), 0));
	TEST_EXPR(ts_resize(b, 5, 0.2));

	UnitTest_stderr("stderr");
	EXPECT(ts_term(func, &Type_TimeSeries, &a, &Type_TimeSeries, &b) == NULL); 
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr",
		"MSG ID: time series not compatible.\n");
	TEST_EXPR(b->base = tindex_create(TS_INDEX, 0));
	EXPECT_TS(ts_term(func, &Type_TimeSeries, &a, &Type_TimeSeries, &b),
		"TimeSeries(NULL, \"#10\")\n"
	);
	TEST_EXPR(b->base = tindex_create(TS_INDEX, 20));
	EXPECT_TS(ts_term(func, &Type_TimeSeries, &a, &Type_TimeSeries, &b),
		"TimeSeries(NULL, \"#20\")\n"
	);
	TEST_EXPR(b->base = tindex_create(TS_INDEX, 12));
	EXPECT_TS(ts_term(func, &Type_TimeSeries, &a, &Type_TimeSeries, &b),
		"TimeSeries(NULL, \"#12\",\n"
		"\t1.20, 1.20, 1.20\n"
		")\n"
	);

	EXPECT_TS(ts_term(func, &Type_TimeSeries, &a, d2->type, d2->data),
		"TimeSeries(NULL, \"#10\",\n"
		"\t1.20, 1.20, 1.20, 1.20, 1.20\n"
		")\n"
	);

	EXPECT_TS(ts_term(func, d1->type, d1->data, &Type_TimeSeries, &b),
		"TimeSeries(NULL, \"#12\",\n"
		"\t1.20, 1.20, 1.20, 1.20, 1.20\n"
		")\n"
	);
}

TEST_SUITE
{
	TEST_RUN(setup);
	TEST_RUN(func);
	TEST_RUN(assign);
	TEST_RUN(term);
	CLEANUP_OUT;
}
