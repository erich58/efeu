#include <EFEU/UnitTest.h>
#include <EFEU/mdmat.h>

static const char *dump_list(mdlist *list)
{
	static STRING_DECL(buf);
	buf.size = 0;

	for (mdlist *l = list; l; l = l->next)
	{
		UnitTest_putc('<', &buf);
		UnitTest_puts(l->name, &buf);
		UnitTest_putc('|', &buf);
		UnitTest_puts(l->option, &buf);
		UnitTest_putc('>', &buf);

		UnitTest_printf(&buf, " %d:", l->dim);

		for (size_t n = 0; n < l->dim; n++)
		{
			UnitTest_putc(' ', &buf);
			UnitTest_putc('<', &buf);
			UnitTest_puts(l->list[n], &buf);
			UnitTest_putc('|', &buf);
			UnitTest_puts(l->lopt[n], &buf);
			UnitTest_putc('>', &buf);
		}

		UnitTest_putc('\n', &buf);
	}

	UnitTest_putc(0, &buf);
	del_mdlist(list);
	return buf.data;
}

#define EXPECT_LIST(a, b)	\
UnitTest_strcheck(__FILE__, __LINE__, "==", dump_list(a), b, # a, # b)

TEST_SUITE
{
	SetupMdMat();
	SetProgName("ID");
	SetApplPath(UnitTest_tmp(NULL));
	TEST_FILE("mdmat.msg",
		"81	$!: Character $1 where '=' is expected.\n"
	);

	EXPECT_LIST(str2mdlist(NULL, 0), "");
	EXPECT_LIST(str2mdlist("name", MDLIST_NAMEOPT), "<name|> 0:\n");
	EXPECT_LIST(str2mdlist("name[]", MDLIST_NAMEOPT), "<name|> 0:\n");

	UnitTest_stderr("stderr");
	EXPECT_LIST(str2mdlist("name[]x", MDLIST_NAMEOPT), "");
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr",
		"MSG ID: Character 'x' where '=' is expected.\n");

	EXPECT_LIST(str2mdlist("name\\[\\]\\=", MDLIST_NAMEOPT),
		"<name[]=|> 0:\n");
	EXPECT_LIST(str2mdlist("name\\\\abc\\x\\", MDLIST_NAMEOPT),
		"<name\\abc\\x|> 0:\n");
	EXPECT_LIST(str2mdlist("name[o\\\\\\x[p]\\]t]", MDLIST_NAMEOPT),
		"<name|o\\\\x[p]]t> 0:\n");
	EXPECT_LIST(str2mdlist("name[o\\\\\\x[p]\\]t\\", MDLIST_NAMEOPT),
		"<name|o\\\\x[p]]t> 0:\n");
	EXPECT_LIST(str2mdlist("name[opt]", 0),
		"<name[opt]|> 0:\n");

	EXPECT_LIST(str2mdlist("a b c", 0),
		"<a|> 0:\n"
		"<b|> 0:\n"
		"<c|> 0:\n"
	);

	EXPECT_LIST(str2mdlist("a=a,b,c x=x[1],y[2]", MDLIST_LISTOPT),
		"<a|> 3: <a|> <b|> <c|>\n"
		"<x|> 2: <x|1> <y|2>\n"
	);

	EXPECT_LIST(str2mdlist("a=a,b,c x=x[1],y[2]", 0),
		"<a|> 3: <a|> <b|> <c|>\n"
		"<x|> 2: <x[1]|> <y[2]|>\n"
	);

	EXPECT_LIST(str2mdlist("a=a[opt],:b[opt],:c", MDLIST_NEWOPT),
		"<a|> 3: <a[opt]|> <:b|opt> <:c|>\n"
	);
}
