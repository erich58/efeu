#include <EFEU/UnitTest.h>

TEST_DECL(strout_nul)
{
	FILE *file = UnitTest_strout(NULL, 0);
	EXPECT(putc('a', file) == EOF);
	EXPECT(fclose(file) == EOF);
}

TEST_DECL(strout_buf)
{
	char buf[2];
	FILE *file = UnitTest_strout(buf, sizeof buf);
	EXPECT(putc('a', file) == 'a');
	EXPECT(putc('b', file) == EOF);
	EXPECT(fclose(file) == EOF);
	EXPECT_STR(buf, "a");
}

TEST_DECL(strin_nul)
{
	FILE *file = UnitTest_strin("xyz", 0);
	EXPECT(getc(file) == 'x');
	EXPECT(getc(file) == 'y');
	EXPECT(getc(file) == 'z');
	EXPECT(getc(file) == EOF);
	EXPECT(fclose(file) == 0);
}

TEST_DECL(strin_fix)
{
	char buf[2] = { 'a', 0 };
	FILE *file = UnitTest_strin(buf, sizeof buf);
	EXPECT(getc(file) == 'a');
	EXPECT(getc(file) == 0);
	EXPECT(getc(file) == EOF);
	EXPECT(fclose(file) == 0);
}
	
TEST_SUITE
{
	TEST_RUN(strout_nul);
	TEST_RUN(strout_buf);
	TEST_RUN(strin_nul);
	TEST_RUN(strin_fix);
}
