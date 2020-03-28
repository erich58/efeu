#include <EFEU/UnitTest.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

char *UnitTest_name = NULL;
static int count_good = 0;
static int count_bad = 0;
static int fork_depth = 0;

typedef void (*runFunction)(void);

/* Redirektion
*/

static int real_stdin = 0;
static int real_stdout = 1;
static int real_stderr = 2;

static const char *std_name(const char *name)
{
	switch (*name)
	{
	case 0:
		name = "/dev/null";
		break;
	case '.':
	case '/':
		break;
	default:
		name = NULL;
		break;
	}

	return name;
}

static int reopen_fd(int fd, const char *name)
{
	const char *fname = std_name(name);

	if	(!fname)
		fname = UnitTest_tmpname(name);

	int fdnew = creat(fname, 0644);

	if	(fdnew < 0)
	{
		UnitTest_error("%s: %s", fname, strerror(errno));
		return 0;
	}

	dup2(fdnew, fd);
	return 1;
}

int UnitTest_stdin(const char *name, const char *content)
{
	if	(name)
	{
		const char *fname;
		
		if	(real_stdin == 0)
			real_stdin = dup(0);

		if	((fname = std_name(name)))
			;
		else if	(content)
			fname = UnitTest_mkfile(name, content);
		else	fname = UnitTest_tmpname(name);

		int fd = open(fname, O_RDONLY); 

		if	(fd < 0)
		{
			UnitTest_error("%s: %s", fname, strerror(errno));
			return 0;
		}

		dup2(fd, 0);
		return 1;
	}
	else if	(real_stdin != 0)
	{
		dup2(real_stdin, 0);
	}

	return 1;
}

int UnitTest_stdout(const char *name)
{
	fflush(stdout);

	if	(name)
	{
		if	(real_stdout == 1)
			real_stdout = dup(1);

		return reopen_fd(1, name);
	}
	else if	(real_stdout != 1)
	{
		dup2(real_stdout, 1);
	}

	return 1;
}

int UnitTest_stderr(const char *name)
{
	fflush(stderr);

	if	(name)
	{
		if	(real_stderr == 2)
			real_stderr = dup(2);

		return reopen_fd(2, name);
	}
	else if	(real_stderr != 2)
	{
		dup2(real_stderr, 2);
	}

	return 1;
}

/* Atomare Infozeile
*/

static void vinfo(const char *pfx, const char *file, int line,
	const char *fmt, va_list list)
{
	static STRING_DECL(buf);
	int n;

	UnitTest_clear(&buf);

	for (n = 0; n < fork_depth; ++n)
		UnitTest_puts(".\t", &buf);

	if	(pfx)
	{
		UnitTest_puts(pfx, &buf);
		UnitTest_putc('\t', &buf);
	}

	if	(file)
		UnitTest_printf(&buf, "%s:%d\t", file, line);

	UnitTest_vprintf(&buf, fmt, list);
	UnitTest_putc('\n', &buf);
	write(real_stderr, buf.data, buf.size);
}

static void info(const char *pfx, const char *file, int line,
	const char *fmt, ...)
{
	va_list list;

	va_start (list, fmt);
	vinfo(pfx, file, line, fmt, list);
	va_end(list);
}

static void strinfo(const char *pfx, const char *file, int line,
	const char *name, const char *value)
{
	static STRING_DECL(buf);
	const char *p = UnitTest_qsplit(&buf, value);
	info(pfx, file, line, "%s -> %s", name, p);

	while ((p = UnitTest_next(&buf)))
		info("+", file, line, "\t%s", p);
}

void UnitTest_expr(const char *file, int line, const char *expr)
{
	info("EXPR", file, line, "%s", expr);
}

void UnitTest_info(const char *file, int line, const char *fmt, ...)
{
	va_list list;

	va_start (list, fmt);
	vinfo("INFO", file, line, fmt, list);
	va_end(list);
}

void UnitTest_strinfo(const char *file, int line,
	const char *str, const char *expr)
{
	static STRING_DECL(buf);
	
	if	(*expr == '"')
	{
		const char *p = UnitTest_split(&buf, expr);

		if	(*p == 0)
			p = UnitTest_next(&buf);

		info("INFO", file, line, "%s", p);

		while ((p = UnitTest_next(&buf)))
			info("+", file, line, "%s", p);
	}
	else
	{
		strinfo("INFO", file, line, expr, str);
	}
}

int UnitTest_create(const char *file, int line, const char *name,
	const char *str, const char *expr)
{
	static STRING_DECL(buf);
	const char *p;
	int r = (UnitTest_mkfile(name, str) != NULL);

	if	(*expr == '"')
		p = UnitTest_split(&buf, expr);
	else
		p = UnitTest_qsplit(&buf, str);

	UnitTest_result(file, line, r, "mkfile(%s) << %s",
		UnitTest_qstr(name), p);

	while ((p = UnitTest_next(&buf)))
		info("+", file, line, "\t%s", p);

	return r;
}

void UnitTest_error(const char *fmt, ...)
{
	va_list list;

	va_start (list, fmt);
	vinfo("#ERROR", NULL, 0, fmt, list);
	va_end(list);
}

void UnitTest_test(const char *file, int line, const char *name)
{
	UnitTest_stdout(NULL);
	UnitTest_stderr(NULL);
	info("#TEST", file, line, "%s", name);
}

int UnitTest_result(const char *file, int line, int res, const char *fmt, ...)
{
	va_list list;

	if	(res)
		count_good++;
	else
		count_bad++;

	va_start (list, fmt);
	vinfo(res ? "GOOD" : "BAD", file, line, fmt, list);
	va_end(list);
	return res;
}

int UnitTest_strcheck(const char *file, int line, const char *op,
	const char *s1, const char *s2, const char *x1, const char *x2)
{
	static STRING_DECL(buf);
	int r = 0;
	int show = 0;

	if	(!op || !*op)
		;
	else if	(strcmp(op, "~") == 0)
		show = 1, r = UnitTest_match(s1, s2);
	else if	(strcmp(op, "!~") == 0)
		show = 1, r = UnitTest_match(s1, s2) == 0;
	else if	(strcmp(op, "==") == 0)
		r = UnitTest_strcmp(s1, s2) == 0;
	else if	(strcmp(op, "!=") == 0)
		r = UnitTest_strcmp(s1, s2) != 0;
	else if	(strcmp(op, "<") == 0)
		r = UnitTest_strcmp(s1, s2) < 0;
	else if	(strcmp(op, "<=") == 0)
		r = UnitTest_strcmp(s1, s2) <= 0;
	else if	(strcmp(op, ">") == 0)
		r = UnitTest_strcmp(s1, s2) > 0;
	else if	(strcmp(op, ">=") == 0)
		r = UnitTest_strcmp(s1, s2) >= 0;

	const char *p = UnitTest_split(&buf, x2);
	UnitTest_result(file, line, r, "%s %s %s", x1, op, p);

	while ((p = UnitTest_next(&buf)))
		info("+", file, line, "\t%s", p);

	if	(r && !show)
		return r;

	if	(*x1 != '"')
		strinfo("BECAUSE", file, line, x1, s1);

	if	(*x2 != '"')
		strinfo("BECAUSE", file, line, x2, s2);

	return r;
}

int main(int argc, char **argv)
{
	char *p;

	if	((p = strrchr(argv[0], '/')))
		UnitTest_name = p + 1;
	else
		UnitTest_name = argv[0];

	fprintf(stderr, "%s\t%s\n", "#RUN", UnitTest_name);
	runUnitTest();
	UnitTest_stdout(NULL);
	printf("%s\t%d\t%d\t%d\t%s\n", UnitTest_name, count_good + count_bad,
		count_good, count_bad, count_bad ? "FAILURE" : "SUCCESS");
	fflush(stdout);
	return count_bad ? 1 : 0;
}

int UnitTest_wait(void)
{
	int status;
	pid_t pid = wait(&status);

	if	(pid < 0)
		return 0;

	if	(WIFEXITED(status))
		return WEXITSTATUS(status);

	if	(WIFSIGNALED(status))
		return -WTERMSIG(status);

	return 0x100;
}

int UnitTest_fork(const char *file, int line)
{
	pid_t pid = fork();

	if	(pid == 0)
	{
		count_good = 0;
		count_bad = 0;
		fork_depth++;
		return 1;
	}
	else if	(pid < 0)
	{
		UnitTest_result(file, line, 1, "fork");
	}

	return 0;
}

void UnitTest_exit(void)
{
	exit(count_bad ? 1 : 0);
}

int UnitTest_exec(const char *file, int line,
	void (*test)(void), const char *name)
{
	if	(UnitTest_fork(file, line))
	{
		info("#RUN", NULL, 0, "%s", name);
		test();
		UnitTest_exit();
	}

	return UnitTest_result(file, line,
		UnitTest_wait() == 0, "%s success", name);
}

int UnitTest_assert(int stat)
{
	if	(!stat)
	{
		UnitTest_tmpclean();
		info("ABORT", NULL, 0, "");
		abort();
	}

	return stat;
}
