#include <EFEU/UnitTest.h>
#include <EFEU/preproc.h>

TEST_DECL(preproc)
{
	IO *in;
	IO *out;
	char *path = mstrcpy(UnitTest_tmpname(NULL));

	TEST_FILE("input",
		"#define a	y\n"
		"#define _STR(z)	# z\n"
		"#define _CPY(z)	z\n"
		"-_STR(\"a\" \"b\" \"c\")-\n"
		"-_STR( a b c )-\n"
		"-_CPY( \"a # b\" )-\n"
	);

	EXPECT_PTR(in = io_fopen(UnitTest_tmpname("input"), "r"));
	EXPECT_PTR(out = io_fopen(UnitTest_tmpname("output"), "w"));
	EXPECT_PTR(in = io_ptrpreproc(in, &path, NULL));
	io_copy(in, out);
	io_close(in);
	io_close(out);
	MATCH_CONTENT("output", ".");
	memfree(path);
}

TEST_SUITE
{
	TEST_RUN(preproc);
}
