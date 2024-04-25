#include <EFEU/UnitTest.h>
#include <EFEU/object.h>
#include <EFEU/printobj.h>
#include <EFEU/EDB.h>

static const char *test_type(const char *def)
{
	EfiType *type = edb_type(def);
	IO *out = io_fopen(UnitTest_tmp("out"), "w");
	PrintType(out, type, 1);
	io_close(out);
	return UnitTest_content("out");
}

TEST_SUITE
{
	SetupStd();
	SetupEDB();

	EXPECT(edb_type(NULL) == NULL);
	EXPECT_STR(test_type("int\n"), "int");
	EXPECT_STR(test_type("int /* a */ x /* b */"), "struct { int x }");
	EXPECT_STR(test_type("int x\ndouble y\n"),
		"struct { int x; double y }");
	EXPECT_STR(test_type("int x; double y\n"),
		"struct { int x; double y }");
	EXPECT_STR(test_type("struct { int x; double y; }\n"),
		"struct { int x; double y }");
	EXPECT_STR(test_type("int\nextra data\n"), "int");
}
