#include <EFEU/UnitTest.h>
#include <stdlib.h>

#define	EDBDIFF	BUILD "/edbdiff"

TEST_DECL(setup)
{
	setenv("LANG", "C", 1);
	PRINT_LABEL("Force separating UnitTest output from cmd output");
	UnitTest_stderr("stderr");
	UnitTest_stderr(NULL);
}

TEST_DECL(synopsis)
{
	TEST_FORK
	{
		setenv("TERM", "dumb", 1);
		setenv("COLUMNS", "60", 1);
		UnitTest_reopen(2, "stderr");
		CMD_EXEC(EDBDIFF, "edbdiff");
	}

	CHILD_EXIT(1);
	LMATCH_CONTENT("stderr", "SYNOPSIS\n");
}

TEST_DECL(method)
{
	TEST_FORK
	{
		UnitTest_reopen(2, "stderr");
		CMD_EXEC(EDBDIFF, "edbdiff int: int: -d?");
	}

	CHILD_EXIT(0);
	LMATCH_CONTENT("stderr",
		"std\n"
		"mark\n"
		"diff\n"
	);

	TEST_FORK
	{
		UnitTest_reopen(2, "stderr");
		CMD_EXEC(EDBDIFF, "edbdiff int: int: -d unknown");
	}

	CHILD_EXIT(1);
	EXPECT_CONTENT("stderr",
		"ERR edbdiff: Unbekannte Darstellungsfunktion unknown.\n"
	);
}

#define	RUN_EDBDIFF(args)	\
TEST_FORK { CMD_EXEC(EDBDIFF, "edbdiff " args); } \
CHILD_EXIT(0);

TEST_DECL(header)
{
	TEST_FILE("type.esh",
		"struct DATA { int x; };\n"
	);

	TEST_FILE("a.edb", UnitTest_tpl(
		"EDB 1.0\n"
		"//Title\n"
		"\\@head\n"
		"include(\"@/type.esh\");\n"
		"\\@type\n"
		"DATA\n"
		"\\@data plain\n"
		"42\n"
	));

	RUN_EDBDIFF("-p plain -d std @/a.edb int: @/out");
	PRINT_CONTENT("out");
	LMATCH_CONTENT("out",
		"[*]{3} .*a.edb\n"
		"[-]{3} int:\n"
		"[*]{3} header [*]{3}\n"
		"[-]{3} header [-]{3}\n"
		"[*]{3} desc [*]{3}\n"
		"[*]{3} head [*]{3}\n"
		"[*]{3} type [*]{3}\n"
		"[-]{3} type [-]{3}\n"
	);

	RUN_EDBDIFF("-p plain -d mark @/a.edb int: @/out");
	LMATCH_CONTENT("out",
		"\\@ header\n"
		"\\@ desc\n"
		"\\@ head\n"
		"\\@ type\n"
	);

	RUN_EDBDIFF("-p plain -d diff @/a.edb int: @/out");
	LMATCH_CONTENT("out",
		"[-]{3} .*a.edb\n"
		"[+]{3} int:\n"
		"\\@@ header @@\n"
		"\\@@ desc @@\n"
		"\\@@ head @@\n"
		"\\@@ type @@\n"
	);

}

TEST_DECL(diff)
{
	TEST_FILE("a.edb",
		"EDB\t1.0\n"
		"int\n"
		"@data plain\n"
		"1\n"
		"2\n"
		"3\n"
		"4\n"
	);

	TEST_FILE("b.edb",
		"EDB\t1.0\n"
		"int\n"
		"@data plain\n"
		"1\n"
		"3\n"
		"4\n"
		"5\n"
		"6\n"
	);

	RUN_EDBDIFF("-p plain -d std -e2 @/a.edb @/b.edb @/out");
	LMATCH_CONTENT("out",
		"@fussy\n"
		"[*]{3} .*/a.edb\n"
		"[-]{3} .*/b.edb\n"
		"[*]{3} 2 [*]{3}\n"
		"2\n"
		"[-]{3} 2 [-]{3}\n"
		"3\n"
		"[*]{3} 3 [*]{3}\n"
		"3\n"
		"[-]{3} 3 [-]{3}\n"
		"4\n"
	);

	RUN_EDBDIFF("-p plain -d std -n2 @/a.edb @/b.edb @/out");
	LMATCH_CONTENT("out",
		"@fussy\n"
		"[*]{3} .*/a.edb\n"
		"[-]{3} .*/b.edb\n"
		"[*]{3} 2 [*]{3}\n"
		"2\n"
		"[-]{3} 2 [-]{3}\n"
		"3\n"
	);

	RUN_EDBDIFF("-p plain -d std -c. @/a.edb @/b.edb @/out");
	LMATCH_CONTENT("out",
		"@fussy\n"
		"[*]{3} .*/a.edb\n"
		"[-]{3} .*/b.edb\n"
		"[*]{3} -2 [*]{3}\n"
		"2\n"
		"[*]{3} EOF [*]{3}\n"
		"[-]{3} 4 [-]{3}\n"
		"5\n"
	);

	RUN_EDBDIFF("-p plain -d std -c. @/b.edb @/a.edb @/out");
	LMATCH_CONTENT("out",
		"@fussy\n"
		"[*]{3} .*/b.edb\n"
		"[-]{3} .*/a.edb\n"
		"[-]{3} [+]2 [-]{3}\n"
		"2\n"
		"[*]{3} 4 [*]{3}\n"
		"5\n"
		"[-]{3} EOF [-]{3}\n"
	);

	RUN_EDBDIFF("-p plain -d mark @/a.edb @/b.edb @/out");
	PRINT_CONTENT("out");
	LMATCH_CONTENT("out",
		"@fussy\n"
		"2c2\n"
		"< 2\n"
		"> 3\n"
		"3c3\n"
		"< 3\n"
		"> 4\n"
		"4c4\n"
		"< 4\n"
		"> 5\n"
		"A5,[$]\n"
		"> 6\n"
	);

	RUN_EDBDIFF("-p plain -d mark -c. @/a.edb @/b.edb @/out");
	LMATCH_CONTENT("out",
		"@fussy\n"
		"2d\n"
		"< 2\n"
		"A4,[$]\n"
		"> 5\n"
	);

	RUN_EDBDIFF("-p plain -d mark -c. @/b.edb @/a.edb @/out");
	LMATCH_CONTENT("out",
		"@fussy\n"
		"a2\n"
		"> 2\n"
		"4,[$]D\n"
		"< 5\n"
	);

	RUN_EDBDIFF("-p plain -d diff @/a.edb @/b.edb @/out");
	LMATCH_CONTENT("out",
		"@fussy\n"
		"[-]{3} .*/a.edb\n"
		"[+]{3} .*/b.edb\n"
		"\\@@ \\-2 \\+2 @@\n"
		"[-]2\n"
		"[+]3\n"
		"\\@@ \\-3 \\+3 @@\n"
		"[-]3\n"
		"[+]4\n"
		"\\@@ \\-4 \\+4 @@\n"
		"[-]4\n"
		"[+]5\n"
		"\\@@ \\-EOF \\+5 @@\n"
		"[+]6\n"
	);

	RUN_EDBDIFF("-p plain -d diff -c . @/a.edb @/b.edb @/out");
	LMATCH_CONTENT("out",
		"[-]{3} .*/a.edb\n"
		"[+]{3} .*/b.edb\n"
		"\\@@ \\-2 @@\n"
		"[-]2\n"
		"\\@@ \\-EOF \\+4 @@\n"
		"[+]5\n"
	);

	RUN_EDBDIFF("-p plain -d diff -c . @/b.edb @/a.edb @/out");
	LMATCH_CONTENT("out",
		"[-]{3} .*/b.edb\n"
		"[+]{3} .*/a.edb\n"
		"\\@@ \\+2 @@\n"
		"[+]2\n"
		"\\@@ \\-4 \\+EOF @@\n"
		"[-]5\n"
	);
}

TEST_SUITE
{
	TEST_RUN(setup);
	TEST_RUN(synopsis);
	TEST_RUN(method);
	TEST_RUN(header);
	TEST_RUN(diff);
}
