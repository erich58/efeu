#include <EFEU/UnitTest_pty.h>

TEST_SUITE
{
	char buf[64] = { 0 };
	int master = pty_open("out");

	UnitTest_write(master, "Input\r");
	EXPECT(read(0, buf, sizeof buf) == 6);
	EXPECT_STR(buf, "Input\n");
	UnitTest_write(1, "Output\n");

	EXPECT(pty_close(master));
	EXPECT_CONTENT("out", "Input\r\nOutput\r\n");
}
