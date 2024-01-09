#include <EFEU/UnitTest.h>
#include <EFEU/mdmat.h>
#include "utest/out.tst.h"

TEST_DECL(null)
{
	EXPECT(edb2md(NULL) == NULL);
}

TEST_DECL(print)
{
	TEST_FILE("edb",
		"EDB\t1.0\n"
		"int x\n"
		"@data plain\n"
		"1\n"
		"2\n"
		"3\n"
	);

	EDB *edb = edb_fopen(UnitTest_tmp(NULL), "edb");
	edb_fout(edb, UnitTest_tmp("out"), "md=std");
	edb_copy(edb, 0);
	rd_deref(edb);

	EXPECT_CONTENT("out",
		"##MDMAT   2.0\n"
		"##Type    int\n"
		"##Locale  C\n"
		"##Rows    \n"
		"##Columns \n"
		"1 .\n"
		". 6\n"
	);

	edb = edb_fopen(UnitTest_tmp(NULL), "edb");
	edb_fout(edb, UnitTest_tmp("binary"), "md");
	edb_copy(edb, 0);
	rd_deref(edb);
	system(UnitTest_tpl("od -c -w8 @/binary > @/dump"));

	EXPECT_CONTENT("dump",
		"0000000 357 323  \\0  \\0  \\b  \\0  \\0 004\n"
		"0000010   i   n   t  \\0  \\0  \\0  \\0 006\n"
		"0000020   *   E   O   F\n"
		"0000024\n"
	);
}


TEST_DECL(simple)
{
	TEST_FILE("edb",
		"EDB\t1.0\n"
		"int\n"
		"@data plain\n"
		"1\n"
		"2\n"
		"3\n"
	);

	EDB *edb = edb_fopen(UnitTest_tmp(NULL), "edb");
	mdmat *md = edb2md(edb);
	md_print(io_refer(OUT()), md, "mode=std");
	rd_deref(md);
	rd_deref(edb);
	EXPECT_NOUT(md,
		"##MDMAT   2.0\n"
		"##Type    int\n"
		"##Locale  C\n"
		"##Rows    \n"
		"##Columns \n"
		"1 .\n"
		". 6\n"
	);
}

TEST_DECL(mixed)
{
	TEST_FILE("edb",
		"EDB\t1.0\n"
		"enum { a, b } label\n"
		"int x[2];\n"
		"struct { int y; int z; } s\n"
		"@data plain\n"
		"\"a\";{1,1};{10,1}\n"
		"\"b\";{2,3};{0,1}\n"
		"\"a\";{1,0};{20,4}\n"
	);

	EDB *edb = edb_fopen(UnitTest_tmp(NULL), "edb");
	mdmat *md = edb2md(edb);
	rd_deref(edb);
	md_print(io_refer(OUT()), md, "mode=std");
	rd_deref(md);
	EXPECT_NOUT(md,
		"##MDMAT   2.0\n"
		"##Type    struct { int x[2]; struct { int y, z } s }\n"
		"##Locale  C\n"
		"##Rows    label\n"
		"##Columns \n"
		"4 x[0] x[1] s.y s.z\n"
		"a 2 1 30 5\n"
		"b 2 3 0 1\n"
	);
}

TEST_DECL(label)
{
	TEST_FILE("edb",
		"EDB\t1.0\n"
		"enum { b = 1, a } a\n"
		"enum { x = 5, y = 2 } x\n"
		"@data plain\n"
		"\"a\";5\n"
		"\"b\";5\n"
		"\"a\";2\n"
		"\"b\";5\n"
		"\"b\";5\n"
		"\"a\";2\n"
	);

	EDB *edb = edb_fopen(UnitTest_tmp(NULL), "edb");
	mdmat *md = edb2md(edb);
	rd_deref(edb);
	md_print(io_refer(OUT()), md, "mode=std");
	rd_deref(md);
	EXPECT_NOUT(md,
		"##MDMAT   2.0\n"
		"##Type    unsigned\n"
		"##Locale  C\n"
		"##Rows    a x\n"
		"##Columns \n"
		"1 .\n"
		"b.y 0\n"
		"b.x 3\n"
		"a.y 2\n"
		"a.x 1\n"
	);
}

TEST_SUITE
{
	SetupStd();
	SetupEDB();
	Setup_edb2md();

	TEST_RUN(null);
	TEST_RUN(print);
	TEST_RUN(simple);
	TEST_RUN(label);
	TEST_RUN(mixed);

	CLEANUP_OUT;
}
