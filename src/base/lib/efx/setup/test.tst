#!/usr/bin/env .build/script.tst

EXPECT(dist(int, varint) == 0x1);
EXPECT(dist(_Ptr_, str) == 0x4);
EXPECT(dist(str, _Ptr_) == -1);

UnitTest::stdout("stdout");
showconv(int, NULL);
showconv(int, varint);
showconv(_Ptr_, str);
showconv(str, _Ptr_);
UnitTest::stdout();
EXPECT_CONTENT("stdout", string !
int -> varint: 0x001 PROMOTE
_Ptr_ -> str: 0x004 CONVERT
str -> _Ptr_: -1 REJECT
!);

EXPECT_REPR(getconv(int, varint), "virtual promotion varint int ()");

UnitTest::stdout("stdout");
objdump(42);
objdump(int);
objdump([x]);
UnitTest::stdout();
MATCH_CONTENT("stdout", string !
0x[[:xdigit:]]*: SmallObj 2 int = 42
0x[[:xdigit:]]*: SmallObj 2 Type_t = int
0x[[:xdigit:]]*: SmallObj 2 Expr_t = \[x]
!);

UnitTest::stdout("stdout");

struct Z { int y; } z;
void Z::func(int x)
{
	static int a;
	int b, c, d;
	vtabstack(2);
	fprintf(iostd, "---\n");
	vtabdump(0);
	vtabdump(3);
}

z.func(42);
UnitTest::stdout();
MATCH_CONTENT("stdout", string !
0x[[:xdigit:]]*\[3]. 0x[[:xdigit:]]* = \{
	a
}. func = \{
	this x
}. Z = \{
	func\() y
}\(Z). global\[[[:digit:]]*]
---
0x[[:xdigit:]]* = \{
	int b \[.*] = 0
	int c \[.*] = 0
	int d \[.*] = 0
}
\(Z) Z = \{
	int y
}
!);

UnitTest::stdout("stdout");
locale();
UnitTest::stdout();
EXPECT_CONTENT("stdout", string !
LC_SCAN="C"
LC_PRINT="C"
LC_DATE="de_AT"
!);

UnitTest::stdout("stdout");
lcshow();
UnitTest::stdout();
EXPECT_CONTENT("stdout", string !
scan.thousands_sep = NULL
scan.decimal_point = "."
print.thousands_sep = NULL
print.decimal_point = "."
print.negativ_sign = "-"
print.positive_sign = "+"
print.zero_sign = " "
!);

UnitTest::stdout("stdout");
parselist();
UnitTest::stdout();
MATCH_CONTENT("stdout", ".*EOF.*const.*while");

EXPECT(typehead(int) == NULL);
EXPECT(xdiv.type == double);

enum E {
	E1, // Wert 1
	E2 = 5, // Wert 2
};

UnitTest::stdout("stdout");
showenum(E);
UnitTest::stdout();
EXPECT_CONTENT("stdout", string !
0;E1;Wert 1
5;E2;Wert 2
!);
