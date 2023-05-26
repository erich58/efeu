#include <EFEU/UnitTest.h>

#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

#define	MKTEMP	"mktemp -d"

static int cleanup_pid = 0;
static char *tmpdir = NULL;

void UnitTest_tmpclean()
{
	if	(getpid() != cleanup_pid)
		return;

	pid_t pid = fork();

	if	(pid == 0)
		execl("/bin/rm", "rm", "-rf", tmpdir, (char*) NULL);
	
	wait(NULL);
	free((void *) tmpdir);
	tmpdir = NULL;
}

static void create_tmpdir()
{
	if	(tmpdir)
		return;

	if	(!(tmpdir = UnitTest_pgets(MKTEMP, '\n')))
	{
		UnitTest_error("'%s' failed", MKTEMP);
		exit(1);
	}

	cleanup_pid = getpid();
	atexit(UnitTest_tmpclean);
}

const char *UnitTest_tpl(const char *tpl)
{
	if	(tpl)
	{
		static STRING_DECL(buf);
		buf.size = 0;

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
				create_tmpdir();
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

const char *UnitTest_tmp(const char *name)
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

static const char *mkparent(const char *name)
{
	static STRING_DECL(buf);
	create_tmpdir();

	if	(!name)
		return tmpdir;

	while (*name == '/')
		name++;

	buf.size = 0;
	UnitTest_puts(tmpdir, &buf);
	UnitTest_putc('/', &buf);

	while (*name != 0)
	{
		if	(*name == '/')
			mkdir(UnitTest_str(&buf), 0777);

		UnitTest_putc(*name++, &buf);
	}

	return UnitTest_str(&buf);
}

const char *UnitTest_mkdir(const char *name)
{
	const char *path = mkparent(name);

	if	(mkdir(path, 0777) == 0)
		return path;

	return (errno == EEXIST) ? path : NULL;
}

const char *UnitTest_mkfile(const char *name, const char *content)
{
	const char *path = mkparent(name);

	if	(content)
	{
		FILE *file = fopen(path, "w");

		if	(!file)
			return NULL;

		fputs(content, file);
		fclose(file);
	}
	else
	{
		remove(path);
	}

	return path;
}

const char *UnitTest_content(const char *name)
{
	static STRING_DECL(buf);
	static int undefined = 1;

	if	(!name)
		return undefined ? NULL : buf.data;

	undefined = 1;
	buf.size = 0;

	FILE *file = fopen(UnitTest_mkpath(name), "r");
	
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

char *UnitTest_pgets(const char *cmd, int delim)
{
	STRING_DECL(buf);
	FILE *file;

	if	((file = popen(cmd, "r")))
	{
		int c;

		while ((c = getc(file)) != EOF && c != delim)
			UnitTest_putc(c, &buf);

		pclose(file);
	}

	return UnitTest_mstr(&buf);
}
