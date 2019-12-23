#ifndef EFEU_UnitTest_h
#define	EFEU_UnitTest_h	1

#include <string.h>

int UnitTest_result(const char *file, int line, int result,
	const char *fmt, ...);
int UnitTest_status(void);
int UnitTest_strcmp(const char *s1, const char *s2);
const char *UnitTest_qstr(const char *str);

void runUnitTest(void);

#define UNIT_TEST	void runUnitTest(void)

#define	EXPECT_TRUE(expr)	\
UnitTest_result(__FILE__,__LINE__, (expr), "%s", # expr)

#define	EXPECT_FALSE(expr)	\
UnitTest_result(__FILE__,__LINE__, !(expr), "!(%s)", # expr)

#define	EXPECT_EQ(a, b)		EXPECT_TRUE(a == b)
#define	EXPECT_NE(a, b)		EXPECT_TRUE(a != b)

#define EXPECT_STRCMP(a, op, b)	\
UnitTest_result(__FILE__, __LINE__, \
	UnitTest_strcmp(a, b) op 0, "%s %s %s", # a, # op, # b)

#define	EXPECT_STREQ(a, b)	EXPECT_STRCMP(a, ==, b)
#define	EXPECT_STRNE(a, b)	EXPECT_STRCMP(a, !=, b)

#endif
