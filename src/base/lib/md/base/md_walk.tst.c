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
		"##Zeilen y\n"
		"##Spalten x\n"
		"2 x1 x2 x3\n"
		"y1 11 12 13\n"
		"y2 21 22 23\n"
	;
	mdmat *md = md_read(io_cstr(data), NULL);
	PAR par;

	EXPECT(md_walk(md, NULL, NULL, NULL, NULL) == 0);

	par_init(&par, 0);
	EXPECT(md_walk(md, NULL, NULL, visit, &par) == 0);
	EXPECT_OUT(
		"11\n"
		"12\n"
		"13\n"
		"21\n"
		"22\n"
		"23\n"
	);

	par_init(&par, 0);
	md_mark(md, "y");
	EXPECT(md_walk(md, NULL, NULL, visit, &par) == 0);
	EXPECT_OUT(
		"11\n"
		"21\n"
		"12\n"
		"22\n"
		"13\n"
		"23\n"
	);
	md_mark(md, NULL);

	par_init(&par, 0);
	EXPECT(md_walk(md, "x=x2", NULL, visit, &par) == 0);
	EXPECT_OUT(
		"12\n"
		"22\n"
	);

	par_init(&par, 2);
	EXPECT(md_walk(md, NULL, NULL, visit, &par) == 2);
	EXPECT_OUT(
		"11\n"
		"12\n"
	);

	par_init(&par, 0);
	EXPECT(md_walk(md, NULL, "x=x1", visit, &par) == 0);
	EXPECT_OUT(
		"11 : 11\n"
		"12 : 11\n"
		"13 : 11\n"
		"21 : 21\n"
		"22 : 21\n"
		"23 : 21\n"
	);

	par_init(&par, 0);
	EXPECT(md_walk(md, NULL, "y=y2", visit, &par) == 0);
	EXPECT_OUT(
		"11 : 21\n"
		"12 : 22\n"
		"13 : 23\n"
		"21 : 21\n"
		"22 : 22\n"
		"23 : 23\n"
	);

	par_init(&par, 0);
	EXPECT(md_walk(md, "x=x2", "x=x1 y=y2", visit, &par) == 0);
	EXPECT_OUT(
		"12 : 21\n"
		"22 : 21\n"
	);

	rd_deref(md);
	CLEANUP_OUT;
}
