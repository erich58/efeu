#include <EFEU/UnitTest.h>
#include <EFEU/object.h>
#include <EFEU/printobj.h>
#include <EFEU/EDB.h>

static const char *print_type(EfiType *type)
{
	IO *out = io_fopen(UnitTest_tmp("out"), "w");
	PrintType(out, type, 1);
	io_close(out);
	return UnitTest_content("out");
}

#define	EXPECT_TYPE(expr, res)	\
	UnitTest_strcheck(__FILE__, __LINE__, \
	"==", print_type(expr), res, # expr , # res)

TEST_SUITE
{
	SetupStd();

	EXPECT_TYPE(str2Type(NULL), ".");
	EXPECT_TYPE(str2Type("int\n"), "int");
	EXPECT_TYPE(str2Type("int /* a */ x /* b */"),
		"struct { int x }");
	EXPECT_TYPE(str2Type("int x\ndouble y\n"),
		"struct { int x; double y }");
	EXPECT_TYPE(str2Type("int x; double y\n"),
		"struct { int x; double y }");
	EXPECT_TYPE(str2Type("struct { int x; double y; }\n"),
		"struct { int x; double y }");
	EXPECT_TYPE(str2Type("int\nextra data\n"), "int");
}
