
#include <EFEU/UnitTest.h>
#include <EFEU/mstring.h>
#include "DocBuf.h"
#include "utest/in.tst.h"
#include "utest/out.tst.h"

TEST_DECL(token)
{
	copy_token(IN("xyz"), OUT(), ' ', 0);
	EXPECT_OUT(" ");
	copy_token(IN("xyz"), OUT(), ' ', 1);
	EXPECT_OUT("");

	copy_token(IN("ab\\\"c\"xyz"), OUT(), '"', 0);
	EXPECT_OUT("\"ab\\\"c\"");
	copy_token(IN("ab\\'c'xyz"), OUT(), '\'', 0);
	EXPECT_OUT("'ab\\'c'");

	copy_token(IN("abc)xyz"), OUT(), '(', 0);
	EXPECT_OUT("(abc)");
	copy_token(IN("abc]xyz"), OUT(), '[', 0);
	EXPECT_OUT("[abc]");
	copy_token(IN("abc}xyz"), OUT(), '{', 0);
	EXPECT_OUT("{abc}");
}

TEST_DECL(block)
{
	copy_block(IN("abc\n\t(def)ghi)xyz"), OUT(), ')', 0);
	EXPECT_OUT("abc\n\t(def)ghi)");
	copy_block(IN("abc\n\t(def)ghi)xyz"), OUT(), ')', 1);
	EXPECT_OUT("abc\n(def)ghi)");
}

TEST_SUITE
{
	TEST_RUN(token);
	TEST_RUN(block);
	CLEANUP_IN;
	CLEANUP_OUT;
}
