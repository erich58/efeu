
#include <EFEU/UnitTest.h>
#include <EFEU/mdmat.h>

TEST_SUITE
{
	SetupStd();
	SetupEDB();
	EXPECT(md2edb(NULL) == NULL);

	const char *data = 
		"##MDMAT   2.0\n"
		"##Title    Data\n"
		"##Type    int\n"
		"##Locale  C\n"
		"##Rows    r\n"
		"##Columns c\n"
		"2 x y\n"
		"a 1 2\n"
		"b 3 0\n"
	;

	mdmat *md = md_read(io_cstr(data), NULL);
	EDB *edb = md2edb(md);
	rd_deref(md);
	edb_fout(edb, UnitTest_tmp("edb"), "plain");
	edb_copy(edb, 0);
	rd_deref(edb);
	EXPECT_CONTENT("edb",
		"EDB\t1.0\n"
		"// Data\n"
		"enum {\n"
		"\ta = 1,\n"
		"\tb = 2,\n"
		"} r;\n"
		"enum {\n"
		"\tx = 1,\n"
		"\ty = 2,\n"
		"} c;\n"
		"int data;\n"
		"@data plain\n"
		"#r;c;data\n"
		"1;1;1\n"
		"1;2;2\n"
		"2;1;3\n"
		"2;2;0\n"
	);
}
