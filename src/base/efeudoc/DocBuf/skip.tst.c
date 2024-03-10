
#include <EFEU/UnitTest.h>
#include <EFEU/mstring.h>
#include "DocBuf.h"
#include "utest/in.tst.h"

TEST_SUITE
{
	EXPECT(skip_blank(IN(" \t")) == EOF);
	EXPECT(skip_blank(IN(" \t\nabc")) == '\n');

	EXPECT(skip_space(IN(" \t\n"), NULL) == EOF);
	EXPECT(skip_space(IN(" \t\nabc"), NULL) == 'a');

	STRBUF(buf, 32);
	EXPECT(skip_space(IN(" /* abc */\nx"), &buf) == 'x');
	EXPECT_STR(sb_cstr(&buf), "abc ");
	EXPECT(skip_space(IN(" // abc \nx"), &buf) == 'x');
	EXPECT_STR(sb_cstr(&buf), " abc ");

	IO *in = IN("KEYx");
	EXPECT(test_key(in, "KX") == 0);
	EXPECT(io_peek(in) == 'K');
	EXPECT(test_key(in, "KEY") == 1);
	EXPECT(io_peek(in) == 'x');

	CLEANUP_IN;
}
