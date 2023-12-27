#include <EFEU/UnitTest.h>
#include <pty.h>

static int pty_open(const char *name)
{
	UnitTest_stdin("pty_in", "");
	UnitTest_stdout("pty_out");

	int master;
	int slave;
	ASSERT(openpty(&master, &slave, NULL, NULL, NULL) == 0);

	TEST_FORK
	{
		char buf[64];
		ssize_t n;
		close(slave);
		FILE *out = fopen(UnitTest_tmp(name), "w");

		while ((n = read(master, buf, sizeof buf)) > 0)
			fwrite(buf, 1, n, out);

		fclose(out);
		close(master);
		TEST_EXIT;
	}

	dup2(slave, 0);
	dup2(slave, 1);
	close(slave);
	return master;
}

static int pty_close(int master)
{
	close(master);
	UnitTest_stdin(NULL, NULL);
	UnitTest_stdout(NULL);
	return CHILD_EXIT(0);
}
