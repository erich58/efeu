#include <EFEU/UnitTest.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#define	BSIZE	64

FILE *UnitTest_file(UnitTestIO *io)
{
	if	(!io->file)
	{
		io->file = tmpfile();
		setvbuf(io->file, NULL, _IONBF, 0);
	}

	return io->file;
}

int UnitTest_fd(UnitTestIO *io)
{
	return fileno(UnitTest_file(io));
}

int UnitTest_open(UnitTestIO *io)
{
	return dup(UnitTest_fd(io));
}

FILE *UnitTest_fopen(UnitTestIO *io, const char *mode)
{
	switch (*mode)
	{
	case 'r': UnitTest_rewind(io); break;
	case 'w': UnitTest_init(io, NULL, 0); break;
	default: break;	
	}

	return fdopen(UnitTest_open(io), mode);
}

void UnitTest_rewind(UnitTestIO *io)
{
	rewind(UnitTest_file(io));
}

ssize_t UnitTest_append(UnitTestIO *io, const char *data, size_t size)
{
	if	(!size)
		size = data ? strlen(data) : 0;

	return write(UnitTest_fd(io), data, size);
}

ssize_t UnitTest_init(UnitTestIO *io, const char *data, size_t size)
{
	if	(io->file)
	{
		fclose(io->file);
		io->file = NULL;
	}

	if	(data)
	{
		ssize_t s = UnitTest_append(io, data, size);
		UnitTest_rewind(io);
		return s;
	}

	return EOF;
}

const char *UnitTest_data(UnitTestIO *io)
{
	static STRING_DECL(buf);
	int c;

	if	(!io->file)
		return NULL;

	UnitTest_rewind(io);
	UnitTest_clear(&buf);

	while ((c = getc(io->file)) != EOF)
		UnitTest_putc(c, &buf);

	return UnitTest_str(&buf);
}
