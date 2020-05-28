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

str boolean_info = string !
name = bool

default = false

cname = "int"
size = 4
recl = 4

io:	print
copy:	memcpy
clean:	memset

conv	Converters
create	Constructors
par	Parameters
var	Variables
!;

UnitTest::stdout("stdout");
bool.info("print");
UnitTest::stdout();
EXPECT_CONTENT("stdout", boolean_info);

UnitTest::stdout("stdout");
Info("print:/Type/bool");
UnitTest::stdout();
EXPECT_CONTENT("stdout", boolean_info);

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

struct TestType {
	int a;
	str b;
};

EXPECT(is_type("TestType"));
EXPECT(is_type("UndefinedType") == false);

set_ctrl("A", "X");
EXPECT_STR(TestType.get_ctrl("A"), "X");
set_ctrl("A", "Y");
EXPECT_STR(TestType.get_ctrl("A"), "Y");
TestType.set_ctrl("C", "D1");
EXPECT_STR(TestType.get_ctrl("C"), "D1");
TestType.set_ctrl("C", "D2");
EXPECT_STR(TestType.get_ctrl("C"), "D2");
TestType.list_ctrl(OUT());
EXPECT_OUT("C\t\nA\t\n");

PRINT_LABEL("Info - Ausgabe")
UnitTest::stdout("stdout");
Info("print:/");
Info("print:/Var");
Info("print:/Func");
Info("print:/Type/int");
Info("print:/Type/str");
Info("print:/Type/varint");
Info("print:/Type/int/par");
Info("print:/Type/int/var");
Info("print:/Type/int/conv");
Info("print:/Type/int/create");
Info("print:/Type/TestType/member");
Info("xdump:/Type/TestType/par");
UnitTest::stdout();
EXPECT(int.order > 0);
EXPECT(int.dim == 0);
EXPECT_STR(int.src, NULL);
EXPECT(typeof int.create == VirFunc);
EXPECT(typeof int.convlist == List_t);
EXPECT(typeof int.convlist.obj == Func);

