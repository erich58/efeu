#include <EFEU/UnitTest.h>
#include "efeudoc.h"
#include <EFEU/utest/in.tst.h>

static DocType doc_test = { "test", NULL, &DocOut_test, NULL, NULL };

TEST_SUITE
{
	SetupDoc();
	IO *out = DocOut(&doc_test, UnitTest_tmp("output")); 
	DocVerb(IN("abc ||xyz|eod"), out);
	io_close(out);

	EXPECT_CONTENT("output", "<+tt>abc \\|xyz<-tt>");
	CLEANUP_IN;
}
