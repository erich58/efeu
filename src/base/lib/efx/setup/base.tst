#!/usr/bin/env .build/script.tst

pconfig !
ResourceName = Test
!

EXPECT_STR(ResourceName, "Test");

EXPECT_STR(typeof true, "bool");
EXPECT_IOTEST(true, "true");
EXPECT_IOTEST(false, "false");
EXPECT_STR(typeof 'a', "char");
EXPECT_IOTEST('a', "'a'");
EXPECT_STR(typeof L'a', "wchar_t");
EXPECT_IOTEST(L'€', "'€'");
EXPECT_IOTEST("abc", "\"abc\"");
EXPECT_IOTEST(str(NULL), "NULL");

{
	str s;
	EXPECT(read(ionull, s) == 0);
}

EXPECT_REPR(true, "true");
EXPECT_REPR(Type_t, "Type_t");
EXPECT_IOTEST(int, "int");
EXPECT_IOTEST(unknown_name, "(_undef_) unknown_name");
EXPECT_IOTEST(Object(5), "5");

EXPECT(!NULL);
EXPECT((bool)NULL == false);
UnitTest::stdout("stdout");
3
(void) 4
5
UnitTest::stdout();
MATCH_CONTENT("stdout", "3\n5\n");

{
	Object o = 5;
	EXPECT(o);
	EXPECT(NULL != o);
	EXPECT_REPR(o, "(Object) 5");
	free(o);
}

EXPECT_REPR((VirFunc) xdiv, "double xdiv (double, double)\n");
EXPECT_REPR((VirFunc) int::operator"++", "int & int::operator\"++\" & ()\n");
