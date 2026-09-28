#include <EFEU/UnitTest.h>
#include <EFEU/procenv.h>
#include <EFEU/ftools.h>

#define	READ_ERROR(expr, msg)	\
UnitTest_stderr("stderr"); \
TEST_FORK { FILE *file = fileopen("/dev/null", "r"); expr; exit(0); } \
CHILD_EXIT(1); \
EXPECT_CONTENT("stderr", msg)

TEST_DECL(setup)
{
	SetProgName("ID");
	SetApplPath(UnitTest_tmp(NULL));
	TEST_FILE("ftools.msg",
		"21	NOTE 21: arg0=$0\n"
		"22	NOTE 22: arg0=$0\n"
		"61	NOTE 61: arg1=$1.\n"
		"write	$!: $0 $# $*\n"
	);
}

TEST_DECL(newtemp)
{
	char *tmp = mstrcpy(UnitTest_tmp(NULL));
	char *f1, *f2, *d;
	MATCH_STR((f1 = newtemp(tmp, "TMP_")), "/TMP_[0-9]*a");
	MATCH_STR((d = newtempdir(tmp, "TMP_")), "/TMP_[0-9]*b");
	MATCH_STR((f2 = newtemp(tmp, "TMP_")), "/TMP_[0-9]*c");
	UnitTest_stderr("stderr");
	TEST_EXPR(tempstat());
	MATCH_CONTENT("stderr",
		"0: dir .*/TMP_[0-9]*b\n"
		"0: file .*/TMP_[0-9]*a\n"
		"1: file .*/TMP_[0-9]*c\n"
	);

	UnitTest_stderr("stderr");
	EXPECT(deltemp("x") == 0);
	EXPECT_CONTENT("stderr", "deltemp: unknown path x.\n");

	EXPECT(deltemp(NULL) == 1);
	EXPECT(deltempdir(NULL) == 1);
	EXPECT(deltemp(f1) == 1);
	EXPECT(deltempdir(d) == 1);

	TEST_FORK
	{
		UnitTest_stderr("stderr");
		exit(0);
	}

	CHILD_EXIT(0);
	MATCH_CONTENT("stderr", "remove .*/TMP_[0-9]*c\n");

	UnitTest_stderr("stderr");
	EXPECT(deltemp(f2) == 0);
	MATCH_CONTENT("stderr", "/TMP_[0-9]*c: ");

	setenv("TMPDIR", UnitTest_tmp("tmp"), 1);
	PRINT_STR(f1 = newtemp(NULL, "TMP_"));
	EXPECT(deltemp(f1) == 1);
}

TEST_DECL(rfread)
{
	char buf[32];
	FILE *file;
	TEST_FILE("data", "ABCD1234XYZW");
	file = fopen(UnitTest_tmp("data"), "r+b");
	EXPECT(rfread(buf, 4, 3, file) == 3);
	buf[12] = 0;
	EXPECT_STR(buf, "DCBA4321WZYX");

	rewind(file);
	EXPECT(rfwrite(buf, 1, 12, file) == 12);
	fflush(file);
	EXPECT_CONTENT("data", "DCBA4321WZYX");

	rewind(file);
	EXPECT(rfwrite(buf, 2, 6, file) == 6);
	fclose(file);
	EXPECT_CONTENT("data", "CDAB3412ZWXY");
}

TEST_DECL(getbyte)
{
	FILE *file = fopen(UnitTest_tmp("data"), "w+");
	TEST_EXPR(put1byte(0xA1, file));
	TEST_EXPR(put2byte(0xB1B2, file));
	TEST_EXPR(put3byte(0xC1C2C3, file));
	TEST_EXPR(put4byte(0xD1D2D3D4, file));
	fflush(file);
	rewind(file);

	EXPECT(get1byte(file) == 0xA1);
	EXPECT(get2byte(file) == 0xB1B2);
	EXPECT(get3byte(file) == 0xC1C2C3);
	EXPECT(get4byte(file) == 0xD1D2D3D4);
	fclose(file);
	EXPECT_CONTENT("data", "\241\261\262\301\302\303\321\322\323\324");

	READ_ERROR(get1byte(file), "getxbyte: Unerwartetes Fileende.\n");
	READ_ERROR(get2byte(file), "getxbyte: Unerwartetes Fileende.\n");
	READ_ERROR(get3byte(file), "getxbyte: Unerwartetes Fileende.\n");
	READ_ERROR(get4byte(file), "getxbyte: Unerwartetes Fileende.\n");
}

TEST_DECL(savestr_error)
{
	UnitTest_stderr("stderr");

	TEST_FORK
	{
		char buf[4];
		FILE *file = UnitTest_strout(buf, sizeof buf);
		filenotice("output", "w", file, NULL);
		savestr("abc", file);
		TEST_EXIT;
	}

	CHILD_EXIT(1);
	EXPECT_CONTENT("stderr", "ERR ID: output 0 \n");
}

TEST_DECL(loadstr)
{
	FILE *file = fopen(UnitTest_tmp("data"), "w+");

	savestr(NULL, file);
	savestr("", file);
	savestr("a", file);
	savestr("xy", file);
	fflush(file);
	rewind(file);
	EXPECT_STR(loadstr(file), NULL);
	EXPECT_STR(loadstr(file), "");
	EXPECT_STR(loadstr(file), "a");
	EXPECT_STR(loadstr(file), "xy");
	fclose(file);

	READ_ERROR(loadstr(file), "getxbyte: Unerwartetes Fileende.\n");

	UnitTest_stderr("stderr");

	TEST_FORK
	{
		FILE *file = UnitTest_strin("\000\100", 2);
		filenotice("input", "r", file, NULL);
		loadstr(file);
		TEST_EXIT;
	}

	CHILD_EXIT(1);
	EXPECT_CONTENT("stderr", "loadstr: Unerwartetes Fileende.\n");
}

TEST_DECL(dbdata)
{
	int wd1[3] = { 0, 12345, ~0 };
	int rd1[3] = { 0, 0, 0 };
	FILE *file = fopen(UnitTest_tmp("data"), "w+");
	EXPECT(dbwrite(file, wd1, sizeof wd1[0], 3) == 3);
	fflush(file);
	rewind(file);
	EXPECT(dbread(file, rd1, sizeof rd1[0], 3) == 3);
	fclose(file);
	EXPECT(memcmp(wd1, rd1, sizeof wd1) == 0);

	int wd[4] = { 0201, 0202, 0203, 0204 };
	int rd[4] = { 0, 0, 0, 0 };
	file = fopen(UnitTest_tmp("data"), "w+");
	EXPECT(dbwrite(file, wd, sizeof wd[0], 4) == 4);
	fflush(file);
	rewind(file);
	EXPECT(dbread(file, rd, sizeof rd[0], 4) == 4);
	EXPECT(memcmp(wd, rd, sizeof wd) == 0);
	fclose(file);

	READ_ERROR(dbread(file, rd, sizeof rd[0], 4),
		"ERR NOTE 21: arg0=/dev/null\n");
}

TEST_DECL(dbdata_error)
{
	UnitTest_stderr("stderr");

	TEST_FORK
	{
		char buf[2];
		FILE *file = UnitTest_strout(buf, sizeof buf);
		filenotice("output", "w", file, NULL);
		dbwrite(file, "data", 1, 4);
		TEST_EXIT;
	}

	CHILD_EXIT(1);
	EXPECT_CONTENT("stderr", "ERR NOTE 22: arg0=output\n");
}

TEST_DECL(fpeek)
{
	TEST_FILE("data", "ab");
	FILE *file = fopen(UnitTest_tmp("data"), "r");
	EXPECT(fpeek(file) == 'a');
	EXPECT(getc(file) == 'a');
	EXPECT(getc(file) == 'b');
	EXPECT(fpeek(file) == EOF);
	EXPECT(getc(file) == EOF);
	fclose(file);
}

TEST_SUITE
{
	TEST_RUN(setup);
	TEST_RUN(newtemp);
	TEST_RUN(rfread);
	TEST_RUN(getbyte);
	TEST_RUN(savestr_error);
	TEST_RUN(loadstr);
	TEST_RUN(dbdata);
	TEST_RUN(dbdata_error);
	TEST_RUN(fpeek);
}
