#include <EFEU/UnitTest.h>

#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define	MKTEMP	"mktemp -d"
#define	CLEANUP	"rm -rvf "
#define	BSIZE	64

typedef struct {
	char *data;
	size_t size;
	size_t pos;
} SBUF;

static void buf_putc(SBUF *buf, int c)
{
	if	(buf->pos >= buf->size)
	{
		buf->size += buf->size ? buf->size : BSIZE;
		buf->data = realloc(buf->data, buf->size);
	}

	buf->data[buf->pos++] = c;
}

static size_t buf_puts(SBUF *buf, const char *str)
{
	if	(str)
	{
		while (*str)
			buf_putc(buf, *str++);
	}

	return buf->pos;
}

static char *cmd_cleanup = NULL;
static char *tmpdir = NULL;

static void cleanup_tmpdir()
{
	if	(cmd_cleanup)
		system(cmd_cleanup);
}

static void create_tmpdir()
{
	SBUF buf = { NULL, 0, 0 };
	FILE *file;
	size_t pos;
	int c;

	if	(tmpdir)
		return;

	if	(!(file = popen(MKTEMP, "r")))
	{
		perror(MKTEMP);
		exit(1);
	}

	pos = buf_puts(&buf, CLEANUP);

	while ((c = getc(file)) != EOF && c != '\n')
		buf_putc(&buf, c);

	buf_putc(&buf, 0);
	fclose(file);
	cmd_cleanup = buf.data;
	tmpdir = cmd_cleanup + pos;
	atexit(cleanup_tmpdir);
}

const char *UnitTest_tempname(const char *name)
{
	create_tmpdir();

	if	(name)
	{
		static SBUF buf = { NULL, 0, 0 };
		buf.pos = 0;
		buf_puts(&buf, tmpdir);
		buf_putc(&buf, '/');
		buf_puts(&buf, name);
		buf_putc(&buf, 0);
		return buf.data;
	}

	return tmpdir;
}

const char *UnitTest_mkdir(const char *name)
{
	const char *path = UnitTest_tempname(name);
	return mkdir(path, 0777) == 0 ? path : NULL;
}

const char *UnitTest_mkfile(const char *name, const char *content)
{
	const char *path = UnitTest_tempname(name);
	FILE *file = fopen(path, "w");
	
	if	(file)
	{
		if	(content)
			fputs(content, file);

		fclose(file);
		return path;
	}
	else
	{
		perror(path);
	}

	return NULL;
}

const char *UnitTest_content(const char *name)
{
	static SBUF buf = { NULL, 0, 0 };
	static int undefined = 1;

	if	(!name)
		return undefined ? NULL : buf.data;

	undefined = 1;
	buf.pos = 0;

	const char *path = UnitTest_tempname(name);
	FILE *file = fopen(path, "r");
	
	if	(file)
	{
		int c;

		while ((c = getc(file)) != EOF)
			buf_putc(&buf, c);
		
		fclose(file);
		buf_putc(&buf, 0);
		undefined = 0;
		return buf.data;
	}

	return NULL;
}
