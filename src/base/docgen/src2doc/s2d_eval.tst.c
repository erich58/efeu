#include <EFEU/UnitTest.h>
#include <EFEU/object.h>
#include "src2doc.h"

TEST_SUITE
{
	SetupStd();
	InsertCode = 1;
	TEST_FILE("input",
		"/*\n"
		"Testfile $1\n"
		"(c) 2024 Erich Frühstück\n"
		"*/\n"
		"#include <some_file>\n"
		"/*\n"
		"$Description\n"
		"Command Description\n"
		"*/\n"
		"int theAnswer()\n"
		"{\n"
		"\treturn 42;\n"
		"}\n"
	);

	IO *in = io_fileopen(UnitTest_tmp("input"), "r");
	IO *out = io_fileopen(UnitTest_tmp("output"), "w");

	/*
	SrcData data;
	data.doc.var[VAR_NAME] = mstrcpy("Name");
	SrcData_init(&data, in);
	SrcData_eval(&data, "Name");
	SrcData_write(&data, out);
	*/
	s2d_src("tst.c", in, out);
	io_close(in);
	io_close(out);
	PRINT_CONTENT("output");
}
