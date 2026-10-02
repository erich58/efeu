#include <EFEU/UnitTest.h>
#include <EFEU/Info.h>
#include <EFEU/mstring.h>
#include <EFEU/procenv.h>

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

#include <EFEU/utest/out.tst.h>
#include <EFEU/utest/mstr.tst.h>

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
	show_info(OUT(), info);
	return utest_out();
}

static const char *dump(InfoNode *info)
{
	dump_info(OUT(), info, 0);
	return utest_out();
}

static IO *fake_interact(const char *prompt, const char *hist)
{
	return NULL;
}

TEST_DECL(InfoPath)
{
	/* Forciere die Farbinitialisierung (benötigt tput!) vor der
	 * Pfadänderung
	 */
	UnitTest_color(0);
	char *path = mstrcpy(getenv("PATH")); /* Save PATH */
	TEST_EXPR(setenv("PATH", "a/bin:b", 1));
	SetInfoPath("x");
	EXPECT_STR(InfoPath, "x:a/lib/eis/%L:a/lib/efeu/%L/info");
	setenv("PATH", path, 1);	/* Restore PATH */
}

TEST_DECL(SetupInfo)
{
	root.setup = f_setup;
	EXPECT_STR(show(&root), "root[1]<-1> setup\n");
	SetupInfo(&root);
	EXPECT(cnt_setup == 1);
	EXPECT_STR(dump(&root), "root[1]<-1>\n");

	EXPECT_MSTR(rd_ident(&root), "root");
	TEST_EXPR(PrintInfo(OUT(), &root));
	EXPECT_OUT("root node\n");

	InfoNode *i1 = AddInfo(&root, "x", "X-Label", NULL, "Desc of $1:$2\n");
	EXPECT_STR(dump(&root), "root[2]<1>\n+x[1]<-1>\n");

	InfoNode *i2 = AddInfo(i1, "y\\/z", "Y-Label", f_puts, "Y-Desc\n");
	EXPECT_STR(dump(&root), "root[2]<1>\n+x[2]<1>\n++y/z[1]<-1> func\n");

	PrintInfo(OUT(), &root);
	EXPECT_OUT("root node\n\nx\tX-Label\n");
	PrintInfo(OUT(), i1);
	EXPECT_OUT("X-Label\nDesc of x:X-Label\n\ny\\/z\tY-Label\n");
	PrintInfo(OUT(), i2);
	EXPECT_OUT("Y-Label\nY-Desc\n");

	rd_refer(i2);
	EXPECT_STR(dump(&root), "root[2]<1>\n+x[2]<1>\n++y/z[2]<-1> func\n");
	EXPECT(rd_deref(i1) != NULL);
	EXPECT(rd_deref(i1) == NULL);
	EXPECT_STR(show(i2), "y/z[1]<-1> func\n");
	EXPECT(i2->prev == NULL);
	EXPECT_STR(dump(&root), "root[1]<0>\n");
}

TEST_DECL(Default)
{
	InfoNode *root = rd_deref(GetInfo(NULL, NULL));
	EXPECT(root != NULL);
	EXPECT(root->refcount == 1);

	InfoNode *info;
	TEST_EXPR(info = AddInfo(root, "a/b", "Label",
		f_puts, "Description\n"));
	TEST_EXPR(PrintInfo(OUT(), info));
	EXPECT_OUT("Label\nDescription\n");
	EXPECT(root->refcount == 2);
	EXPECT(info->refcount == 1);

	TEST_EXPR(InfoName(OUT(), NULL, info));
	EXPECT_OUT("/a/b");
	EXPECT_MSTR(rd_ident(info), "/a/b");

	TEST_EXPR(DumpInfo(OUT(), NULL, NULL, 0));
	EXPECT_OUT("@:\n@a| --info=dump:/a\n");

	TEST_EXPR(DumpInfo(OUT(), NULL, NULL, 1));
	EXPECT_OUT("@:\n@a:\n@a/b: Label\nDescription\n");

	TEST_EXPR(DumpInfo(OUT(), NULL, "/a", 1));
	EXPECT_OUT("@:\n@b: Label\nDescription\n");

	TEST_EXPR(DumpInfo(OUT(), NULL, "/a/b", 1));
	EXPECT_OUT("@: Label\nDescription\n");

	IO *save = iostd;
	iostd = OUT();
	BrowseInfo("xdump:");
	EXPECT_OUT("@:\n@a:\n@a/b: Label\nDescription\n");
	iostd = OUT();
	BrowseInfo("dump:/a/b");
	EXPECT_OUT("@: Label\nDescription\n");
	iostd = OUT();
	BrowseInfo("print:/a/b");
	EXPECT_OUT("Label\nDescription\n");
	iostd = OUT();
	BrowseInfo("?");
	MATCH_OUT(
		"auto\n\t[^\n]*\n"
		"print\n\t[^\n]*\n"
		"dump\n\t[^\n]*\n"
		"xdump\n\t[^\n]*\n"
		"std\n\t[^\n]*\n"
	);
	iostd = save;

	Pager = mstrcpy(UnitTest_tmp("std.out"));
	_interact_open = fake_interact;
	BrowseInfo("std:/a/b");
	EXPECT_CONTENT("std.out", "Label\nDescription\n");

	TEST_EXPR(info = GetInfo(NULL, "/a/./b/../"));
	TEST_EXPR(PrintInfo(OUT(), info));
	EXPECT_OUT("\nb\tLabel\n");
	EXPECT(rd_deref(info) != NULL);

	TEST_EXPR(info = AddInfo(NULL, "x/z\n\t\f\by-<|:>\\z",
		"L<$$x>|\n", NULL, NULL));
	TEST_EXPR(DumpInfo(OUT(), NULL, "/x", 1));
	EXPECT_OUT("@:\n@z\\n\\t\\f\\by-\\<\\|\\:>\\\\\\\\z: "
		"L\\<$$$x>\\|\\\n\n");
	PRINT_OUT();
}

TEST_DECL(LoadInfo)
{
	EXPECT_STR(dump(&root), "root[1]<0>\n");
	TEST_EXPR(LoadInfo(&root, "|printf '@0: Label\nDescription\n'"));
	PrintInfo(OUT(), rd_deref(GetInfo(&root, "0")));
	EXPECT_OUT("Label\nDescription\n");

	TEST_EXPR(SetInfoPath(UnitTest_tmp(NULL)));

	TEST_FILE("1.info", "@1\\t\\n\\f\\b\\ \\x \t\fy :"
		" \tz\\\n\\\t\\ \\<\\z \n");
	LoadInfo(&root, "1.info");
	InfoNode *info = *(InfoNode **) vb_data(root.list, 1);
	EXPECT_STR(info->name, "1\t\n\f\b \\xy");
	EXPECT_STR(info->label, "z\n\t <\\z");

	TEST_FILE("2.info", "@2 : File<2\\ dat\\a   \n");
	TEST_FILE("2 dat\\a", "Inhalt\n");
	LoadInfo(&root, "2.info");
	PrintInfo(OUT(), rd_deref(GetInfo(&root, "2")));
	EXPECT_OUT("File\nInhalt\n");

	TEST_FILE("3.info", "@3\t:Pipe |echo Input\n");
	LoadInfo(&root, "3.info");
	PrintInfo(OUT(), rd_deref(GetInfo(&root, "3")));
	EXPECT_OUT("Pipe\nInput\n");
}

TEST_DECL(Coverage)
{
	TEST_EXPR(PrintInfo(iostd, NULL));
	EXPECT(!InfoNameToken(NULL));

	TEST_FORK
	{
		freopen("/dev/null", "w", stderr);
		EXPECT(!GetInfo(NULL, "-undefined-"));
		EXPECT(!AddInfo(NULL, "a", "Label", NULL, "Desc\n"));
		TEST_EXIT;
	}

	CHILD_EXIT(0);
}

TEST_SUITE
{
	TEST_RUN(InfoPath);
	TEST_RUN(SetupInfo);
	TEST_RUN(Default);
	TEST_RUN(LoadInfo);
	TEST_RUN(Coverage);
	CLEANUP_MSTR;
	CLEANUP_OUT;
}
