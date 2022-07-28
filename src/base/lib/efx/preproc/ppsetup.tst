#!/usr/bin/env .build/script.tst

/* Comment */
EXPECT_STR(lastcomment(), "Comment ");
/* Comment */
clearcomment();
EXPECT_STR(lastcomment(), NULL);

copy(preproc(IN("#define a x\na\n")), OUT());
EXPECT_OUT("x\n");

IncPath=UnitTest::tmp();
PRINT_STR(IncPath);
TEST_FILE("input", "#define b z\nb\n");
copy(ppopen("input"), OUT());
EXPECT_OUT("z\n");
