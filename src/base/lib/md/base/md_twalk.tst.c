#include <EFEU/UnitTest.h>
#include <EFEU/mdmat.h>
#include <EFEU/printobj.h>
#include "utest/out.tst.h"

typedef struct {
	int n;
	int lim;
	IO *out;
} PAR;

static void par_init(PAR *par, int lim)
{
	par->n = 0;
	par->lim = lim;
	par->out = OUT();
}

static int visit(EfiType *type, void *data, void *base, void *par)
{
	PAR *p = par;
	p->n++;
	PrintData(p->out, type, data);

	if	(base)
	{
		io_printf(p->out, " : ");
		PrintData(p->out, type, base);
	}

	io_printf(p->out, "\n");
	return p->n == p->lim ? p->n : 0;
}

TEST_SUITE
{
	SetupStd();

	const char *data =
		"##MDMAT 2.0\n"
		"##Type int\n"
		"##Locale C\n"
		"##Rows x\n"
		"##Columns t\n"
		"2 t1 t2 t3 t4 t5\n"
		"x1 11 12 13 14 15\n"
		"x2 21 22 23 24 25\n"
	;
	mdmat *md = md_read(io_cstr(data), NULL);
	PAR par;

	EXPECT(md_twalk(md, NULL, 0, NULL, NULL) == 0);

	par_init(&par, 0);
	EXPECT(md_twalk(md, NULL, 0, visit, &par) == 0);
	EXPECT_OUT(
		"11\n"
		"12\n"
		"13\n"
		"14\n"
		"15\n"
		"21\n"
		"22\n"
		"23\n"
		"24\n"
		"25\n"
	);

	par_init(&par, 0);
	md_time(md, "t");
	EXPECT(md_twalk(md, NULL, 0, visit, &par) == 0);
	EXPECT_OUT(
		"11 : 11\n"
		"12 : 12\n"
		"13 : 13\n"
		"14 : 14\n"
		"15 : 15\n"
		"21 : 21\n"
		"22 : 22\n"
		"23 : 23\n"
		"24 : 24\n"
		"25 : 25\n"
	);

	par_init(&par, 0);
	md_mark(md, "x");
	EXPECT(md_twalk(md, NULL, 0, visit, &par) == 0);
	EXPECT_OUT(
		"11 : 11\n"
		"21 : 21\n"
		"12 : 12\n"
		"22 : 22\n"
		"13 : 13\n"
		"23 : 23\n"
		"14 : 14\n"
		"24 : 24\n"
		"15 : 15\n"
		"25 : 25\n"
	);
	md_mark(md, NULL);

	par_init(&par, 0);
	EXPECT(md_twalk(md, NULL, 2, visit, &par) == 0);
	EXPECT_OUT(
		"11\n"
		"12\n"
		"13 : 11\n"
		"14 : 12\n"
		"15 : 13\n"
		"21\n"
		"22\n"
		"23 : 21\n"
		"24 : 22\n"
		"25 : 23\n"
	);

	par_init(&par, 0);
	EXPECT(md_twalk(md, NULL, -2, visit, &par) == 0);
	EXPECT_OUT(
		"11 : 13\n"
		"12 : 14\n"
		"13 : 15\n"
		"14\n"
		"15\n"
		"21 : 23\n"
		"22 : 24\n"
		"23 : 25\n"
		"24\n"
		"25\n"
	);

	par_init(&par, 4);
	EXPECT(md_twalk(md, NULL, -2, visit, &par) == 4);
	EXPECT_OUT(
		"11 : 13\n"
		"12 : 14\n"
		"13 : 15\n"
		"14\n"
	);

	par_init(&par, 0);
	EXPECT(md_twalk(md, "t=t2", 1, visit, &par) == 0);
	EXPECT_OUT(
		"12 : 11\n"
		"22 : 21\n"
	);

	rd_deref(md);
	CLEANUP_OUT;
}
