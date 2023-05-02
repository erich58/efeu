#include <EFEU/UnitTest.h>
#include <EFEU/procenv.h>
#include <EFEU/ftools.h>
#include <EFEU/Debug.h>
#include <EFEU/MakeDepend.h>
#include <EFEU/LangDef.h>

static LogControl *flog = NULL;

TEST_DECL(setup)
{
	SetApplPath(UnitTest_tmp(NULL));

	TEST_FILE("ftools.msg",
		"1	bad mode\n"
		"2	popen failed\n"
		"3	open failed\n"
		"4	pipe locked\n"
		"6	file $1 not found\n"
		"11	bad fileclose\n"
		"21	unexpected EOF\n"
		"22	write error\n"
	);

	TEST_FILE("test.msg",
		"1	$0: Message\n"
		"2	$0: Error\n"
	);

	EXPECT((flog = LogControl_create("flog.note")) != NULL);
	LogConfig("@:");
	TEST_EXPR(LogConfig(UnitTest_tpl("flog.note=@/flog")));
	TEST_EXPR(LogConfig(UnitTest_tpl("file.debug=@/debug")));
}

TEST_DECL(tempopen)
{
	FILE *file;
	TEST_FILE("debug", NULL);
	EXPECT((file = tempopen()) != NULL);
	fileclose(file);
	MATCH_LINES(UnitTest_content("debug"),
		"file: open.*<temp>.*\n"
		"file: close.*<temp>.*\n"
	);
}

TEST_DECL(filetab)
{
	FILE *file;
	
	EXPECT((file = fopen(UnitTest_tmp("test.data"), "w")) != NULL);
	EXPECT(fputs("Test data\n", file) > 0);
	fflush(file);
	EXPECT_STR(fileident(file), "<unknown>");
	TEST_EXPR(filenotice("test-file", "r", file, fclose));
	EXPECT_STR(fileident(file), "test-file");

	TEST_FILE("debug", NULL);
	TEST_FILE("flog", NULL);
	TEST_EXPR(filedebug(file, flog, "[test:1]", NULL));
	MATCH_CONTENT("flog", "test-file: Message\n");
	MATCH_LINES(UnitTest_content("debug"),
		"file: open.*/test.msg.*\n"
		"file: close.*/test.msg.*\n"
	);

	TEST_FILE("debug", NULL);
	UnitTest_stderr("stderr");

	TEST_FORK
	{
		TEST_EXPR(fileerror(file, "[test:2]", NULL));
		TEST_EXIT;
	}

	CHILD_EXIT(1);
	EXPECT_CONTENT("stderr", "ERR test-file: Error\n");
	MATCH_CONTENT("debug",
		"file: forced close.*test-file.*\n"
	);

	EXPECT(filerefer(file) == file);
	EXPECT(fileclose(file) == 0);
	EXPECT(fileclose(file) == 0);

	UnitTest_stderr("stderr");
	EXPECT(fileclose(file) == EOF);
	EXPECT_CONTENT("test.data", "Test data\n");
	EXPECT_CONTENT("stderr", "MSG bad fileclose\n");

	PRINT_LABEL("Default identifier");
	EXPECT_STR(fileident(NULL), NULL);
	EXPECT_STR(fileident(stdin), "<stdin>");
	EXPECT_STR(fileident(stdout), "<stdout>");
	EXPECT_STR(fileident(stderr), "<stderr>");

	PRINT_LABEL("No effect");
	EXPECT(fileclose(NULL) == 0);
	EXPECT(fileclose(stdin) == 0);
	EXPECT(fileclose(stdout) == 0);
	EXPECT(fileclose(stderr) == 0);
}

TEST_DECL(fileopen)
{
	FILE *file;
	char buf[512];

	PRINT_LABEL("Predefinied files");

	EXPECT(fileopen(NULL, "r") == stdin);
	EXPECT(fileopen("-", "r") == stdin);
	EXPECT(fileopen(NULL, "w") == stdout);
	EXPECT(fileopen("-", "w") == stdout);
	EXPECT(fileopen("&0", "r") == stdin);
	EXPECT(fileopen("&1", "w") == stdout);
	EXPECT(fileopen("&2", "w") == stderr);

	PRINT_LABEL("Errors");

	TEST_FORK
	{
		UnitTest_stderr("stderr");
		fileopen("/dev/null", "x");
		TEST_EXIT;
	}

	CHILD_EXIT(1);
	EXPECT_CONTENT("stderr", "ERR bad mode\n");

	TEST_FORK
	{
		UnitTest_stderr("stderr");
		fileopen("./bad/file/name", "r");
		TEST_EXIT;
	}

	CHILD_EXIT(1);
	EXPECT_CONTENT("stderr", "ERR open failed\n");

	PRINT_LABEL("Compressed data");

	ASSERT((file = fileopen(UnitTest_tmp("data.gz"), "wz")) != NULL);
	EXPECT_STR(fileident(file), "<!gzip>");
	EXPECT(fputs("Test data\n", file) > 0);
	EXPECT(fileclose(file) == 0);

	ASSERT((file = fileopen(UnitTest_tmp("data.gz"), "az")) != NULL);
	EXPECT(fputs("Second line\n", file) > 0);
	EXPECT(fileclose(file) == 0);

	ASSERT((file = fileopen(UnitTest_tmp("data.gz"), "rz")) != NULL);
	EXPECT_STR(fgets(buf, sizeof buf, file), "Test data\n");
	EXPECT_STR(fgets(buf, sizeof buf, file), "Second line\n");
	EXPECT_STR(fgets(buf, sizeof buf, file), NULL);
	EXPECT(fileclose(file) == 0);

	ASSERT((file = fileopen(UnitTest_tmp("data.gz"), "r")) != NULL);
	EXPECT_STR(fileident(file), UnitTest_tmp("data.gz"));
	EXPECT(getc(file) == 037);	// gzip magic
	EXPECT(getc(file) == 0213);	// gzip magic
	EXPECT(fileclose(file) == 0);

	PRINT_LABEL("Pipes");
	EXPECT(pipeopen(NULL, "r") == NULL);

	ASSERT((file = fileopen("|echo TEST", "r")) != NULL);
	EXPECT_STR(fileident(file), "<!echo>");
	EXPECT_STR(fgets(buf, sizeof buf, file), "TEST\n");
	EXPECT(fileclose(file) == 0);
	
	TEST_FORK
	{
		pipe_lock = 1;
		UnitTest_stderr("stderr");
		fileopen("|echo TEST", "r");
		TEST_EXIT;
	}

	CHILD_EXIT(1);
	EXPECT_CONTENT("stderr", "ERR pipe locked\n");

	UnitTest_stderr("stderr");
	EXPECT((file = fileopen("|bad-command-name", "r")) != NULL);
	EXPECT(fileclose(file) != 0);
	MATCH_CONTENT("stderr", "sh:.*bad-command-name.*\n");

	MakeDepend = 1;
	ASSERT((file = fileopen(UnitTest_tmp("data.gz"), "rzd")) != NULL);
	EXPECT_STR(fgets(buf, sizeof buf, file), "Test data\n");
	fileclose(file);

	ASSERT((file = fileopen(UnitTest_tmp("output"), "wd")) != NULL);
	EXPECT(fputs("Test data\n", file) > 0);
	EXPECT(fileclose(file) == 0);
	EXPECT_CONTENT("output", NULL);
}

TEST_DECL(findopen)
{
	char *path;
	FILE *file;

	TEST_EXPR(path = mstrcpy(UnitTest_tmp(NULL)));
	TEST_DIR("sub");
	TEST_FILE("test.txt", "data\n");

	EXPECT((file = findopen(path, NULL, "test", "txt", "r")) != NULL);
	fileclose(file);
	EXPECT((file = findopen(path, NULL, "test.txt", "txt", "r")) != NULL);
	fileclose(file);
	EXPECT((file = findopen(path, NULL, "test.txt", NULL, "r")) != NULL);
	fileclose(file);
	EXPECT((file = findopen(path, NULL, "|echo TEST", "txt", "r")) != NULL);
	fileclose(file);
	
	TEST_FORK
	{
		UnitTest_stderr("stderr");
		findopen(path, "sub", "test", "dat", "r");
		TEST_EXIT;
	}

	CHILD_EXIT(1);
	EXPECT_CONTENT("stderr", "ERR file test.dat not found\n");
}


static const char *t_fsearch(const char *path, const char *sub,
	const char *name, const char *ext)
{
	char *m = fsearch(UnitTest_tpl(path), sub, name, ext);
	const char *c = UnitTest_cstr(m);
	memfree(m);
	return c;
}

#define	TMP(x)	UnitTest_tmp(x)

TEST_DECL(fsearch)
{
	char *home = mstrpaste("/", getenv("HOME"), "");

	TEST_EXPR(SetLangDef("en_US"));
	TEST_EXPR(setenv("TERM", "xterm", 1));
	TEST_FILE("en/sub/a", "");
	TEST_FILE("sub/a", "");
	TEST_FILE("sub/b", "");
	TEST_FILE("c", "");
	TEST_FILE("c.x", "");
	TEST_FILE(".x", "");
	TEST_FILE("..x", "");
	TEST_DIR(home);

	EXPECT_STR(t_fsearch("~", NULL, "", NULL), home);
	EXPECT_STR(t_fsearch("%H", NULL, "", NULL), home);
	EXPECT_STR(t_fsearch("@/~", NULL, "", NULL), TMP(home));
	EXPECT_STR(t_fsearch("@/%H", NULL, "", NULL), TMP(home));

	EXPECT_STR(t_fsearch("@/%L/%S", "sub", "a", NULL), TMP("en/sub/a")); 
	EXPECT_STR(t_fsearch("@/%S", "sub", "a", NULL), TMP("sub/a")); 
	EXPECT_STR(t_fsearch("@/%S:@/%L/%S", "sub", "a", NULL), TMP("sub/a")); 
	EXPECT_STR(t_fsearch("@%L", "xxx", "sub/a", NULL), TMP("sub/a")); 
	EXPECT_STR(t_fsearch("@/%L/%S", "sub", "b", NULL), TMP("sub/b")); 
	EXPECT_STR(t_fsearch("@/%L/%S", "sub", "c", NULL), NULL);
	EXPECT_STR(t_fsearch("@/%L/%S:@", "sub", "c", NULL), TMP("c"));
	EXPECT_STR(t_fsearch("@/%L/%S:@", "sub", "c", "x"), TMP("c.x"));
	EXPECT_STR(t_fsearch("@/%L/%S:@", "sub", "c.x", "x"), TMP("c.x"));
	EXPECT_STR(t_fsearch("@/%L/%S:@", "sub", "c.a", "x"), NULL);

	TEST_FILE("xterm/x", "");
	TEST_FILE("x", "");
	TEST_FILE("y", "");
	EXPECT_STR(t_fsearch("@/%T", NULL, "x", NULL), TMP("xterm/x")); 
	EXPECT_STR(t_fsearch("@/%T", NULL, "y", NULL), NULL);

	TEST_FILE("root/a/b", "");
	EXPECT_STR(t_fsearch("@~root/a", NULL, "b", NULL), TMP("root/a/b")); 

	EXPECT_STR(t_fsearch("@", NULL, "/bad/file/name", NULL), NULL);
	EXPECT_STR(t_fsearch("@", NULL, ".", NULL), ".");
	EXPECT_STR(t_fsearch("@", NULL, "./", NULL), "./");
	EXPECT_STR(t_fsearch("@", NULL, ".x", NULL), TMP(".x"));
	EXPECT_STR(t_fsearch("@", NULL, "/", NULL), "/");
	EXPECT_STR(t_fsearch("@", NULL, "..", NULL), "..");
	EXPECT_STR(t_fsearch("@", NULL, "../", NULL), "../");
	EXPECT_STR(t_fsearch("@", NULL, "..x", NULL), TMP("..x"));
	EXPECT_STR(t_fsearch("@", NULL, NULL, NULL), NULL);
}

TEST_SUITE
{
	TEST_RUN(setup);
	TEST_RUN(tempopen);
	TEST_RUN(filetab);
	TEST_RUN(fileopen);
	TEST_RUN(findopen);
	TEST_RUN(fsearch);
}
