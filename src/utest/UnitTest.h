#ifndef EFEU_UnitTest_h
#define	EFEU_UnitTest_h	1

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <unistd.h>

typedef struct {
	char *data;
	size_t size;
	size_t capacity;
	size_t pos;
} UnitTestString;

typedef struct {
	FILE *file;
} UnitTestIO;

#define	STRING_DECL(name)	UnitTestString name = { NULL, 0, 0, 0, }
#define	TEST_IO_DECL(name)	UnitTestIO name = { NULL }

void UnitTest_clear(UnitTestString *ustr);
void UnitTest_free(UnitTestString *ustr);
void UnitTest_putc(int c, UnitTestString *ustr);
void UnitTest_puts(const char *s, UnitTestString *ustr);
void UnitTest_qputc(int c, int delim, UnitTestString *ustr);
void UnitTest_qputs(const char *s, int delim, UnitTestString *ustr);
void UnitTest_vprintf(UnitTestString *ustr, const char *fmt, va_list list);
void UnitTest_printf(UnitTestString *ustr, const char *fmt, ...);

size_t UnitTest_start(UnitTestString *ustr);
const char *UnitTest_next(UnitTestString *ustr);
const char *UnitTest_split(UnitTestString *ustr, const char *str);
const char *UnitTest_qsplit(UnitTestString *ustr, const char *str);
size_t UnitTest_lsplit(UnitTestString *ustr, const char *str);
const char *UnitTest_str(UnitTestString *ustr);
char *UnitTest_mstr(UnitTestString *ustr);

FILE *UnitTest_file(UnitTestIO *io);
int UnitTest_fd(UnitTestIO *io);
FILE *UnitTest_fopen(UnitTestIO *io, const char *mode);
int UnitTest_open(UnitTestIO *io);
void UnitTest_rewind(UnitTestIO *io);
ssize_t UnitTest_append(UnitTestIO *io, const char *data, size_t size);
ssize_t UnitTest_init(UnitTestIO *io, const char *data, size_t size);
const char *UnitTest_data(UnitTestIO *io);

const char *UnitTest_cstr(const char *str);
const char *UnitTest_qstr(const char *str);
const char *UnitTest_qchar(int c);
const char *UnitTest_color(int color);

int UnitTest_strcmp(const char *s1, const char *s2);
int UnitTest_match(const char *str, const char *expr);
int UnitTest_dcmp(double d1, double d2);

void UnitTest_expr(const char *file, int line, const char *expr);
void UnitTest_test(const char *file, int line, const char *name);
void UnitTest_info(const char *file, int line, const char *pfx,
	const char *fmt, ...);
void UnitTest_strinfo(const char *file, int line, const char *pfx,
	const char *str, const char *expr);
void UnitTest_error(const char *fmt, ...);
int UnitTest_result(const char *file, int line, int res, const char *fmt, ...);
int UnitTest_strcheck(const char *file, int line, const char *op,
	const char *s1, const char *s2, const char *x1, const char *x2);
int UnitTest_linematch(const char *file, int line,
	const char *str, const char *expr, const char *term);
int UnitTest_double(const char *file, int line,
	double d1, double d2, const char *x1, const char *x2);
int UnitTest_intmax(const char *file, int line,
	intmax_t i1, intmax_t i2, const char *x1, const char *x2);
int UnitTest_uintmax(const char *file, int line,
	uintmax_t i1, uintmax_t i2, const char *x1, const char *x2);
int UnitTest_char(const char *file, int line,
	int c1, int c2, const char *x1, const char *x2);
int UnitTest_create(const char *file, int line, const char *name,
	const char *content, const char *expr);
char **UnitTest_mkarg(const char *file, int line, const char *name,
	size_t n, const char *def, int delim);
void UnitTest_summary(const char *issue);

char *UnitTest_pgets(const char *cmd_, int delim);
const char *UnitTest_tpl(const char *tpl);
const char *UnitTest_tmp(const char *name);
const char *UnitTest_mkdir(const char *name);
const char *UnitTest_mkfile(const char *name, const char *content);
const char *UnitTest_content(const char *name);
void UnitTest_tmpclean(void);

int UnitTest_stdin(const char *name, const char *content);
int UnitTest_stdout(const char *name);
int UnitTest_stderr(const char *name);
int UnitTest_reopen(int fd, const char *name);
FILE *UnitTest_strout(char *buf, size_t size);
FILE *UnitTest_strin(const char *buf, size_t size);

int UnitTest_fork(const char *file, int line);
int UnitTest_exec(const char *file, int line,
	void (*test)(void), const char *name);
void UnitTest_exit(void);
int UnitTest_wait(void);
int UnitTest_assert(int stat);

void runUnitTest(int argc, char **argv);
extern char *UnitTest_name;
extern int UnitTest_child;

#define TEST_DECL(name)	static void _test_ ## name (void)
#define TEST_SUITE	\
void runUnitTest(int UnitTest_argc, char **UnitTest_argv)

#define TEST_RUN(name)	\
UnitTest_test(NULL, 0, # name), _test_ ## name ()

#define TEST_EXPR(expr)	\
UnitTest_expr(__FILE__, __LINE__, # expr), (expr)

#define PRINT_LABEL(label)	\
UnitTest_info(__FILE__, __LINE__, "INFO", "# %s", label)

#define PRINT_FMT(fmt, expr)	\
UnitTest_info(__FILE__, __LINE__, "INFO", \
	"(%s, %s) -> " fmt, # fmt, # expr, (expr))

#define PRINT_INT(expr)	\
UnitTest_info(__FILE__, __LINE__, "INFO", \
	"(int)(%s) -> %d", # expr, (int)(expr))

#define PRINT_SIZE(expr) \
UnitTest_info(__FILE__, __LINE__, "INFO", \
	"(size_t)(%s) -> %zu", # expr, (size_t)(expr))

#define PRINT_SSIZE(expr) \
UnitTest_info(__FILE__, __LINE__, "INFO", \
	"(ssize_t)(%s) -> %zd", # expr, (ssize_t)(expr))

#define PRINT_LONG(expr) \
UnitTest_info(__FILE__, __LINE__, "INFO", \
	"(long long)(%s) -> %lld", # expr, (long long)(expr))

#define PRINT_PTR(expr)	\
UnitTest_info(__FILE__, __LINE__, "INFO", \
	"%s -> %p", # expr, (void *)(expr))

#define PRINT_STR(expr)	\
UnitTest_strinfo(__FILE__, __LINE__, "INFO", expr, # expr)

#define	EXPECT(expr) \
UnitTest_result(__FILE__,__LINE__, (expr), "%s", # expr)

#define	ASSERT(expr) \
UnitTest_assert(EXPECT(expr))

#define EXPECT_PTR(expr)	\
UnitTest_result(__FILE__,__LINE__, (expr) != NULL, "%s", # expr)

#define ASSERT_PTR(expr)	\
UnitTest_assert(EXPECT_PTR(expr))

#define EXPECT_DBL(a, b)	\
UnitTest_double(__FILE__, __LINE__, a, b, # a, # b)

#define EXPECT_INT(a, b)	\
UnitTest_intmax(__FILE__, __LINE__, a, b, # a, # b)

#define EXPECT_UINT(a, b)	\
UnitTest_uintmax(__FILE__, __LINE__, a, b, # a, # b)

#define EXPECT_CHAR(a, b)	\
UnitTest_char(__FILE__, __LINE__, a, b, # a, # b)

#define EXPECT_STR(a, b)	\
UnitTest_strcheck(__FILE__, __LINE__, "==", a, b, # a, # b)

#define EXPECT_STRCMP(a, op, b)	\
UnitTest_strcheck(__FILE__, __LINE__, # op, a, b, # a, # b)

#define MATCH_STR(a, b)	\
UnitTest_strcheck(__FILE__, __LINE__, "~", a, b, # a, # b)

#define MATCH_LINES(a, b)	\
UnitTest_linematch(__FILE__, __LINE__, a, b, # a)

#define	TEST_DIR(name)	\
UnitTest_result(__FILE__, __LINE__, \
	UnitTest_mkdir(name) != NULL, "mkdir(%s)", # name)

#define	TEST_FILE(name, cont)	\
UnitTest_create(__FILE__, __LINE__, name, cont, # cont)

#define PRINT_CONTENT(name)	\
UnitTest_strinfo(__FILE__, __LINE__, "INFO", \
	UnitTest_content(name), "content(" # name ")")

#define	EXPECT_CONTENT(name, cont)	\
UnitTest_strcheck(__FILE__, __LINE__, "==", \
	UnitTest_content(name), cont, "content(" # name ")", # cont)

#define	MATCH_CONTENT(name, cont)	\
UnitTest_strcheck(__FILE__, __LINE__, "~", \
	UnitTest_content(name), cont, "content(" # name ")", # cont)

#define	TEST_FORK	\
	if	(UnitTest_fork(__FILE__, __LINE__))

#define	TEST_EXIT		UnitTest_exit()

#define	CHILD_EXIT(n)		\
UnitTest_result(__FILE__, __LINE__, \
	UnitTest_wait() == n, "child exit %d", n)

#define	CHILD_SIGNALED(n)		\
UnitTest_result(__FILE__, __LINE__, \
	UnitTest_wait() == -n, "child signaled %d", n)

#define	TEST_EXEC(name)		\
UnitTest_exec(__FILE__, __LINE__, _test_ ## name, # name)

#define	TEST_ARGV(name, n, def)	\
	char **name = UnitTest_mkarg(__FILE__, __LINE__, # name, n, def, ' ')

#define	TEST_ARGV2(name, n, def, delim)	\
	char **name = UnitTest_mkarg(__FILE__, __LINE__, # name, n, def, delim)


#endif
