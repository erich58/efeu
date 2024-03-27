#include <EFEU/UnitTest.h>
#include "efeudoc.h"

TEST_SUITE
{
	SetupStd();
	Doc *doc;
	EXPECT_PTR(doc = Doc_create("term"));
	EXPECT_STR(rd_ident(doc),
		"type = \"term\", out = , stat = 0, indent = 0"
	);
	rd_deref(doc);
}
