#ifndef EFEU_UnitTest_h
#define	EFEU_UnitTest_h	1

#include <string.h>
#include <stdio.h>

void UnitTest_expr(const char *file, int line, const char *expr);
void UnitTest_info(const char *file, int line, const char *fmt, ...);
int UnitTest_result(const char *file, int line, int res, const char *fmt, ...);
const char *UnitTest_tpl(const char *tpl);
const char *UnitTest_tmpname(const char *name);
const char *UnitTest_mkdir(const char *name);
const char *UnitTest_mkfile(const char *name, const char *content);
const char *UnitTest_content(const char *name);
const char *UnitTest_qstr(const char *str);
int UnitTest_strcmp(const char *s1, const char *s2);
int UnitTest_match(const char *str, const char *expr);
int UnitTest_fork(const char *file, int line);
void UnitTest_exit(void);
int UnitTest_wait(void);

void runUnitTest(void);
extern char *UnitTest_name;

#define TEST_DECL(name)	static void _test_ ## name (void)
#define TEST_SUITE	void runUnitTest(void)

#define TEST_RUN(name)	\
fprintf(stderr, "#TEST\t%s\n", # name), _test_ ## name ()

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

#define	EXPECT_TRUE(expr) \
UnitTest_result(__FILE__,__LINE__, (expr), "%s", # expr)

#define	EXPECT_FALSE(expr) \
UnitTest_result(__FILE__,__LINE__, !(expr), "!(%s)", # expr)

#define	EXPECT_EQ(a, b)		EXPECT_TRUE(a == b)
#define	EXPECT_NE(a, b)		EXPECT_TRUE(a != b)

#define EXPECT_STR(a, b)	\
UnitTest_result(__FILE__, __LINE__, \
	UnitTest_strcmp(a, b) == 0, "%s", # b)

#define EXPECT_STRCMP(a, op, b)	\
UnitTest_result(__FILE__, __LINE__, \
	UnitTest_strcmp(a, b) op 0, "%s %s %s", # a, # op, # b)

#define	EXPECT_STREQ(a, b)	EXPECT_STRCMP(a, ==, b)
#define	EXPECT_STRNE(a, b)	EXPECT_STRCMP(a, !=, b)

#define EXPECT_MATCH(a, b)	\
UnitTest_result(__FILE__, __LINE__, \
	UnitTest_match(a, b), "%s ~ %s", # a, # b)

#define EXPECT_NOMATCH(a, b)	\
UnitTest_result(__FILE__, __LINE__, \
	UnitTest_match(a, b) == 0, "%s !~ %s", # a, # b)

#define	TEST_DIR(name)	\
UnitTest_result(__FILE__, __LINE__, \
	UnitTest_mkdir(name) != NULL, "mkdir(%s)", # name)

#define	TEST_FILE(name, cont)	\
UnitTest_result(__FILE__, __LINE__, \
	UnitTest_mkfile(name, cont) != NULL, "mkfile(%s, %s)", # name, # cont)

#define PRINT_CONTENT(name)	\
UnitTest_info(__FILE__, __LINE__, \
	"content(%s) -> %s", # name, UnitTest_qstr(UnitTest_content(name)))

#define	TEST_CONTENT(name, cont)	\
UnitTest_result(__FILE__, __LINE__, \
	UnitTest_strcmp(UnitTest_content(name), cont) == 0, \
	"content(%s) == %s", # name, # cont)

#define	MATCH_CONTENT(name, cont)	\
UnitTest_result(__FILE__, __LINE__, \
	UnitTest_match(UnitTest_content(name), cont), \
	"content(%s) ~ %s", # name, # cont)

#define	TEST_FORK	\
	if	(UnitTest_fork(__FILE__, __LINE__))

#define	TEST_EXIT		UnitTest_exit()

#define	CHILD_EXIT(n)		\
UnitTest_result(__FILE__, __LINE__, \
	UnitTest_wait() == n, "child exit %d", n)

#define	CHILD_SIGNALED(n)		\
UnitTest_result(__FILE__, __LINE__, \
	UnitTest_wait() == -n, "child signaled %d", n)

#define	TEST_WAIT		CHILD_EXIT(0)

#endif
