#!/usr/bin/env .build/script.tst

AssignArg a;
EXPECT_REPR(a, "NULL");
a = "Name[Opt]=Arg";
EXPECT_REPR(a, "Name[Opt]=Arg");
EXPECT_STR(a.name, "Name");
EXPECT_STR(a.opt, "Opt");
EXPECT_STR(a.arg, "Arg");
EXPECT_REPR((List_t) a, "{\"Name\", \"Opt\", \"Arg\"}");
str s = "a=b,c";
a = AssignArg(s, ",");
EXPECT_REPR(a, "a=b");
EXPECT_STR(s, "c");
