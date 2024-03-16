#include <EFEU/UnitTest.h>
#include <EFEU/object.h>
#include <EFEU/mstring.h>
#include "DocBuf.h"
#include "utest/in.tst.h"

static DocBuf doc;

static const char *copy(const char *def)
{
	static STRBUF(in, 1024);
	static STRBUF(out, 1024);
	sb_clear(&in);
	sb_puts(def, &in);
	sb_clear(&out);
	DocBuf_copy(&doc, &in, &out, "Name");
	return sb_cstr(&out);
}

TEST_DECL(init)
{
	setenv("LANG", "C", 1);
	SetupStd();
	DocBuf_init(&doc);
}

TEST_DECL(psub)
{
	EXPECT_STR(copy("\\$ \\/ \\* \\\\ \\x $$ $;\n"), "$ / * \\ \\x $ $;\n");
	EXPECT_STR(copy("Document $1\n"), "Document Name\n");
	EXPECT_STR(copy("${3*5}\n"), "15\n");
	EXPECT_STR(copy("$_unknown\nignored\n"), "");
}

TEST_DECL(set)
{
	DocBuf_init(&doc);
	doc.var[VAR_TITLE] = mstrcpy("The Title");
	sb_puts("The Synopsis\n", doc.synopsis);
	sb_puts("The Description\n", doc.tab[BUF_DESC]);

	EXPECT_STR(copy(
		"Prefix\n"
		"$title New title for $1\n"
		"$synopsis\n"
		"Additional synopsis\n"
		"$desc\n"
		"Additional description\n"
		"$undefined\n"
		"ignored text\n"
	), "Prefix\n");

	EXPECT_STR(doc.var[VAR_TITLE], "New title for Name");
	EXPECT_STR(sb_cstr(doc.synopsis),
		"The Synopsis\n"
		"\n\n"
		"Additional synopsis\n"
	);
	EXPECT_STR(sb_cstr(doc.tab[BUF_DESC]),
		"The Description\n"
		"\n\n"
		"Additional description\n"
	);
}

TEST_DECL(source)
{
	DocBuf_init(&doc);
	sb_puts("The Source\n", doc.source);
	EXPECT_STR(copy("Some text\n$source|nl\nAnother text\n"),
	"Some text\n"
	"---- verbatim|nl\n"
	"The Source\n"
	"----\n"
	"\n"
	"Another text\n"
	);
}

TEST_DECL(alias)
{
	EXPECT(AliasTab == NULL);

	EXPECT_STR(copy(
		"pre\n"
		"$alias abc\n"
		"text\n"
		"$alias\n"
		"post\n"
	), "pre\n\n\ntext\n\npost\n");

	EXPECT_STR(nkt_fetch(AliasTab, "Name", "x"), NULL);
	EXPECT_STR(nkt_fetch(AliasTab, "abc", "x"), NULL);
	EXPECT_STR(nkt_fetch(AliasTab, "undef", "x"), "x");
}

TEST_DECL(macro)
{
	MacDef_clean();
	MacDef_add(IN("A 1 //Macro 1\n"));
	MacDef_add(IN("B 2 //Macro 2\n"));

	EXPECT_STR(copy(
		"pre\n"
		"$maclist\n"
		"post\n"
	),	"pre\n"
		"\\ttitem A\n"
		"Macro 1\n"
		"\\ttitem B\n"
		"Macro 2\n"
		"\\endlist\n"
		"\n\n"
		"post\n"
	);
}

TEST_DECL(cmdpar)
{
	MacDef_clean();
	EXPECT_STR(copy(
		"$pconfig !\n"
		"f|\n"
		"\tflag\n"
		"o:opt|\n"
		"\toption\n"
		":arg|\n"
		"\targument\n"
		"!\n"
		"Command description\n"
		"$!\n"
		"Arguments\n"
		"@arglist\n"
		"Continuation.\n"
	), 
		"\n"
		"Command description\n"
		"\\index{|Name|}\n"
		"Arguments\n"
		"[<=|-f|=>]\n"
		"\tflag\n"
		"[<=|-o|~</opt/>=>]\n"
		"\toption\n"
		"[<=</arg/>=>]\n"
		"\targument\n"
		"\\end\n"
		"Continuation.\n"
	);
	EXPECT_STR(sb_cstr(doc.tab[BUF_SYN]),
		"\\hang\n"
		"|Name|\n"
		"\\[~|-f|~]\n"
		"\\[~|-o|~</opt/>~]\n"
		"</arg/>\n"
		"\\end\n"
	);
}

TEST_SUITE
{
	TEST_RUN(init);
	TEST_RUN(psub);
	TEST_RUN(set);
	TEST_RUN(source);
	TEST_RUN(alias);
	TEST_RUN(macro);
	TEST_RUN(cmdpar);
	CLEANUP_IN;
}
