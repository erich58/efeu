#!/usr/bin/env .build/script.tst

TEST_FILE("info", string !
@name: label
description
with extra line
@name/1: subentry | echo Input
!);

LoadInfo("test", UnitTest::tmp("info"));
InfoNode node = "/test/name";
EXPECT_REPR(node, "InfoNode: /test/name");
EXPECT_REPR(node.prev, "InfoNode: /test");
EXPECT_STR(node.name, "name");
EXPECT_STR(node.label, "label");
EXPECT_STR(node.desc, "description\nwith extra line\n");
AddInfo("/test/name/2", "second", "second entry\n");
EXPECT(node.dim == 2);
EXPECT_STR(node[0].name, "1");
EXPECT_STR(node[0].desc, "Input\n");
EXPECT_STR(node[1].name, "2");

UnitTest::stdin("/dev/null");
UnitTest::stdout("stdout");
ConsoleInfoBrowser("/test");
UnitTest::stdin();
UnitTest::stdout();
MATCH_CONTENT("stdout", ".");
UnitTest::stdout("stdout");
Info("xdump:/test");
UnitTest::stdout();
EXPECT_CONTENT("stdout", string !
@:
@name: label
description
with extra line
@name/1: subentry
Input
@name/2: second
second entry
!);

EXPECT_STR(GetInfo("/test/name"), string !
label
description
with extra line

1	subentry
2	second
!);
