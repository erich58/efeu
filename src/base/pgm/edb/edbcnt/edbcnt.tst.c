#include <EFEU/UnitTest.h>
#include <stdlib.h>

#define	EDBCNT	BUILD "/edbcnt"

#define	RUN_EDBCNT(args)	\
TEST_FORK { \
	UnitTest_reopen(1, "stdout"); \
	CMD_EXEC(EDBCNT, "edbcnt " args); \
} CHILD_EXIT(0);

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
		CMD_EXEC(EDBCNT, "edbcnt");
	}

	CHILD_EXIT(1);
	LMATCH_CONTENT("stderr", "SYNOPSIS\n");
}

TEST_DECL(info)
{
	RUN_EDBCNT("int: -ixdump");
	EXPECT_FILE("stdout",
		"@: count definitions\n"
		"@class: grouping classes\n"
		"@class/total: Totality\n"
		"TOTAL\tTotality\n"
		"@count: counter types\n"
		"@count/count: standard counter\n"
		"int\n"
		"@count/wcount: weighted count\n"
		"double\n"
	);

	RUN_EDBCNT("int: -l*");
	EXPECT_CONTENT("stdout",
		"total[1]\tTotality\n"
		"\tTOTAL\tTotality\n"
	);
}

TEST_DECL(counter)
{
	RUN_EDBCNT("int: -c?");
	EXPECT_CONTENT("stdout",
		"count\tstandard counter\n"
		"wcount\tweighted count\n"
	);

	TEST_FORK
	{
		UnitTest_reopen(2, "stderr");
		CMD_EXEC(EDBCNT, "edbcnt int: -c unknown");
	}

	CHILD_EXIT(1);
	EXPECT_CONTENT("stderr",
		"ERR edbcnt: undefined counter unknown.\n"
	);
}

TEST_DECL(record)
{
	TEST_FILE("a.edb",
		"EDB\t1.0\n"
		"//EDB Description\n"
		"int x\n"
		"double val\n"
		"@data plain\n"
		"1;7.5\n"
		"5;3.8\n"
		"3;4.2\n"
	);

	RUN_EDBCNT("@/a.edb -Rplain -c?");
	EXPECT_FILE("stdout",
		"count\tstandard counter\n"
		"count.index\tcount index\n"
		"data\tEDB Description\n"
		"data.val\t\n"
		"data.x\t\n"
		"event\tevent number\n"
		"recnum\trecord number\n"
		"wcount\tweighted count\n"
		"weight\tcount weight\n"
	);

	RUN_EDBCNT("@/a.edb -Rlabel -p2 -r2000..2002 -w2 "
		"-c count,wcount,weight,count.index,event,recnum,data @/out");
	EXPECT_FILE("out",
		"EDB\t1.0\n"
		"// edbcnt -e record -c count,wcount,weight,"
			"count.index,event,recnum,data -r 2000..2002\n"
		"enum : 1 {\n"
		"\t\"2000\" = 1,\n"
		"\t\"2001\" = 2,\n"
		"\t\"2002\" = 3,\n"
		"} jahr;\n"
		"int count;\t/* standard counter */\n"
		"double wcount;\t/* weighted count */\n"
		"double weight;\t/* count weight */\n"
		"int count_index;\t/* count index */\n"
		"unsigned event;\t/* event number */\n"
		"unsigned recnum;\t/* record number */\n"
		"struct {\n"
		"\tint x;\n"
		"\tdouble val;\n"
		"} data;\t/* EDB Description */\n"
		"@data label\n"
		"#jahr;count;wcount;weight;count_index;event;recnum;data\n"
		"\"2000\";1;2.00;2.00;0;1;0;{1, 7.50}\n"
		"\"2001\";1;2.00;2.00;1;2;0;{1, 7.50}\n"
		"\"2002\";1;2.00;2.00;2;3;0;{1, 7.50}\n"
		"\"2000\";1;2.00;2.00;3;4;3;{5, 3.80}\n"
		"\"2001\";1;2.00;2.00;4;5;3;{5, 3.80}\n"
		"\"2002\";1;2.00;2.00;5;6;3;{5, 3.80}\n"
		"\"2000\";1;2.00;2.00;6;7;6;{3, 4.20}\n"
		"\"2001\";1;2.00;2.00;7;8;6;{3, 4.20}\n"
		"\"2002\";1;2.00;2.00;8;9;6;{3, 4.20}\n"
	);
}

TEST_SUITE
{
	TEST_RUN(setup);
	TEST_RUN(synopsis);
	TEST_RUN(info);
	TEST_RUN(counter);
	TEST_RUN(record);
}
