#include <EFEU/UnitTest.h>
#include <EFEU/mdcount.h>
#include <EFEU/EfiCount.h>
#include <EFEU/locale.h>
#include "utest/out.tst.h"

TEST_SUITE
{
	SetupStd();
	SetupEfiCount();
	int data = 0;


	EfiObj *obj = ptr2Obj(NULL);
	MdCountPar *par = MdCountPar_create();
	MdCountPar_init(par, obj, "record");

	MdCntObj(par, LvalObj(&Lval_ptr, &Type_int, &data),
		"data", "Record data");
	md_showcnt(OUT(), par);
	EXPECT_OUT(
		"count\tstandard counter\n"
		"data\tRecord data\n"
		"wcount\tweighted count\n"
	);

	mdmat *md = new_mdmat();
	md->sbuf = NewStrPool();
	md->i_name = StrPool_xadd(md->sbuf, "Titel");
	md_ctabinit(md, MdCountList_get(par, "count,data"));
	par->md = rd_refer(md);

	data = 10;
	EXPECT(MdCountPar_count(par) == 1);
	data = 32;
	EXPECT(MdCountPar_count(par) == 1);
	md_print(io_refer(OUT()), par->md, "mode=std");
	EXPECT_NOUT(par->md,
		"##MDMAT   2.0\n"
		"##Title   Titel\n"
		"##Type    struct { int count, data }\n"
		"##Locale  C\n"
		"##Rows    \n"
		"##Columns \n"
		"2 count data\n"
		". 2 42\n"
	);

	rd_deref(par);

	CLEANUP_OUT;
}
