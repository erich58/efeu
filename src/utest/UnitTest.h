#ifndef EFEU_UnitTest_h
#define	EFEU_UnitTest_h	1

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

typedef struct {
	char *data;
	size_t size;
	size_t capacity;
} UnitTestString;

#define	STRING_DECL(name)	UnitTestString name = { NULL, 0, 0, }

void UnitTest_clear(UnitTestString *ustr);
void UnitTest_free(UnitTestString *ustr);
void UnitTest_putc(int c, UnitTestString *ustr);
void UnitTest_puts(const char *s, UnitTestString *ustr);
void UnitTest_qputs(const char *s, int delim, UnitTestString *ustr);
void UnitTest_vprintf(UnitTestString *ustr, const char *fmt, va_list list);
void UnitTest_printf(UnitTestString *ustr, const char *fmt, ...);

const char *UnitTest_cstr(UnitTestString *ustr);
const char *UnitTest_qstr(const char *str);

int UnitTest_strcmp(const char *s1, const char *s2);
int UnitTest_match(const char *str, const char *expr);

void UnitTest_expr(const char *file, int line, const char *expr);
void UnitTest_test(const char *file, int line, const char *name);
void UnitTest_info(const char *file, int line, const char *fmt, ...);
void UnitTest_error(const char *fmt, ...);
int UnitTest_result(const char *file, int line, int res, const char *fmt, ...);
int UnitTest_strcheck(const char *file, int line, const char *op,
	const char *s1, const char *s2, const char *x1, const char *x2);

const char *UnitTest_tpl(const char *tpl);
const char *UnitTest_tmpname(const char *name);
const char *UnitTest_mkdir(const char *name);
const char *UnitTest_mkfile(const char *name, const char *content);
const char *UnitTest_content(const char *name);
int UnitTest_stdout(const char *name_);
int UnitTest_stderr(const char *name_);
void UnitTest_tmpclean(void);

int UnitTest_fork(const char *file, int line);
int UnitTest_exec(const char *file, int line,
	void (*test)(void), const char *name);
void UnitTest_exit(void);
int UnitTest_wait(void);
int UnitTest_assert(int stat);

void runUnitTest(void);
extern char *UnitTest_name;

#define TEST_DECL(name)	static void _test_ ## name (void)
#define TEST_SUITE	void runUnitTest(void)

#define TEST_RUN(name)	\
UnitTest_test(NULL, 0, # name), _test_ ## name ()

#define TEST_EXPR(expr)	\
UnitTest_expr(__FILE__, __LINE__, # expr), (expr)

#define PRINT_LABEL(label)	\
UnitTest_info(__FILE__, __LINE__, "/* %s */", label)

#define PRINT_FMT(fmt, expr)	\
UnitTest_info(__FILE__, __LINE__, \
	"(%s, %s) -> " fmt, # fmt, # expr, (expr))

#define PRINT_INT(expr)	\
UnitTest_info(__FILE__, __LINE__, \
	"(int)(%s) -> %d", # expr, (int)(expr))

#define PRINT_SIZE(expr) \
UnitTest_info(__FILE__, __LINE__, \
	"(size_t)(%s) -> %zu", # expr, (size_t)(expr))

#define PRINT_SSIZE(expr) \
UnitTest_info(__FILE__, __LINE__, \
	"(ssize_t)(%s) -> %zd", # expr, (size_t)(expr))

#define PRINT_LONG(expr) \
UnitTest_info(__FILE__, __LINE__, \
	"(long long)(%s) -> %lld", # expr, (long long)(expr))

#define PRINT_PTR(expr)	\
UnitTest_info(__FILE__, __LINE__, \
	"%s -> %p", # expr, (void *)(expr))

#define PRINT_STR(expr)	\
UnitTest_info(__FILE__, __LINE__, \
	"%s -> %s", # expr, UnitTest_qstr(expr))

#define	EXPECT(expr) \
UnitTest_result(__FILE__,__LINE__, (expr), "%s", # expr)

#define	ASSERT(expr) \
UnitTest_assert(EXPECT(expr))

#define	EXPECT_FALSE(expr) \
UnitTest_result(__FILE__,__LINE__, !(expr), "!(%s)", # expr)

#define EXPECT_STR(a, b)	\
UnitTest_strcheck(__FILE__, __LINE__, "==", a, b, # a, # b)

#define EXPECT_STRCMP(a, op, b)	\
UnitTest_strcheck(__FILE__, __LINE__, # op, a, b, # a, # b)

#define MATCH_STR(a, b)	\
UnitTest_strcheck(__FILE__, __LINE__, "~", a, b, # a, # b)

#define	TEST_DIR(name)	\
UnitTest_result(__FILE__, __LINE__, \
	UnitTest_mkdir(name) != NULL, "mkdir(%s)", # name)

#define	TEST_FILE(name, cont)	\
UnitTest_result(__FILE__, __LINE__, \
	UnitTest_mkfile(name, cont) != NULL, "mkfile(%s, %s)", # name, # cont)

#define PRINT_CONTENT(name)	\
UnitTest_info(__FILE__, __LINE__, \
	"content(%s) -> %s", # name, UnitTest_qstr(UnitTest_content(name)))

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

#endif
