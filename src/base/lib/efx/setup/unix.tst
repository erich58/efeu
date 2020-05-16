#!/usr/bin/env .build/script.tst

PRINT_LABEL("Datentyp: mode_t");

EXPECT(mode_t.base == unsigned);
EXPECT_REPR(mode_t(0), "?---------");
EXPECT_REPR(mode_t(0100111), "---x--x--x");
EXPECT_REPR(mode_t(0100222), "--w--w--w-");
EXPECT_REPR(mode_t(0100444), "-r--r--r--");

EXPECT_REPR(mode_t(0101000), "---------T");
EXPECT_REPR(mode_t(0102000), "------S---");
EXPECT_REPR(mode_t(0104000), "---S------");
EXPECT_REPR(mode_t(0101111), "---x--x--t");
EXPECT_REPR(mode_t(0102111), "---x--s--x");
EXPECT_REPR(mode_t(0104111), "---s--x--x");

EXPECT_REPR(mode_t(0010000), "p---------");
EXPECT_REPR(mode_t(0020000), "c---------");
EXPECT_REPR(mode_t(0040000), "d---------");
EXPECT_REPR(mode_t(0060000), "b---------");
EXPECT_REPR(mode_t(0100000), "----------");
EXPECT_REPR(mode_t(0120000), "l---------");
EXPECT_REPR(mode_t(0140000), "s---------");

PRINT_LABEL("Datentyp: FileStat");

EXPECT(FileStat.base == void);
Time t = Time();
umask(027);
TEST_FILE("data", "1234\n");
EXPECT(FileStat s = FileStat(UnitTest::tmp("data")));
EXPECT(s);
MATCH_STR(s.path, UnitTest::tmp("data"));
EXPECT(s.uid == getuid());
EXPECT(s.gid == getgid());
EXPECT_STR(s.mode, "-rw-r-----");
EXPECT((int) s.mode == 0100640);
EXPECT(s.type == 'f');
EXPECT(s.size == 5);
EXPECT(s.mtime >= t);
MATCH_REPR(s, "^.*/data: .* 0100640 .* 5)");

EXPECT(FileStat(".").type == 'd');
EXPECT(FileStat("/dev/null").type == 'c');
EXPECT(FileStat::mtime == 0);
EXPECT_REPR(FileStat("/bad/file/name"), "(unknown)");
EXPECT(!FileStat("/bad/file/name"));

system(UnitTest::tpl("ln -s @/data @/link"));
EXPECT(FileStat(UnitTest::tmp("link")).type == 'f');
EXPECT(FileStat(UnitTest::tmp("link"), true).type == 'l');
str dev = fgets(open("|find /dev -type b 2>/dev/null", "r"));
EXPECT(FileStat(dev).type == 'b');

PRINT_LABEL("Datentyp: passwd");

EXPECT(passwd pw = passwd("root"));
EXPECT_STR(pw.name, "root");
EXPECT_STR(pw.gecos, "root");
EXPECT_STR(pw.home, "/root");
MATCH_STR(pw.shell, "/bin/.*");
EXPECT(pw.uid == 0);
EXPECT(pw.gid == 0);
EXPECT_STR(passwd(0).name, "root");
EXPECT_REPR(passwd("bad user name"), "(unknown)");
EXPECT(!passwd("bad user name"));
EXPECT(passwd().uid == getuid());

PRINT_LABEL("Dateilisten, Verzeichnisse");

EXPECT(mkdir(UnitTest::tpl("@/sdir")) == true);
EXPECT(mkdir(UnitTest::tpl("@/sdir")) == false);
TEST_FILE("sdir/.hidden", "Text\n");

str tmp = UnitTest::tmp();
EXPECT(dim(flist(tmp)) == 3);
PRINT(flist(tmp, NULL, true));
EXPECT_STR(flist(tmp, "l*", true).obj, "link");
EXPECT_STR(flist(tmp, NULL, true, "d").obj, "sdir");
EXPECT(dim(flist(tmp, NULL, true, "fd")) == 4);

TEST_FORK
{
	EXPECT(chdir("/"));
	EXPECT_STR(getcwd(), "/");
	EXPECT(chdir("/bad/dir/name") == false);
	TEST_EXIT;
}

CHILD_EXIT(0);
