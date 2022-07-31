#include <EFEU/UnitTest.h>
#include <EFEU/strbuf.h>
#include <EFEU/LogConfig.h>
#include <fcntl.h>
#include <unistd.h>

TEST_DECL(to_int)
{
	static STRBUF(buf, 32);
	EXPECT_STR(sb_setint(NULL, 1), NULL);
	EXPECT_STR(sb_setint(&buf, 42), "42");
	EXPECT(sb_getint(&buf) == 42);
}

TEST_SUITE
{
	TEST_RUN(to_int);
}
