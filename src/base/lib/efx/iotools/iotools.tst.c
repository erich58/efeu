#include <EFEU/UnitTest.h>
#include <EFEU/efio.h>
#include <EFEU/ioctrl.h>
#include "utest/in.tst.h"
#include "utest/out.tst.h"

TEST_DECL(io_skip)
{
	IO *in = IN("abc:xyz");
	EXPECT(io_skip(in, ":;") == ':');
	EXPECT(io_copy(in, OUT()) == 4);
	EXPECT_OUT(":xyz");
}

TEST_DECL(io_mputs)
{
	EXPECT(io_mputs(NULL, OUT(), "';") == 0);
	EXPECT_OUT("");
	EXPECT(io_mputs("abc'def'g;h", OUT(), "';") == 14);
	EXPECT_OUT("abc\\'def\\'g\\;h");
}

TEST_DECL(iocpy_skip)
{
	IO *in;
	EXPECT_PTR(in = IN("abc:xyz"));
	EXPECT(iocpy_skip(in, NULL, 0, ":", 0) == 0);
	EXPECT(io_copy(in, OUT()) == 3);
	EXPECT_OUT("xyz");
	EXPECT_PTR(in = IN("abc:xyz"));
	EXPECT(iocpy_skip(in, NULL, 0, ":", 1) == 0);
	EXPECT(io_copy(in, OUT()) == 4);
	EXPECT_OUT(":xyz");

	EXPECT_PTR(in = IN("/x//abc\nx/*y*/z"));
	EXPECT(io_getc(in) == '/');
	EXPECT(iocpy_cskip(in, NULL, 0, NULL, 0) == EOF);
	EXPECT(io_getc(in) == 'x');
	EXPECT(io_getc(in) == '/');
	EXPECT(iocpy_cskip(in, NULL, 0, NULL, 1) == 0);
	EXPECT(io_getc(in) == '\n');
	EXPECT(io_getc(in) == 'x');
	EXPECT(io_getc(in) == '/');
	EXPECT(iocpy_cskip(in, NULL, 0, NULL, 0) == 0);
	EXPECT(io_getc(in) == 'z');
}

TEST_DECL(io_split)
{
	char **data;
	EXPECT(io_split(IN("abc;d\\;ef\nxyz\n"), ";\n", &data) == 3);
	EXPECT_STR(data[0], "abc");
	EXPECT_STR(data[1], "d;ef");
	EXPECT_STR(data[2], "xyz");
	memfree(data);
	EXPECT(io_split(IN("anything"), ";\n", NULL) == 0);
}

TEST_DECL(savestr)
{
	IO *tmp = io_tmpbuf(64);
	EXPECT(io_savestr(tmp, "abc") == 4);
	EXPECT(io_savestr(tmp, NULL) == 0);
	EXPECT(io_savestr(tmp, "z") == 2);
	EXPECT(io_rewind(tmp) == 0);
	char buf[8];
	char *p = buf;
	EXPECT_STR(io_loadstr(tmp, &p), "abc");
	EXPECT_STR(io_loadstr(tmp, &p), NULL);
	EXPECT_STR(io_loadstr(tmp, &p), "z");
	io_close(tmp);
	EXPECT(memcmp(buf, "abc\000z\000", 6) == 0);
}

TEST_DECL(cp_str)
{
	IO *in;
	EXPECT_PTR(in = IN("'abc''x'"));
	EXPECT(io_getc(in) == '\'');
	EXPECT(iocpy_str(in, OUT(), '\'', "'", 0) == 3);
	EXPECT_OUT("abc");
	EXPECT(io_getc(in) == '\'');
	EXPECT(io_getc(in) == 'x');

	EXPECT_PTR(in = IN("'abc''x'"));
	EXPECT(io_getc(in) == '\'');
	EXPECT(iocpy_str(in, OUT(), '\'', "'", 1) == 5);
	EXPECT_OUT("'abc'");
	EXPECT(io_getc(in) == '\'');
	EXPECT(io_getc(in) == 'x');

	EXPECT_PTR(in = IN("'abc''x'"));
	EXPECT(io_getc(in) == '\'');
	EXPECT(iocpy_str(in, OUT(), '\'', "'", 2) == 5);
	EXPECT_OUT("abc'x");
	EXPECT(io_getc(in) == EOF);

	EXPECT_PTR(in = IN("ab\\'c"));
	EXPECT(iocpy_str(in, OUT(), '\'', "'", 1) == 6);
	EXPECT_OUT("'ab\\'c");
	EXPECT(io_getc(in) == EOF);
}

TEST_DECL(cp_util)
{
	EXPECT(iocpy_repl(NULL, OUT(), 0, "repl", 0) == 4);
	EXPECT_OUT("repl");
	EXPECT(iocpy_mark(NULL, OUT(), 'x', "mark", 0) == 5);
	EXPECT_OUT("markx");
	EXPECT(iocpy_name(IN("ame_42+rest"), OUT(), 'N', "%n_", 0) == 7); 
	EXPECT_OUT("Name_42");

	EXPECT(iocpy_esc(IN("x"), NULL, '\\', "\\;", 0) == EOF); 
	EXPECT(iocpy_esc(IN(";"), OUT(), '\\', "\\;", 0) == 1); 
	EXPECT_OUT(";");
	EXPECT(iocpy_esc(IN(";"), OUT(), '\\', "\\;", 1) == 2); 
	EXPECT_OUT("\\;");

	EXPECT(iocpy_xstr(IN("ab\\;x;y"), OUT(), 0, ";", 0) == 4); 
	EXPECT_OUT("ab;x");
}

TEST_DECL(crfilter)
{
	IO *in = io_crfilter(io_string("ab\rc\r\nxyz\r\r\n", NULL));
	EXPECT(io_copy(in, OUT()) == 8);
	EXPECT_OUT("abc\nxyz\n");
	EXPECT(io_ctrl(in, IO_REWIND) == 0);
	EXPECT(io_ctrl(in, IO_FLUSH) == EOF);
	io_close(in);
}

TEST_DECL(getstring)
{
	EXPECT_STR(getstring(IN(" \t\n")), NULL);
	EXPECT_STR(getstring(IN(" (abc)x")), "abc");
	EXPECT_STR(getstring(IN(" {abc}x")), "abc");
	EXPECT_STR(getstring(IN(" [abc]x")), "abc");
	EXPECT_STR(getstring(IN(" <abc>x")), "abc");
	EXPECT_STR(getstring(IN(" `abc'x")), "abc");
	EXPECT_STR(getstring(IN(" 'abc'x")), "abc");
	EXPECT_STR(getstring(IN(" \"abc\"x")), "abc");
	EXPECT_STR(getstring(IN(" !abc!x")), "abc");
	EXPECT_STR(getstring(IN(" /abc/x")), "abc");
	EXPECT_STR(getstring(IN(" abc\nx")), "abc");
	EXPECT_STR(getstring(IN(" !\nabc!\n!x")), "abc!\n");
	EXPECT_STR(getstring(IN(" !\n\\abc\n\\!x\n!y")), "\\abc\n!x\n");
}

TEST_DECL(textool)
{
	EXPECT(TeXputs(NULL, OUT()) == 0);
	EXPECT(TeXputs("%$#_&{}", OUT()) == 14);
	EXPECT_OUT("\\%\\$\\#\\_\\&\\{\\}");
	EXPECT(TeXputs("x^~x", OUT()) == 8);
	EXPECT_OUT("x\\^~\\~~x");
	EXPECT(TeXputs("< | >", OUT()) == 11);
	EXPECT_OUT("$<$ $|$ $>$");
	EXPECT(TeXputs("\" \\ - *", OUT()) == 40);
	EXPECT_OUT("{\\tt\\symbol{34}} $\\backslash$ {-} $\\ast$");
	EXPECT(TeXputs("\t\n\001 \277 x", OUT()) == 23);
	EXPECT_OUT("\t\n$\\bullet$ $\\bullet$ x");
	EXPECT(TeXquote("abc", OUT()) == 7);
	EXPECT_OUT("\"`abc\"'");
}


TEST_SUITE
{
	TEST_RUN(io_skip);
	TEST_RUN(io_mputs);
	TEST_RUN(iocpy_skip);
	TEST_RUN(io_split);
	TEST_RUN(savestr);
	TEST_RUN(cp_str);
	TEST_RUN(cp_util);
	TEST_RUN(crfilter);
	TEST_RUN(getstring);
	TEST_RUN(textool);
	CLEANUP_IN;
	CLEANUP_OUT;
}
