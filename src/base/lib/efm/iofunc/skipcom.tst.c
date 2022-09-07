#include <EFEU/UnitTest.h>
#include <EFEU/io.h>

#include "utest/in.tst.h"

#define	EXPECT_BUF(a, b) \
	UnitTest_strcheck(__FILE__, __LINE__, "==", sb_nul(a), b, # a, # b)

static const char *skipcom(char *def, StrBuf *cbuf, int flag)
{
	static STRBUF(sbuf, 64);
	IO *io = IN(def);
	int c;

	sb_clear(&sbuf);
	sb_clear(cbuf);

	while ((c = io_skipcom(io, cbuf, flag)) != EOF)
		sb_putc(c, &sbuf);

	return sb_nul(&sbuf);
}
 

TEST_DECL(skipcom)
{
	StrBuf *sb = sb_create(0);

	EXPECT_STR(skipcom("a\\\n \t/*X*/\n", NULL, 0), "a\n");
	EXPECT_STR(skipcom("a/*X*/\n", NULL, 1), "a");
	EXPECT_STR(skipcom("b//X", NULL, 0), "b");
	EXPECT_STR(skipcom("b//X\n", NULL, 0), "b\n");
	EXPECT_STR(skipcom("b//X\\\nY\n", NULL, 0), "b\n");
	EXPECT_STR(skipcom("b//X\n", NULL, 1), "b");

	EXPECT_STR(skipcom("// C1\n// C2\n", sb, 1), "");
	EXPECT_BUF(sb, " C1\n C2");
	EXPECT_STR(skipcom("// C1\n// C2\n", sb, 0), "\n\n");
	EXPECT_BUF(sb, " C2");
	EXPECT_STR(skipcom("// C1\\\n C2\n", sb, 0), "\n");
	EXPECT_BUF(sb, " C1 C2");
	EXPECT_STR(skipcom("A// C1", sb, 0), "A");
	EXPECT_BUF(sb, " C1");

	EXPECT_STR(skipcom("a/*** \tX X \t***/\nb", sb, 0), "a\nb");
	EXPECT_BUF(sb, "X X \t");
	EXPECT_STR(skipcom("a/*** \tX X \t***/\nb", sb, 1), "ab");
	EXPECT_BUF(sb, "X X \t");
	EXPECT_STR(skipcom("a/**\\* \tX X \t***/\nb", sb, 1), "ab");
	EXPECT_BUF(sb, "* \tX X \t");
	EXPECT_STR(skipcom("/* A\n * B\n */\n", sb, 1), "");
	EXPECT_BUF(sb, "A\nB\n");
	EXPECT_STR(skipcom("/*\n * B\n */\n", sb, 0), "\n");
	EXPECT_BUF(sb, "B\n");

	EXPECT_STR(skipcom("a/b\n", sb, 1), "a/b\n");
	EXPECT_BUF(sb, "");

	EXPECT_STR(skipcom("a\\\nb", sb, 0), "ab");
	EXPECT_STR(skipcom("a/*A*/\\\n b", sb, 0), "ab");
	EXPECT_STR(skipcom("a/*A*/\\\\\n b", sb, 0), "a\\\n b");
	EXPECT_BUF(sb, "A");
	sb_destroy(sb);
}

#define	EXPECT_EAT(def, wmark, c)	\
	EXPECT(io_eat(IN(def), wmark) == c)

TEST_DECL(io_eat)
{
	EXPECT_EAT(" \tx", "%s", 'x');
	EXPECT_EAT(" \tx", " ", '\t');
	EXPECT_EAT(" \tx", NULL, ' ');

	EXPECT_EAT(" \t\nx", " \t", '\n');
	EXPECT_EAT(" \t\\\nx", " \t", 'x');

	EXPECT_EAT(" \t/*C*/\nx", " \t", '\n');
	EXPECT_EAT(" \t/*C*/x", " \t", 'x');
	EXPECT_EAT(" \t/*C\n*/x", " \t", 'x');
	EXPECT_EAT(" \t/*C\n*/\n", " \t", '\n');

	EXPECT_EAT(" \t// C\nx", " \t", '\n');
	EXPECT_EAT(" \t// C\\\nx", " \t", EOF);
}

TEST_SUITE
{
	TEST_RUN(skipcom);
	TEST_RUN(io_eat);
	CLEANUP_IN;
}
