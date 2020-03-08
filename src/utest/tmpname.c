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

static int cleanup_pid = 0;
static const char *tmpdir = NULL;

void UnitTest_tmpclean()
{
	if	(getpid() != cleanup_pid)
		return;

	pid_t pid = fork();

	if	(pid == 0)
		execl("/bin/rm", "rm", "-rf", tmpdir, (char*) NULL);
	
	wait(NULL);
	tmpdir = NULL;
}

static void create_tmpdir()
{
	STRING_DECL(buf);
	FILE *file;

	if	(tmpdir)
		return;

	if	((file = popen(MKTEMP, "r")))
	{
		int c;

		while ((c = getc(file)) != EOF && c != '\n')
			UnitTest_putc(c, &buf);

		fclose(file);
	}

	if	(buf.size)
	{
		cleanup_pid = getpid();
		tmpdir = UnitTest_cstr(&buf);
		atexit(UnitTest_tmpclean);
	}
	else
	{
		exit(1);
	}
}

const char *UnitTest_tpl(const char *tpl)
{
	if	(tpl)
	{
		static STRING_DECL(buf);
		buf.size = 0;
		create_tmpdir();

		while (*tpl)
		{
			if	(*tpl == '\\')
			{
				++tpl;

				if	(*tpl)
					UnitTest_putc(*tpl, &buf);
				else
					break;
			}
			else if	(*tpl == '@')
			{
				UnitTest_puts(tmpdir, &buf);
			}
			else
			{
				UnitTest_putc(*tpl, &buf);
			}

			++tpl;
		}

		UnitTest_putc(0, &buf);
		return buf.data;
	}

	return NULL;
}

const char *UnitTest_tmpname(const char *name)
{
	create_tmpdir();

	if	(name)
	{
		static STRING_DECL(buf);
		buf.size = 0;
		UnitTest_puts(tmpdir, &buf);
		UnitTest_putc('/', &buf);
		UnitTest_puts(name, &buf);
		UnitTest_putc(0, &buf);
		return buf.data;
	}

	return tmpdir;
}

const char *UnitTest_mkdir(const char *name)
{
	const char *path = UnitTest_tmpname(name);
	return mkdir(path, 0777) == 0 ? path : NULL;
}

const char *UnitTest_mkfile(const char *name, const char *content)
{
	const char *path = UnitTest_tmpname(name);
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
	static STRING_DECL(buf);
	static int undefined = 1;

	if	(!name)
		return undefined ? NULL : buf.data;

	undefined = 1;
	buf.size = 0;

	const char *path = UnitTest_tmpname(name);
	FILE *file = fopen(path, "r");
	
	if	(file)
	{
		int c;

		while ((c = getc(file)) != EOF)
			UnitTest_putc(c, &buf);
		
		fclose(file);
		UnitTest_putc(0, &buf);
		undefined = 0;
		return buf.data;
	}

	return NULL;
}
