#include <EFEU/UnitTest.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

FILE *UnitTest_strout(char *buf, size_t size)
{
	int fd = open("/dev/null", O_WRONLY);
	FILE *file = fdopen(fd, "w");
	close(fd);

	if	(size)
	{
		memset(buf, 0, size);
		setvbuf(file, buf, _IOFBF, size - 1);
	}
	else
	{
		setvbuf(file, NULL, _IONBF, 0);
	}

	return file;
}

FILE *UnitTest_strin(const char *buf, size_t size)
{
	FILE *file = tmpfile();

	if	(size)
		fwrite(buf, 1, size, file);
	else	
		fputs(buf, file);

	rewind(file);
	return file;
}
