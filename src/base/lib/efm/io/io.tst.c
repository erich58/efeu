#include <EFEU/UnitTest.h>
#include <EFEU/io.h>
#include "utest/data.tst.h"

TEST_DECL(io)
{
	IO *io = io_alloc();
	EXPECT_STR(cstr(rd_ident(io)), "<io>");
	EXPECT_STR(cstr(io_xident(io, NULL)), "<io>");
	EXPECT(io_err(io) == 0);
	EXPECT(io_rewind(io) == EOF);
	EXPECT(io_close(io) == EOF);
}

TEST_SUITE
{
	TEST_RUN(io);
}
