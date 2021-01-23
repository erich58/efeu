#include <EFEU/UnitTest.h>
#include <EFEU/object.h>
#include <EFEU/EDB.h>
#include <EFEU/EDBMeta.h>
#include "utest/out.tst.h"

TEST_DECL(list)
{
	InfoNode root = STD_INFO(NULL, NULL);
	EDBMetaInfo(&root);
	InfoNode *info;
	EXPECT_PTR(info = GetInfo(&root, "meta"));
	PrintInfo(OUT(), info);
	MATCH_OUT(".");

	EDBMetaDef *mdef;
	EXPECT_PTR(mdef = GetEDBMetaDef("data"));
	ShowEDBMetaDef(OUT(), mdef);
	EXPECT_OUT("data\tstart of data section\n");
}

TEST_DECL(show)
{
	static EDBMetaDef mdef = { "test", NULL, 0,
		"Description name=$1 ($$-$2-$x)"
	};

	ShowEDBMetaDef(OUT(), &mdef);
	EXPECT_OUT("test\tDescription name=test ($-2-x)\n");
}

TEST_SUITE
{
	TEST_RUN(list);
	TEST_RUN(show);
	CLEANUP_OUT;
}
