#include <EFEU/UnitTest.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

char *UnitTest_name = NULL;
static int count_good = 0;
static int count_bad = 0;
static int fork_depth = 0;

typedef void (*runFunction)(void);

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
	write(2, buf.data, buf.size);
}

static void info(const char *pfx, const char *file, int line,
	const char *fmt, ...)
{
	va_list list;

	va_start (list, fmt);
	vinfo(pfx, file, line, fmt, list);
	va_end(list);
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

int UnitTest_expect_strcmp(const char *file, int line, const char *op,
	const char *s1, const char *s2, const char *x1, const char *x2)
{
	int r = 0;

	if	(!op || !*op)
		;
	else if	(strcmp(op, "~") == 0)
		r = UnitTest_match(s1, s2);
	else if	(strcmp(op, "!~") == 0)
		r = UnitTest_match(s1, s2) == 0;
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

	UnitTest_result(file, line, r, "%s %s %s", x1, op, x2, NULL);

	if	(r)
		return r;

	if	(*x1 != '"')
		info("#AS", file, line, "%s = %s", x1, UnitTest_qstr(s1), NULL);

	if	(*x2 != '"')
		info("#AS", file, line, "%s = %s", x2, UnitTest_qstr(s2), NULL);

	return 0;
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
	printf("%s\t%d\t%d\t%d\t%s\n", UnitTest_name, count_good + count_bad,
		count_good, count_bad, count_bad ? "FAILURE" : "SUCCESS");
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
