#include <EFEU/UnitTest.h>
#include <EFEU/preproc.h>
#include "utest/in.tst.h"
#include "utest/out.tst.h"

static const char *define(const char *def)
{
	PPMacro *macro = ParseMacro(IN(def));
	AddMacro(macro);
	ShowMacro(OUT(), macro);
	return utest_out();
}

static const char *subst(const char *def)
{
	io_macsub(IN(def), OUT(), NULL);
	return utest_out(); 
}

#define	MACDEF(def)	PRINT_STR(define(def))
#define	MACSUB(in,out)	EXPECT_STR(subst(in), out)

TEST_DECL(noarg)
{
	MACDEF("A1 [a\\\nb\\c]\nd");
	MACSUB("{A1}", "{[ab\\c]}");

	MACDEF("A2 [ a ]\\");
	MACSUB("{A2}", "{[ a ]\\}");
	MACSUB("{A2()}", "{[ a ]\\()}");
}

TEST_DECL(blank)
{
	MACDEF("B() [foo]");
	MACSUB("{B ( )}", "{[foo]}");
	MACSUB("{B(1)}", "{[foo]}");
}

TEST_DECL(single)
{
	MACDEF("C(x) [x]");
	MACSUB("{C()}", "{[]}");
	MACSUB("{C(1)}", "{[1]}");
	MACSUB("{C ( 1 )}", "{[1 ]}");
	MACSUB("{C(1, 2)}", "{[1]}");
	MACSUB("{C ( 1 , 2)}", "{[1 ]}");
	MACSUB("{C ((1, 2))}", "{[(1, 2)]}");
}

TEST_DECL(vaarg)
{
	MACDEF("D(...) [va_list]");
	MACSUB("{D()}", "{[]}");
	MACSUB("{D(1)}", "{[1]}");
	MACSUB("{D(1, 2)}", "{[1, 2]}");
}

TEST_DECL(mixed)
{
	MACDEF("E(x, ...) [x:va_list]");
	MACSUB("{E()}", "{[:]}");
	MACSUB("{E(1)}", "{[1:]}");
	MACSUB("{E(1, 2)}", "{[1:2]}");
	MACSUB("{E(1, 2, 3)}", "{[1:2, 3]}");
	MACSUB("{E((1, 2), 3, 4)}", "{[(1, 2):3, 4]}");

}

TEST_SUITE
{
	TEST_RUN(noarg);
	TEST_RUN(blank);
	TEST_RUN(single);
	TEST_RUN(vaarg);
	TEST_RUN(mixed);
	CLEANUP_IN;
	CLEANUP_OUT;
}
