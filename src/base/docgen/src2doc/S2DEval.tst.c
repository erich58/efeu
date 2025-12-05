#include <EFEU/UnitTest.h>
#include "src2doc.h"
#include <EFEU/utest/out.tst.h>

TEST_SUITE
{
	EXPECT(S2DEval_name(NULL) == NULL);
	EXPECT(S2DEval_name("*.h") == s2d_hdr);
	EXPECT(S2DEval_mode(NULL) == NULL);
	EXPECT(S2DEval_mode("hdr") == s2d_hdr);
	S2DEval_list(OUT());
	LMATCH_STR(utest_out(),
		"hdr\n"
		"xhdr\n"
		"src\n"
		"cmd\n"
		"sh\n"
		"std\n"
		"xstd\n"
		"script\n"
		"xscript\n"
		"com\n"
		"doc\n"
	);
	CLEANUP_OUT;
}
