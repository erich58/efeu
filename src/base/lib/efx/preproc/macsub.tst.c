#include <EFEU/UnitTest.h>
#include <EFEU/preproc.h>
#include <EFEU/utest/out.tst.h>

static const char *do_macsub(const char *input)
{
	PushMacroTab(NULL);
	IO *in = io_ptrpreproc(io_cstr(input), NULL, NULL);
	io_copy(in, OUT());
	io_close(in);
	rd_deref(PopMacroTab());
	return input;
}

#define	MACSUB(input)	\
UnitTest_strinfo(__FILE__, __LINE__, "INFO", do_macsub(input), "input")

TEST_DECL(noarg_recursiv)
{
	MACSUB(
		"#define A	C\n"
		"#define B	A\n"
		"#define C	B\n"
		"A\n"
		"B\n"
		"C\n"
	);
	EXPECT_OUT(
		"A\n"
		"B\n"
		"C\n"
	);
}

TEST_DECL(noarg_str)
{
	MACSUB(
		"#define A	c\n"
		"#define B	\"A\"\n"
		"#define C	B\n"
		"A\n"
		"B\n"
		"C\n"
	);
	EXPECT_OUT(
		"c\n"
		"\"A\"\n"
		"\"A\"\n"
	);
}

TEST_DECL(noarg_op)
{
	MACSUB(
		"#define A	# X\n"
		"#define B	X ## Y\n"
		"#define C	1 ## 2\n"
		"A\n"
		"B\n"
		"C\n"
	);
	EXPECT_OUT(
		"# X\n"
		"XY\n"
		"12\n"
	);
}

TEST_DECL(arg_op)
{
	MACSUB(
		"#define f(x)	x\n"
		"#define g(x)	# x\n"
		"#define h(x)	x ## 1\n"
		"f(a)\n"
		"g(a)\n"
		"h(a)\n"
	);
	EXPECT_OUT(
		"a\n"
		"\"a\"\n"
		"a1\n"
	);
}

TEST_DECL(arg_catsub)
{
	MACSUB(
		"#define b1	OK\n"
		"#define f(x)	x##1\n"
		"f(a)\n"
		"f(b)\n"
	);
	EXPECT_OUT(
		"a1\n"
		"OK\n"
	);
}

TEST_DECL(expand)
{
	MACSUB(
		"#define a	OK\n"
		"expand: a\n"
		"#noexpand\n"
		"noexpand: a\n"
		"#expand\n"
		"expand: a\n"
		"#undef a\n"
		"undef: a\n"
	);
	EXPECT_OUT(
		"expand: OK\n"
		"noexpand: a\n"
		"expand: OK\n"
		"undef: a\n"
	);
}

TEST_SUITE
{
	TEST_RUN(noarg_recursiv);
	TEST_RUN(noarg_str);
	TEST_RUN(noarg_op);
	TEST_RUN(arg_op);
	TEST_RUN(arg_catsub);
	TEST_RUN(expand);
	CLEANUP_OUT;
}
