#include <EFEU/UnitTest.h>
#include <EFEU/mdmat.h>
#include "utest/out.tst.h"

TEST_SUITE
{
	SetupStd();
	SetupEDB();
	Setup_edb2md();
	TEST_FILE("edb",
		"EDB\t1.0\n"
		"enum { a, b } label\n"
		"int x;\n"
		"int y;\n"
		"@data plain\n"
		"\"a\";1;10\n"
		"\"b\";3;0\n"
		"\"a\";1;20\n"
	);

	EDB *edb;
	edb = edb_fopen(UnitTest_tmp(NULL), "edb");
	mdmat *md = edb2md(edb);
	md_print(io_refer(OUT()), md, "mode=std");
	rd_deref(md);
	rd_deref(edb);
	EXPECT_NOUT(md,
		"##MDMAT   2.0\n"
		"##Type    struct { int x, y }\n"
		"##Locale  C\n"
		"##Rows    label\n"
		"##Columns \n"
		"2 x y\n"
		"a 2 30\n"
		"b 3 0\n"
	);

	edb = edb_fopen(UnitTest_tmp(NULL), "edb");
	edb_fout(edb, UnitTest_tmp("out"), "md=std");
	edb_copy(edb, 0);
	rd_deref(edb);
	EXPECT_CONTENT("out",
		"##MDMAT   2.0\n"
		"##Type    struct { int x, y }\n"
		"##Locale  C\n"
		"##Rows    \n"
		"##Columns label\n"
		"4 a.x a.y b.x b.y\n"
		". 2 30 3 0\n"
	);

	CLEANUP_OUT;
}
