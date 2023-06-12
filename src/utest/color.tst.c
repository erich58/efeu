#include <EFEU/UnitTest.h>
#include <unistd.h>
#include <stdlib.h>

TEST_SUITE
{
	if	(UnitTest_argc == 1)
	{
		TEST_FORK
		{
			int fdsave = dup(2);
			UnitTest_reopen(2, UnitTest_tmp("err"));
			EXPECT_STR(UnitTest_color(1), "{");
			EXPECT_STR(UnitTest_color(2), "{");
			EXPECT_STR(UnitTest_color(3), "{");
			EXPECT_STR(UnitTest_color(0), "}");
			dup2(fdsave, 2);
			PRINT_CONTENT("err");
			TEST_EXIT;
		}

		CHILD_EXIT(0);

		PRINT_LABEL("Restart with color flag");
		execlp(UnitTest_argv[0], UnitTest_name, "-c", "arg", NULL);
	}

	TEST_FORK
	{
		UnitTest_mkfile("tput",
			"#!/bin/sh\n"
			"case \"$1$2\" in\n"
			"setaf2) printf '<green:';;\n"
			"setaf1) printf '<red:';;\n"
			"setaf4) printf '<blue:';;\n"
			"sgr0) printf '>';;\n"
			"esac\n"
		);

		system(UnitTest_tpl("chmod a+x @/tput"));

		STRING_DECL(path);
		UnitTest_puts(UnitTest_tmp(NULL), &path);
		UnitTest_putc(':', &path);
		UnitTest_puts(getenv("PATH"), &path);
		setenv("PATH", UnitTest_str(&path), 1);

		EXPECT_STR(UnitTest_color(1), "<green:");
		EXPECT_STR(UnitTest_color(2), "<red:");
		EXPECT_STR(UnitTest_color(3), "<blue:");
		EXPECT_STR(UnitTest_color(0), ">");

		UnitTest_info(__FILE__, __LINE__, "*LABEL", "green colored");
		UnitTest_info(__FILE__, __LINE__, "!LABEL", "red colored");
		UnitTest_info(__FILE__, __LINE__, "?LABEL", "blue colored");
	
		TEST_EXIT;
	}

	CHILD_EXIT(0);
}
