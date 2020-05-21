#!/bin/sh
#if 0
export LANG=C
exec .build/script.tst $0 arg
#endif

EXPECT(argc == 2);
EXPECT_STR(argv[1], "arg");

str a;

pconfig !
:arg| :eval 'a = "{}"'
!

EXPECT(argc == 1);
PRINT_STR(a);

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

EXPECT_REPR(int operator+ (int, int);,
	"virtual int operator\"+\" (int, int)");

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

str operator@ = "special";
EXPECT_STR(operator@ , "special");
EXPECT_STR(operator"@", "special");
EXPECT_REPR([3*5], "[15]");
EXPECT_STR(parsub/-${6*7 'c'}-/, "-42 c-");
EXPECT_STR(parsub/-$(6*7:%3d)-/, "- 42-");
EXPECT_STR(parsub/-$(6*7)-/, "-42-");
EXPECT_STR(parsub/-$()-/, "--");
EXPECT_STR(string!abc!, "abc");

EXPECT_STR(typeof 3, "int");
EXPECT_STR(typeof(3), "int");
EXPECT_STR(typeof global, "VarTab");
EXPECT_STR(typeof local, "VarTab");
EXPECT_STR(typeof cinterp, "Efi");

List_t l1 = { 1, 2 };
EXPECT(l1.obj == 1);
EXPECT(l1.next.obj == 2);
VarTab v1 = VarTab("tab");
EXPECT(v1.refcount == 1);
EXPECT_STR(v1.ident, "tab[0]");

int n;
EXPECT_STR(parsub!$(&n)!, "((int *) n->data)");
EXPECT_STR(parsub!$(&undef_var)!, "undef_var->data");
struct { int a; int b; } s1;
EXPECT_STR(parsub!$(&s1.a)!, "((int *) ((char *) s1->data + 0))");
EXPECT_STR(parsub!$(&s1.b)!, "((int *) ((char *) s1->data + 4))");

UnitTest::stdin("stdin", "3 * 5\n");
UnitTest::stdout("stdout");
interactive
UnitTest::stdin();
UnitTest::stdout();
EXPECT_CONTENT("stdout", "15\n");


/* Code coverage */
const int x1 = 3;
static int x2 = 5;
provide int;

PRINT_LABEL("Info - Ausgabe")
UnitTest::stdout("stdout");
Info("print:/");
Info("print:/Var");
Info("print:/Func");
UnitTest::stdout();

