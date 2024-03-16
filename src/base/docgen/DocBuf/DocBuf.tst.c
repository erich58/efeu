#include <EFEU/UnitTest.h>
#include <EFEU/mstring.h>
#include "DocBuf.h"
#include "utest/out.tst.h"

TEST_DECL(protect)
{
	copy_protect("<abc> [|]", OUT());
	EXPECT_OUT("\\<abc\\> \\[\\|]");
	copy_protect("\\ * # $ &", OUT());
	EXPECT_OUT("\\\\ \\* \\# \\$ \\&");
	copy_protect("a/b//c", OUT());
	EXPECT_OUT("a/b\\//c");
}


TEST_DECL(DocBuf)
{
	DocBuf doc;
	DocBuf_init(&doc);
	EXPECT(doc.cmdpar == NULL);

	doc.var[VAR_NAME] = mstrcpy("The Name");
	doc.var[VAR_TITLE] = mstrcpy("The Title");
	doc.var[VAR_HEAD] = mstrcpy("The Header");
	doc.var[VAR_COPYRIGHT] = mstrcpy("The Copyright");

	sb_puts("The Synopsis\n", doc.synopsis);
	sb_puts("The Source\n", doc.source);
	sb_puts("The Syntax\n", doc.tab[BUF_SYN]);
	sb_puts("The Description\n", doc.tab[BUF_DESC]);
	sb_puts("The Examples\n", doc.tab[BUF_EX]);
	sb_puts("The References\n", doc.tab[BUF_SEE]);
	sb_puts("The Diagnostics\n", doc.tab[BUF_DIAG]);
	sb_puts("The Notes\n", doc.tab[BUF_NOTE]);
	sb_puts("The Warnings\n", doc.tab[BUF_WARN]);
	sb_puts("The Errors\n", doc.tab[BUF_ERR]);

	DocBuf_write(&doc, OUT());
	EXPECT_OUT(
		"\\Name\n"
		"The Name -- The Title\n"
		"\\Synopsis\n"
		"\\code\n"
		"\\#include The Header\n"
		"\n"
		"The Synopsis\n"
		"\\end\n"
		"\n"
		"The Syntax\n"
		"\\Description\n"
		"The Description\n"
		"\\Examples\n"
		"The Examples\n"
		"\\SeeAlso\n"
		"The References\n"
		"\\Diagnostics\n"
		"The Diagnostics\n"
		"\\Notes\n"
		"The Notes\n"
		"\\Warnings\n"
		"The Warnings\n"
		"\\Errors\n"
		"The Errors\n"
		"\\Copyright\n"
		"Copyright The Copyright\n"
	);
}

TEST_SUITE
{
	TEST_RUN(protect);
	TEST_RUN(DocBuf);
	CLEANUP_OUT;
}
