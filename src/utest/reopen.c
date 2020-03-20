#include "UnitTest.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

static int reopen_fd(int *fdsave, int fd, const char *name)
{
	if	(name == NULL)
	{
		if	(*fdsave != -1)
		{
			dup2(*fdsave, fd);
			close(*fdsave);
			*fdsave = -1;
		}

		return 1;
	}

	int fdnew = creat(name, 0644);

	if	(fdnew < 0)
		return 0;

	if	(*fdsave == -1)
		*fdsave = dup(fd);

	dup2(fdnew, fd);
	return 1;
}

int UnitTest_stdout(const char *name)
{
	static int fdsave = -1;
	fflush(stdout);
	return reopen_fd(&fdsave, 1, name ? UnitTest_tmpname(name) : NULL);
}

int UnitTest_stderr(const char *name)
{
	static int fdsave = -1;
	fflush(stderr);
	return reopen_fd(&fdsave, 2, name ? UnitTest_tmpname(name) : NULL);
}
