#include <EFEU/UnitTest.h>
#include <EFEU/mdmat.h>
#include "utest/out.tst.h"

TEST_SUITE
{
	SetupStd();

	const char *data =
		"##MDMAT 2.0\n"
		"##Type struct { int a; int b }\n"
		"##Locale us\n"
		"##Zeilen y\n"
		"##Spalten x\n"
		"2 x.a x.b\n"
		"y1 1 2\n"
		"y2 3 4\n"
	;
	mdmat *md = md_read(io_cstr(data), NULL);
	md_print(io_refer(OUT()), md, "mode=data");
	EXPECT_NOUT(md,
		"1, 2\n"
		"3, 4\n"
	);
	/*
	md_print(io_refer(OUT()), md, "mode=tab");
	EXPECT_NOUT(md,
		"1, 2\n"
		"3, 4\n"
	);
	*/
	CLEANUP_OUT;
}
