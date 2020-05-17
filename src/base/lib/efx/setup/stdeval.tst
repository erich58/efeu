#!/bin/sh
#if 0
export LANG=C
exec .build/script.tst $0
#endif

EXPECT_STR(str.name, "str");
EXPECT_STR(str.cname, "char *");
EXPECT_STR(str.desc, NULL);
EXPECT(str.recl == 0);
EXPECT(int.default == 0);
EXPECT(str.base == _Ptr_);
EXPECT(str.flags == 4);
EXPECT(int.flags == 0);
EXPECT(int32_t.size == 4);
EXPECT_REPR(Func.var, "Func = {\n\ttype\n}");

UnitTest::stdout("stdout");
bool.info("print");
UnitTest::stdout();
EXPECT_CONTENT("stdout", "\
name = bool\n\
\n\
default = false\n\
\n\
cname = \"int\"\n\
size = 4\n\
recl = 4\n\
\n\
io:\tprint\n\
copy:\tmemcpy\n\
clean:\tmemset\n\
\n\
conv\tConverters\n\
create\tConstructors\n\
par\tParameters\n\
var\tVariables\n"
);
