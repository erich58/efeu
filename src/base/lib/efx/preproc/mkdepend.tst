#!/usr/bin/env .build/script.tst

makedepend(NULL, OUT());
EXPECT_OUT("");
makedepend("target", OUT());
EXPECT_OUT("");

target("target_file_1");
target("target_file_2");
depend("depend_file_1");
depend("depend_file_2");
makedepend("target", OUT());
EXPECT_OUT("\ntarget: depend_file_1 depend_file_2\n");
EXPECT(dependlist() == { "depend_file_1", "depend_file_2" });
EXPECT(targetlist() == { "target_file_1", "target_file_2" });
