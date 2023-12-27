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

const char *UnitTest_name = NULL;
int UnitTest_child = 0;

static int count_good = 0;
static int count_bad = 0;
static int test_good = 0;
static int test_bad = 0;
static int main_good = 0;
static int main_bad = 0;

static int fork_depth = 0;
static int force_color = 0;

static int color_enabled(int fd_)
{
	if	(force_color)
		return 1;

	/* WARNING: isatty might change errno! So we save the previous value
	   an restore it after check.
	 */
	int save_errno = errno;
	int res = isatty(fd_);
	errno = save_errno;
	return res;
}

static const char *colorString(int color)
{
	static const char *cdef[4] = { NULL, NULL, NULL, NULL };
	static int need_setup = 1;

	if	(need_setup)
	{
		cdef[0] = UnitTest_pgets("tput sgr0", EOF);	// Off
		cdef[1] = UnitTest_pgets("tput setaf 2", EOF);	// Green
		cdef[2] = UnitTest_pgets("tput setaf 1", EOF);	// Red
		cdef[3] = UnitTest_pgets("tput setaf 4", EOF);	// Blue
		need_setup = 0;
	}

	return color < 4 ? cdef[color] : "";
}

static const char *colored(int fd, const char *name)
{
	static STRING_DECL(buf);

	int color = 0;

	if	(!name)	return name;

	switch (*name)
	{
	case '*':	color = 1; name++; break;
	case '!':	color = 2; name++; break;
	case '?':	color = 3; name++; break;
	default:	return name;
	}

	if	(!color_enabled(fd))
		return name;

	buf.size = 0;
	UnitTest_puts(colorString(color), &buf);
	UnitTest_puts(name, &buf);
	UnitTest_puts(colorString(0), &buf);
	return UnitTest_str(&buf);
}


/* Redirektion
*/

static int real_stdin = 0;
static int real_stdout = 1;
static int real_stderr = 2;

static const char *qualified_name(const char *name_)
{
	if	(!name_)
		return NULL;

	switch (*name_)
	{
	case 0:
		return "/dev/null";
	case '/':
		return name_;
	case '.':
		if	(name_[1] == 0 || name_[1] == '/')
			return name_;
		if	(name_[1] == '.' && (name_[2] == 0 || name_[2] == '/'))
			return name_;
			
		break;
	}

	return NULL;
}

const char *UnitTest_mkpath(const char *name)
{
	const char *fn = qualified_name(name);
	return fn ? fn : UnitTest_tmp(name);
}

ssize_t UnitTest_write(int fd, const char *str)
{
	return str ? write(fd, str, strlen(str)) : 0;
}

int UnitTest_reopen(int fd, const char *name)
{
	const char *fname = UnitTest_mkpath(name);
	int fdnew = creat(fname, 0644);

	if	(fdnew < 0)
	{
		UnitTest_error("%s: %s", fname, strerror(errno));
		return 0;
	}

	dup2(fdnew, fd);
	close(fdnew);
	return 1;
}

int UnitTest_stdin(const char *name, const char *content)
{
	fseek(stdin, 0L, SEEK_END);
	fflush(stdin);

	if	(name)
	{
		const char *fname;
		
		if	(real_stdin == 0)
			real_stdin = dup(0);

		if	((fname = qualified_name(name)))
			;
		else if	(content)
			fname = UnitTest_mkfile(name, content);
		else	fname = UnitTest_tmp(name);

		int fd = open(fname, O_RDONLY); 

		if	(fd < 0)
		{
			UnitTest_error("%s: %s", fname, strerror(errno));
			return 0;
		}

		dup2(fd, 0);
		close(fd);
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

		return UnitTest_reopen(1, name);
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

		return UnitTest_reopen(2, name);
	}
	else if	(real_stderr != 2)
	{
		dup2(real_stderr, 2);
	}

	return 1;
}

int UnitTest_fderr()
{
	return real_stderr;
}

const char *UnitTest_color(int color)
{
	if	(color_enabled(real_stderr))
		return colorString(color);

	return color ? "{" : "}";
}

static void strinfo(const char *pfx, const char *file, int line,
	const char *name, const char *value)
{
	static STRING_DECL(buf);
	const char *p = UnitTest_qsplit(&buf, value);

	if	(!name)
	{
		if	(*p == 0)
			p = UnitTest_next(&buf);

		UnitTest_info(file, line, pfx, "%s", p);

		while ((p = UnitTest_next(&buf)))
			UnitTest_info(file, line, "+", "%s", p);
	}
	else
	{
		UnitTest_info(file, line, pfx, "%s -> %s", name, p);

		while ((p = UnitTest_next(&buf)))
			UnitTest_info(file, line, "+", "\t%s", p);
	}
}

void UnitTest_expr(const char *file, int line, const char *expr)
{
	UnitTest_info(file, line, "EXPR", "%s", expr);
}

/* Atomare Infozeile
*/

void UnitTest_vinfo(const char *file, int line, const char *pfx,
	const char *fmt, va_list list)
{
	static STRING_DECL(buf);
	int n;

	UnitTest_clear(&buf);

	for (n = 0; n < fork_depth; ++n)
		UnitTest_puts(".\t", &buf);

	if	(pfx)
	{
		UnitTest_puts(colored(real_stderr, pfx), &buf);
		UnitTest_putc('\t', &buf);
	}

	if	(file)
	{
		UnitTest_puts(file, &buf);
		
		if	(line > 0)
			UnitTest_printf(&buf, ":%d", line);

		UnitTest_putc('\t', &buf);
	}

	UnitTest_vprintf(&buf, fmt, list);
	UnitTest_putc('\n', &buf);
	write(real_stderr, buf.data, buf.size);
}

void UnitTest_info(const char *file, int line, const char *pfx,
	const char *fmt, ...)
{
	va_list list;

	va_start (list, fmt);
	UnitTest_vinfo(file, line, pfx, fmt, list);
	va_end(list);
}

void UnitTest_strinfo(const char *file, int line, const char *pfx,
	const char *str, const char *expr)
{
	static STRING_DECL(buf);
	
	if	(expr && *expr == '"')
	{
		const char *p = UnitTest_split(&buf, expr);

		if	(*p == 0)
			p = UnitTest_next(&buf);

		UnitTest_info(file, line, pfx, "%s", p);

		while ((p = UnitTest_next(&buf)))
			UnitTest_info(file, line, "+", "%s", p);
	}
	else
	{
		strinfo(pfx, file, line, expr, str);
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
		UnitTest_info(file, line, "+", "\t%s", p);

	return r;
}

void UnitTest_error(const char *fmt, ...)
{
	va_list list;

	va_start (list, fmt);
	UnitTest_vinfo(NULL, 0, "#ERROR", fmt, list);
	va_end(list);
}

int UnitTest_vresult(const char *file, int line, int res,
	const char *fmt, va_list list)
{
	if	(res)
		count_good++;
	else
		count_bad++;

	UnitTest_vinfo(file, line, res ? "*GOOD" : "!BAD", fmt, list);
	return res;
}

int UnitTest_result(const char *file, int line, int res, const char *fmt, ...)
{
	va_list list;

	va_start (list, fmt);
	UnitTest_vresult(file, line, res, fmt, list);
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

	const char *p = x2 ? UnitTest_split(&buf, x2) :
		UnitTest_qsplit(&buf, s2);

	UnitTest_result(file, line, r, "%s %s %s", x1, op, p);

	while ((p = UnitTest_next(&buf)))
		UnitTest_info(file, line, "+", "\t%s", p);

	if	(r && !show)
		return r;

	if	(*x1 != '"')
		strinfo("BECAUSE", file, line, x1, s1);

	if	(x2 && *x2 != '"')
		strinfo("BECAUSE", file, line, x2, s2);

	return r;
}

int UnitTest_double(const char *file, int line,
	double d1, double d2, const char *x1, const char *x2)
{
	int r = UnitTest_dcmp(d1, d2);
	UnitTest_result(file, line, r, "%s ~ %s", x1, x2);

	if	(r == 0)
	{
		UnitTest_info(file, line, "BECAUSE", "%s = %.16g", x1, d1);
		UnitTest_info(file, line, "BECAUSE", "%s = %.16g", x2, d2);
	}

	return r;
}


int UnitTest_intmax(const char *file, int line,
	intmax_t i1, intmax_t i2, const char *x1, const char *x2)
{
	int r = (i1 == i2);
	UnitTest_result(file, line, r, "%s == %s", x1, x2);

	if	(r == 0)
	{
		UnitTest_info(file, line, "BECAUSE", "%s = %jd", x1, i1);
		UnitTest_info(file, line, "BECAUSE", "%s = %jd", x2, i2);
	}

	return r;
}

int UnitTest_char(const char *file, int line,
	int c1, int c2, const char *x1, const char *x2)
{
	int r = (c1 == c2);
	UnitTest_result(file, line, r, "%s == %s", x1, x2);

	if	(r == 0)
	{
		UnitTest_info(file, line, "BECAUSE", "%s = %s (%d)",
			x1, UnitTest_qchar(c1), c1);
		UnitTest_info(file, line, "BECAUSE", "%s = %s (%d)",
			x2, UnitTest_qchar(c2), c2);
	}

	return r;
}

int UnitTest_uintmax(const char *file, int line,
	uintmax_t i1, uintmax_t i2, const char *x1, const char *x2)
{
	int r = (i1 == i2);
	UnitTest_result(file, line, r, "%s == %s", x1, x2);

	if	(r == 0)
	{
		UnitTest_info(file, line, "BECAUSE", "%s = %ju", x1, i1);
		UnitTest_info(file, line, "BECAUSE", "%s = %ju", x2, i2);
	}

	return r;
}

void UnitTest_summary(const char *issue)
{
	main_good += count_good;
	main_bad += count_bad;

	if	(main_bad)	test_bad++;
	else if	(main_good)	test_good++;

	if	(!issue)
		issue = test_bad ? "!FAILURE" : "*SUCCESS";

	printf("%s\t%d\t%d\t%d\t%s\n", UnitTest_name, test_good + test_bad,
		test_good, test_bad, colored(real_stdout, issue));
	fflush(stdout);
	UnitTest_info(NULL, 0, NULL, NULL);
}

static void skiparg(int *narg, char **arg, int n)
{
	char *p;
	
	if	(n >= *narg || n < 0)	return;

	p = arg[n];

	for ((*narg)--; n < *narg; n++)
		arg[n] = arg[n + 1];

	arg[*narg] = p;
}

int main(int argc, char **argv)
{
	char *p;
	size_t n;

	if	((p = strrchr(argv[0], '/')))
		UnitTest_name = p + 1;
	else
		UnitTest_name = argv[0];

	if	(argc > 1 && strcmp("-c", argv[1]) == 0)
	{
		force_color = 1;
		skiparg(&argc, argv, 1);
	}

	fprintf(stderr, "%s\t%s", "#RUN", UnitTest_name);

	for (n = 1; n < argc; n++)
	{
		fputc(' ', stderr);
		fputs(argv[n], stderr);
	}

	fputc('\n', stderr);
	runUnitTest(argc, argv);
	UnitTest_summary(NULL);
	return test_bad ? 1 : 0;
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

int UnitTest_childstat(const char *file, int line, int stat)
{
	int s = UnitTest_wait();
	int res = (s == stat);

	if	(stat < 0)
		UnitTest_result(file, line, res, "child signaled %d", -stat);
	else
		UnitTest_result(file, line, res, "child exit %d", stat);

	if	(res)
		return res;

	if	(s < 0)
		UnitTest_info(file, line, "BECAUSE", "child signaled %d", -s);
	else
		UnitTest_info(file, line, "BECAUSE", "child exit %d", s);

	return res;
}

int UnitTest_fork(const char *file, int line)
{
	UnitTest_child = fork();

	if	(UnitTest_child == 0)
	{
		count_good = 0;
		count_bad = 0;
		test_good = 0;
		test_bad = 0;
		main_good = 0;
		main_bad = 0;
		fork_depth++;
		return 1;
	}
	else if	(UnitTest_child < 0)
	{
		UnitTest_result(file, line, 1, "fork");
	}

	return 0;
}

void UnitTest_exit(void)
{
	exit(count_bad || test_bad || main_bad ? 1 : 0);
}

static void run_test(const char *file, int line,
	void (*test)(void), const char *name)
{
	UnitTest_info(file, line, "#START", "%s", name);
	test();

	const char *pfx = "REPORT";

	if	(count_bad)
	{
		test_bad++;
		pfx = "!FAILURE";
	}
	else if	(count_good)
	{
		test_good++;
		pfx = "*SUCCESS";
	}

	count_good = 0;
	count_bad = 0;
	UnitTest_info(file, line, pfx, "%s", name);
}

void UnitTest_run(const char *file, int line,
	void (*test)(void), const char *name)
{
	main_good += count_good;
	main_bad += count_bad;
	count_good = 0;
	count_bad = 0;
	run_test(file, line, test, name);
	UnitTest_info(NULL, 0, NULL, NULL);
}

void UnitTest_exec(const char *file, int line,
	void (*test)(void), const char *name)
{
	UnitTest_info(file, line, "#EXEC", "%s", name);
	main_good += count_good;
	main_bad += count_bad;
	count_good = 0;
	count_bad = 0;

	if	(UnitTest_fork(file, line))
	{
		dup2(2, 1);
		run_test(file, line, test, name);
		UnitTest_exit();
	}

	if	(UnitTest_wait() == 0)
	{
		test_good++;
		UnitTest_info(file, line, "*SUCCESS", "%s", name);
	}
	else
	{
		test_bad++;
		UnitTest_info(file, line, "!FAILURE", "%s", name);
	}

	UnitTest_info(NULL, 0, NULL, NULL);
}

int UnitTest_assert(int stat)
{
	if	(!stat)
	{
		UnitTest_info(NULL, 0, "!ABORT", "");
		UnitTest_summary("!ABORT");
		exit(2);
	}

	return stat;
}

int UnitTest_dcmp(double a, double b)
{
	if	(b != 0.)
	{
		double x = (a - b) / b;
		return -0.0000005 < x && x < 0.0000005;
	}

	return a == 0.;
}

char **UnitTest_mkarg(const char *file_, int line_, const char *name_,
	size_t dim_, const char *data_, int delim_)
{
	if	(!name_)	name_ = "argv";
	if	(!data_)	data_ = "";

	size_t slen = strlen(data_) + 1;
	char **v = malloc((dim_ + 1) * sizeof v[0] + slen);

	if	(!v)
		UnitTest_assert(UnitTest_result(file_, line_,
			0, "UnitTest_mkarg()"));

	char *p = memcpy(v + dim_ + 1, data_, slen);

	for (size_t i = 0; i < dim_; ++i)
	{
		for (v[i] = p; *p; ++p)
		{
			if	(*p == delim_)
			{
				*p++ = 0;
				break;
			}
		}
	}

	v[dim_] = NULL;
	UnitTest_info(file_, line_, "*GOOD", "%s[%zd + 1] = {", name_, dim_);

	for (size_t i = 0; i <= dim_; ++i)
		UnitTest_info(file_, line_, "+", "\t%s,", UnitTest_qstr(v[i]));

	UnitTest_info(file_, line_, "+", "}");
	return v;
}
