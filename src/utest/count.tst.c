#include <EFEU/UnitTest.h>
#include <unistd.h>
#include <stdlib.h>

static const char *cmd = 0;
static const char *arg = 0;

TEST_DECL(test_arg)
{
	if	(*arg == '#')
	{
		++arg;
		PRINT_STR(arg);
		return;
	}

	ASSERT(*arg != 0);
	EXPECT_STR(arg, "good");
}

#define	MAXARG	10

static int test(const char *first, ...)
{
	UnitTest_stdout("out");
	UnitTest_stderr("err");

	TEST_FORK
	{
		char *arg[MAXARG + 1];
		int n = 0;
		arg[n++] = "TEST";
		arg[n++] = (char *) first;

		va_list list;
		va_start(list, first);

		while (n < MAXARG && (arg[n] = va_arg(list, char *)))
			++n;

		va_end(list);
		arg[n++] = NULL;

		//execlp(cmd, "TEST", first, second, third, NULL);
		execvp(cmd, arg);
		perror(cmd);
		exit(1);
	}

	int res = UnitTest_wait();
	UnitTest_stderr(NULL);
	UnitTest_stdout(NULL);
	return res;
}

#define	CHECK(cmd, res, out, err)	\
	EXPECT_INT(cmd, res); \
UnitTest_linematch(__FILE__, __LINE__, UnitTest_content("out"), \
	"@fussy\n" out, "stdout"); \
UnitTest_linematch(__FILE__, __LINE__, UnitTest_content("err"), \
	"@sloppy\n" err, "stderr")

TEST_SUITE
{
	if	(UnitTest_argc > 1)
	{
		for (int i = 1; i < UnitTest_argc; ++i)
		{
			arg = UnitTest_argv[i];

			if	(*arg == '!')
			{
				++arg;
				TEST_EXEC(test_arg);
			}
			else if	(*arg == '@')
			{
				++arg;
				TEST_RUN(test_arg);
			}
			else if	(*arg == '#')
			{
				++arg;
				PRINT_STR(arg);
			}
			else
			{
				ASSERT(*arg != 0);
				EXPECT_STR(arg, "good");
			}
		}

		return;
	}

	cmd = UnitTest_argv[0];
	CHECK(test("good", "good", "good", NULL), 0,
		"TEST\t1\t1\t0\tSUCCESS\n",
		"GOOD\t.*arg != 0\n"
		"GOOD\t.*arg == .good.\n"
		"GOOD\t.*arg != 0\n"
		"GOOD\t.*arg == .good.\n"
		"GOOD\t.*arg != 0\n"
		"GOOD\t.*arg == .good.\n"
		"^$\n"
	);
	CHECK(test("good", "bad", "", "good", NULL), 2,
		"TEST\t1\t0\t1\tABORT\n",
		"GOOD\t.*arg != 0\n"
		"GOOD\t.*arg == .good.\n"
		"GOOD\t.*arg != 0\n"
		"BAD\t.*arg == .good.\n"
		"BAD\t.*arg != 0\n"
		"ABORT\n"
		"^$\n"
	);
	CHECK(test("good", "@good", "good", NULL), 0,
		"TEST\t2\t2\t0\tSUCCESS\n",
		"GOOD\t.*arg != 0\n"
		"GOOD\t.*arg == .good.\n"
		"#START\t.*test_arg\n"
		"GOOD\t.*arg != 0\n"
		"GOOD\t.*arg == .good.\n"
		"SUCCESS\t.*test_arg\n"
		"^$\n"
		"GOOD\t.*arg != 0\n"
		"GOOD\t.*arg == .good.\n"
		"^$\n"
	);
	CHECK(test("good", "bad", "#mark", NULL), 1,
		"TEST\t1\t0\t1\tFAILURE\n",
		"GOOD\t.*arg != 0\n"
		"GOOD\t.*arg == .good.\n"
		"GOOD\t.*arg != 0\n"
		"BAD\t.*arg == .good.\n"
		"INFO\t.*arg -> .mark.\n"
		"^$\n"
	);
	CHECK(test("@good", "@bad", "@#mark", NULL), 1,
		"TEST\t2\t1\t1\tFAILURE\n",
		"#START\t.*test_arg\n"
		"GOOD\t.*arg != 0\n"
		"GOOD\t.*arg == .good.\n"
		"SUCCESS\t.*test_arg\n"
		"^$\n"
		"#START\t.*test_arg\n"
		"GOOD\t.*arg != 0\n"
		"BAD\t.*arg == .good.\n"
		"FAILURE\t.*test_arg\n"
		"^$\n"
		"#START\t.*test_arg\n"
		"INFO\t.*arg -> .mark.\n"
		"REPORT\t.*test_arg\n"
		"^$\n"
		"^$\n"
	);
	CHECK(test("@good", "@", "@bad", "@#mark", NULL), 2,
		"TEST\t2\t1\t1\tABORT\n",
		"#START\t.*test_arg\n"
		"GOOD\t.*arg != 0\n"
		"GOOD\t.*arg == .good.\n"
		"SUCCESS\t.*test_arg\n"
		"^$\n"
		"#START\t.*test_arg\n"
		"BAD\t.*arg != 0\n"
		"ABORT\n"
		"^$\n"
	);
	CHECK(test("!good", "!", "!bad", "!#mark", NULL), 1,
		"TEST\t4\t2\t2\tFAILURE\n",
		"#EXEC\t.*test_arg\n"
		".\t#START\t.*test_arg\n"
		".\tGOOD\t.*arg != 0\n"
		".\tGOOD\t.*arg == .good.\n"
		".\tSUCCESS\t.*test_arg\n"
		"SUCCESS\t.*test_arg\n"
		"^$\n"
		"#EXEC\t.*test_arg\n"
		".\t#START\t.*test_arg\n"
		".\tBAD\t.*arg != 0\n"
		".\tABORT\n"
		"TEST\t1\t0\t1\tABORT\n"
		"^.\t$\n"
		"FAILURE\t.*test_arg\n"
		"^$\n"
		"#EXEC\t.*test_arg\n"
		".\t#START\t.*test_arg\n"
		".\tGOOD\t.*arg != 0\n"
		".\tBAD\t.*arg == .good.\n"
		".\tFAILURE\t.*test_arg\n"
		"FAILURE\t.*test_arg\n"
		"^$\n"
		"#EXEC\t.*test_arg\n"
		".\t#START\t.*test_arg\n"
		".\tINFO\t.*arg -> .mark.\n"
		".\tREPORT\t.*test_arg\n"
		"SUCCESS\t.*test_arg\n"
		"^$\n"
		"^$\n"
	);

	CHECK(test("@good", "!good", NULL), 0,
		"TEST\t2\t2\t0\tSUCCESS\n",
		"#START\t.*test_arg\n"
		"GOOD\t.*arg != 0\n"
		"GOOD\t.*arg == .good.\n"
		"SUCCESS\t.*test_arg\n"
		"^$\n"
		"#EXEC\t.*test_arg\n"
		".\t#START\t.*test_arg\n"
		".\tGOOD\t.*arg != 0\n"
		".\tGOOD\t.*arg == .good.\n"
		".\tSUCCESS\t.*test_arg\n"
		"SUCCESS\t.*test_arg\n"
		"^$\n"
		"^$\n"
	);

	return;

	CHECK(test("@!", NULL, NULL), 1,
		"TEST\t1\t0\t1\tFAILURE\n",
		""
	);
}
