#include <EFEU/UnitTest.h>
#include <EFEU/Info.h>
#include <EFEU/mstring.h>

static InfoNode root = STD_INFO("root", "root node");

static int cnt_setup = 0;

static void f_setup(InfoNode *info)
{
	cnt_setup++;
}

static void f_puts(IO *out, InfoNode *info)
{
	io_puts(info->par, out);
}

#include "utest/data.tst.h"

static void show_info(IO *out, InfoNode *info)
{
	if	(!info)	return;

	io_printf(out, "%s[%d]<%d>", info->name, info->refcount,
		info->list ? (int) info->list->used : -1);

	if	(info->setup)
		io_puts(" setup", out);

	if	(info->func)
		io_puts(" func", out);

	io_puts("\n", out);
}

static void dump_info(IO *out, InfoNode *info, int depth)
{
	size_t n;

	if	(!info)
		return;

	for (n = 0; n < depth; n++)
		io_putc('+', out);

	show_info(out, info);

	for (n = 0; n < vb_size(info->list); ++n)
		dump_info(out, *(InfoNode **) vb_data(info->list, n), depth + 1);
}

static const char *show(InfoNode *info)
{
	show_info(out(), info);
	return data();
}

static const char *dump(InfoNode *info)
{
	dump_info(out(), info, 0);
	return data();
}

TEST_DECL(InfoPath)
{
	char *path = mstrcpy(getenv("PATH")); /* Save PATH */
	TEST_EXPR(setenv("PATH", "a/bin:b", 1));
	SetInfoPath("x");
	EXPECT_STREQ(InfoPath, "x:a/lib/eis/%L:a/lib/efeu/%L/info");
	setenv("PATH", path, 1);	/* Restore PATH */
}

TEST_DECL(SetupInfo)
{
	root.setup = f_setup;
	EXPECT_STR(show(&root), "root[1]<-1> setup\n");
	SetupInfo(&root);
	EXPECT_TRUE(cnt_setup == 1);
	EXPECT_STR(dump(&root), "root[1]<-1>\n");

	EXPECT_STR(cstr(rd_ident(&root)), "root");
	TEST_EXPR(PrintInfo(out(), &root));
	EXPECT_STREQ(data(), "root node\n");

	InfoNode *i1 = AddInfo(&root, "x", "X-Label", NULL, "Desc of $1:$2\n");
	EXPECT_STR(dump(&root), "root[2]<1>\n+x[1]<-1>\n");

	InfoNode *i2 = AddInfo(i1, "y\\/z", "Y-Label", f_puts, "Y-Desc\n");
	EXPECT_STR(dump(&root), "root[2]<1>\n+x[2]<1>\n++y/z[1]<-1> func\n");

	PrintInfo(out(), &root);
	EXPECT_STR(data(), "root node\n\nx\tX-Label\n");
	PrintInfo(out(), i1);
	EXPECT_STR(data(), "X-Label\nDesc of x:X-Label\n\ny\\/z\tY-Label\n");
	PrintInfo(out(), i2);
	EXPECT_STR(data(), "Y-Label\nY-Desc\n");

#if	1
	rd_refer(i2);
	EXPECT_STR(dump(&root), "root[2]<1>\n+x[2]<1>\n++y/z[2]<-1> func\n");
	EXPECT_TRUE(rd_deref(i1) != NULL);
	EXPECT_TRUE(rd_deref(i1) == NULL);
	EXPECT_STR(show(i2), "y/z[1]<-1> func\n");
	EXPECT_TRUE(i2->prev == NULL);
#else
	EXPECT_TRUE(rd_deref(i2) == NULL);
	EXPECT_STR(dump(&root), "root[2]<1>\n+x[1]<0>\n");
	EXPECT_TRUE(rd_deref(i1) == NULL);
#endif
	EXPECT_STR(dump(&root), "root[1]<0>\n");
}

TEST_DECL(Default)
{
	InfoNode *root = rd_deref(GetInfo(NULL, NULL));
	EXPECT_TRUE(root != NULL);
	EXPECT_TRUE(root->refcount == 1);

	InfoNode *info;
	TEST_EXPR(info = AddInfo(root, "a/b", "Label",
		f_puts, "Description\n"));
	TEST_EXPR(PrintInfo(out(), info));
	EXPECT_STREQ(data(), "Label\nDescription\n");
	EXPECT_TRUE(root->refcount == 2);
	EXPECT_TRUE(info->refcount == 1);

	TEST_EXPR(InfoName(out(), NULL, info));
	EXPECT_STREQ(data(), "/a/b");
	EXPECT_STREQ(cstr(rd_ident(info)), "/a/b");

	TEST_EXPR(DumpInfo(out(), NULL, 0));
	EXPECT_STREQ(data(), "@:\n@a| --info=dump:/a\n");

	TEST_EXPR(DumpInfo(out(), NULL, 1));
	EXPECT_STREQ(data(), "@:\n@a:\n@a/b: Label\nDescription\n");

	TEST_EXPR(DumpInfo(out(), "/a", 1));
	EXPECT_STREQ(data(), "@:\n@b: Label\nDescription\n");

	TEST_EXPR(DumpInfo(out(), "/a/b", 1));
	EXPECT_STREQ(data(), "@: Label\nDescription\n");

	TEST_EXPR(info = GetInfo(NULL, "/a/./b/../"));
	TEST_EXPR(PrintInfo(out(), info));
	EXPECT_STREQ(data(), "\nb\tLabel\n");
	EXPECT_TRUE(rd_deref(info) != NULL);

	TEST_EXPR(info = AddInfo(NULL, "x/z\n\t\f\by-<|:>\\z",
		"L<$$x>|\n", NULL, NULL));
	TEST_EXPR(DumpInfo(out(), "/x", 1));
	EXPECT_STR(data(), "@:\n@z\\n\\t\\f\\by-\\<\\|\\:>\\\\\\\\z: "
		"L\\<$$$x>\\|\\\n\n");
	PRINT_STR(data());
}

TEST_DECL(LoadInfo)
{
	EXPECT_STR(dump(&root), "root[1]<0>\n");
	TEST_EXPR(LoadInfo(&root, "|printf '@0: Label\nDescription\n'"));
	PrintInfo(out(), rd_deref(GetInfo(&root, "0")));
	EXPECT_STR(data(), "Label\nDescription\n");

	TEST_EXPR(SetInfoPath(UnitTest_tmpname(NULL)));

	TEST_FILE("1.info", "@1\\t\\n\\f\\b\\ \\x \t\fy :"
		" \tz\\\n\\\t\\ \\<\\z \n");
	LoadInfo(&root, "1.info");
	InfoNode *info = *(InfoNode **) vb_data(root.list, 1);
	EXPECT_STREQ(info->name, "1\t\n\f\b \\xy");
	EXPECT_STREQ(info->label, "z\n\t <\\z");

	TEST_FILE("2.info", "@2 : File<2\\ dat\\a   \n");
	TEST_FILE("2 dat\\a", "Inhalt\n");
	LoadInfo(&root, "2.info");
	PrintInfo(out(), rd_deref(GetInfo(&root, "2")));
	EXPECT_STR(data(), "File\nInhalt\n");

	TEST_FILE("3.info", "@3\t:Pipe |echo Input\n");
	LoadInfo(&root, "3.info");
	PrintInfo(out(), rd_deref(GetInfo(&root, "3")));
	EXPECT_STR(data(), "Pipe\nInput\n");

//	PRINT_STR(data());
}

TEST_DECL(Coverage)
{
	TEST_EXPR(PrintInfo(iostd, NULL));
	EXPECT_FALSE(InfoNameToken(NULL));

	TEST_FORK
	{
		freopen("/dev/null", "w", stderr);
		EXPECT_FALSE(GetInfo(NULL, "-undefined-"));
		EXPECT_FALSE(AddInfo(NULL, "a", "Label", NULL, "Desc\n"));
		TEST_EXIT;
	}

	TEST_WAIT;
}

TEST_SUITE
{
	TEST_RUN(InfoPath);
	TEST_RUN(SetupInfo);
	TEST_RUN(Default);
	TEST_RUN(LoadInfo);
	TEST_RUN(Coverage);
}
