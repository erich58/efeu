#ifndef EFEU_UnitTest_h
#define	EFEU_UnitTest_h	1

int UnitTest_result(const char *file, int line, int result,
	const char *fmt, ...);
int UnitTest_status(void);

void runUnitTest(void);

#define UNIT_TEST	void runUnitTest(void)

#define	EXPECT_TRUE(expr)	\
UnitTest_result(__FILE__,__LINE__, (expr), "%s", # expr)

#endif
