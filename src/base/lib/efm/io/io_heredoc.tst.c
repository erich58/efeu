#include <EFEU/io.h>
#include <EFEU/utest/out.tst.h>

TEST_SUITE
{
	IO *io = io_heredoc(rd_refer(OUT()), ".");
	EXPECT_INT(io_rewind(io), EOF);
	EXPECT_INT(io_puts("abc\nxyz\n", io), 8);
	EXPECT_INT(io_puts("..\n", io), 3);
	EXPECT_INT(io_puts(".\nabc\n", io), 2);
	EXPECT_OUT("abc\nxyz\n..\n");
	io_close(io);
	CLEANUP_OUT;
}
