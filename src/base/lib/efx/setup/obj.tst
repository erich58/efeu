#!/usr/bin/env .build/script.tst

{
	Expr_t x;
	EXPECT_STR(x, "[void]");
	EXPECT(int a = 7);
	x = [ 6 * a ];
	MATCH_STR(x, "_Expr_");
	EXPECT(eval(x) == 42);
	x = "return 6 * a;";
	MATCH_STR(x, "_Block_");
	EXPECT(eval(x) == 42);

	Expr_t c = classification(a, "switch=7 a7;* ax");
	Expr_t cx = classification(x, "switch=42 x42;* xx");
	EXPECT_STR(eval(c), "a7");
	EXPECT_STR(eval(cx), "x42");
	a = 1;
	EXPECT_STR(eval(c), "ax");
	EXPECT_STR(eval(cx), "xx");
}

{
	VirFunc f = "sqrt";
	EXPECT_DBL(f(4.), 2.);
}

{
	VarTab v = VarTab("test");
	int v::a = 5;
	int a = 3;
	Expr_t x = [ a * a ];
	EXPECT(eval(x) == 9);
	EXPECT(eval(x, v) == 25);
}

EXPECT(dim({ 1, 2, 3 }) == 3);
EXPECT(dim(2.5) == 1);
EXPECT(dim("abc") == 1);
EXPECT(dim(5) == 1);

{
	int n;
	char c = 'a';
	EXPECT((c += 1) == 'b');

	polymorph int cmp(Object a, Object b)
		return cmp(a.data(), b.data());
	Object & Object::operator"=" & (Object x) { this = x; }

	Object a = 5;
	Object b = 7;
	EXPECT(a < b);
	EXPECT(a <= b);
	EXPECT(a != b);
	LogConfig("func.trace");
	EXPECT(b == b);
	LogConfig("@");
	EXPECT(b >= a);
	EXPECT(b > a);
	EXPECT(!a == false);
	a := b;
	EXPECT(a.data() == b.data());
	a.update();	// for coverage
}

{
}
struct X { int a; };
polymorph X operator+ (X x, int i) { return { x.a + i}; }
polymorph X operator- (X x, int i) { return { x.a - i}; }
X & X::operator+= & (int i) { a += i; }
X & X::operator-= & (int i) { a -= i; }

{
	bool X() return this.a;

	X x = { 5 };
	X y = { 7 };

	EXPECT(x < y);
	EXPECT(x <= y);
	EXPECT(x != y);
	EXPECT(y == y);
	EXPECT(y >= x);
	EXPECT(y > x);
	EXPECT(cmp(x, y) < 0);
	EXPECT(!x == false);
	x++;
	PRINT(x.a)
	EXPECT(x.a == 6);
	++x;
	EXPECT(x.a == 7);
	x--;
	EXPECT(x.a == 6);
	--x;
	EXPECT(x.a == 5);
}

{
	List_t l1 = { 1, 3, };
	int x = 10;
	List_t l2 = { x, 2.5, 5 };
	EXPECT(l1 + l2 == { 11, 5.5 });
	EXPECT(catlist(l1, l2) == { 1, 3, 10, 2.5, 5 });
	int a = 5;
	int b = 2;
	List_t l = { a, b };
	l += l2;
	EXPECT(l == { 15, 4 });
}

{
	int x = 1;
	List_t l;
	l.push(x, "a");
	l.push(2);
	x = 20;
	EXPECT(l == { 2, "a", 1 });
	l.dump(open(UnitTest::tmp("dump"), "w"));
	MATCH_CONTENT("dump", ".* int\n.* str\n.*int\n");
	l.top(10, x);
	x = 0;
	EXPECT(l == { 2, "a", 1, 10, 20 });
	EXPECT(l.pop() == 2);
	EXPECT(l.pop() == "a");
	EXPECT(l == { 1, 10, 20 });
}

{
	int x = 1;
	List_t l = { x };
	l.data()++;
	EXPECT(x == 2);
	l.update();	// for coverage
}

{
	List_t l = { 7, 1, 2.5 };
	l.qsort(cmp);
	EXPECT(l == { 1, 2.5, 7 });
	EXPECT(l.bsearch(7, cmp) == 7);
	EXPECT(l.bsearch(2, cmp) == NULL);
	EXPECT(l.bsearch(2.5, cmp) == 2.5);
	EXPECT(l.bsearch(2.51, cmp) == NULL);
}

{
	List_t l = { 2, 1, 3, 3, 1, 1, 2, 5 };
	l.uniq(cmp);
	EXPECT(l == { 2, 1, 3, 1, 2, 5 });
	PRINT(l);
}
