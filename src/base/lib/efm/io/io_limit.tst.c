#include <EFEU/io.h>
#include <EFEU/utest/out.tst.h>

TEST_SUITE
{
	IO *io = io_limit(rd_refer(OUT()), 4);
	EXPECT_INT(io_rewind(io), EOF);
	EXPECT_INT(io_puts("12", io), 2);
	EXPECT_INT(io_puts("3456", io), 2);
	EXPECT_OUT("1234");
	io_close(io);
	CLEANUP_OUT;
}
