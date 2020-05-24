#!/usr/bin/env .build/script.tst

EXPECT(isdigit('1'));
EXPECT(isalpha('a'));
EXPECT(isalnum('4'));
EXPECT(isspace('\t'));
EXPECT(sizeof(int) == 4);
EXPECT(sizeof(3) == 4);
EXPECT(sizeof(undef) == 0);
MATCH_REPR(whatis(4), "SmallObj 2 int");

struct X { int a; int b; } x;
EXPECT_STR(declare(X), "struct X {\n\tint a;\n\tint b;\n}"); 
EXPECT_STR(declare(X, true), "struct X { int a, b }"); 
EXPECT(offset(x, x.a) == 0);
EXPECT(offset(x, x.b) == 4);
EXPECT(align(6, 4) == 8);
EXPECT(align(5, 0) == 5);

int X::foo() { return 42; }
X X(void) return { 1, 2 };
EXPECT(call(X::foo, x) == 42);
EXPECT_REPR(call(X), "{1, 2}");

PRINT_LABEL("Ausgabefunktionen")

int a = 0;
IO in = "3 24";
in >> a;
EXPECT(a == 3);
in >> a;
EXPECT(a == 24);

StrBuf sb;
putc('*', sb);
puts("ABC", sb);
fprintf(sb, ": %d", 42);
EXPECT_STR(sb, "*ABC: 42");
sb.free();

UnitTest::stdout("stdout");
printf("Testausgabe %d\n", 42);
UnitTest::stdout();
EXPECT_CONTENT("stdout", "Testausgabe 42\n");

EXPECT_STR(sprintf("%d", 42), "42");
EXPECT(pcopy(IN("abc\nxyz\n"), OUT(), '\n') == 3);
EXPECT_OUT("abc");
EXPECT_STR(pcopy(IN("abc\nxyz\n"), 'y'), "abc\nx");

EXPECT(TeXputs(OUT(), "a_c") == 4);
EXPECT_OUT("a\\_c");
EXPECT_REPR(expr(3 * 5), "15");

PRINT_LABEL("Termauswertung");

int a = 10;
StrBuf sb;

{
	int a = 0;
	eval("a += 2");
	geval("a += 2");
	eval(IN("a++"));
	EXPECT(a == 3);
	sb.free();
	IO out = sb;
	cmdeval(IN("a+100"), out, false);
	putc(' ', out);
	cmdeval(IN("a+200"), out, true);
}

EXPECT(a == 12);
EXPECT_STR(sb, "103 212");

PRINT_LABEL("Skripts laden");

TEST_FILE("s1.hdr", "int s1 = 10;");

IncPath += UnitTest::tpl(":@");
MATCH_STR(fsearch(IncPath, "s1", "hdr"), ".*/s1.hdr");
include("s1.hdr");
EXPECT(s1 == 10);

TEST_FILE("s2.hdr", "a += 100;");
TEST_FILE("s3.hdr", "a += 200;");
int a = 10;

{
	int a = 0;
	EXPECT(load(IncPath, "s2", "hdr"));
	EXPECT(gload(IncPath, "s3", "hdr"));
	EXPECT(a == 100);
}

EXPECT(a == 210);

PRINT_LABEL("Mustervergleich");

{
	str r;
	EXPECT(patcmp("a*", "abc"));
	EXPECT(patcmp("a", "abc") == false);
	EXPECT(patcmp("a", "abc", r));
	EXPECT_STR(r, "bc");
	EXPECT(pselect("2", "a", "+[0-9]*", "-3"));
	EXPECT(pselect("3", "a", "+[0-9]*", "-3") == false);
	EXPECT(listcmp("0-9", '3'));
}

PRINT_LABEL("Strings und Listen");
EXPECT_STR(cat(":", "a", "b", "c"), "a:b:c");
EXPECT_REPR(split("a b c"), "{\"a\", \"b\", \"c\"}");
EXPECT(index("abc", 'b') == 1);
EXPECT(dim(typelist()) > 0);

PRINT_LABEL("Datentypen");
enum E { a, b, c };
EXPECT_REPR(enumlist(E), "{0, 1, 2}"); 
sb.free();
typetree(Type_t, sb);
PRINT_STR(sb, "Type_t\n `- Lval_t\n");
EXPECT(IsTypeClass(Lval_t, Type_t));

PRINT_LABEL("Systemumgebung (nur für Coverage)");
TEST_EXPR(setlocale(LC_DATE));
TEST_EXPR(setlang(LC_DATE));
TEST_EXPR(lcsave());
TEST_EXPR(printparam());
TEST_EXPR(lcrestore());
TEST_EXPR(pushcontext());
TEST_EXPR(popcontext());
