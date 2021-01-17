#!/usr/bin/env .build/script.tst

StrBuf buf;

TEST(null)
{
	int x = 42;
	EDB edb = EDB(x);
	edb.out(buf, "null");
	edb.write();
	edb = NULL;
	EXPECT_STR(buf, "");
	buf.free();
}

TEST(binary)
{
	int x = 0x01020304;
	EDB edb = EDB(x);
	edb.out(buf, "binary,nohead");
	edb.write();
	edb = NULL;
	EXPECT_STR(buf, "EDB\t1.0\nint\n@data binary\n\001\002\003\004");
	buf.free();
}

TEST(plain_obj)
{
	int x = 42;
	EDB edb = EDB(x);
	edb.out(buf, "plain");
	edb.write();
	edb = NULL;
	EXPECT_STR(buf, "EDB\t1.0\nint\n@data plain\n42\n");
	buf.free();
}

TEST(plain_struct)
{
	struct { int a; int b; } x = { 3, 5 };
	EDB edb = EDB(x);
	edb.out(buf, "plain,nohead");
	edb.write();
	edb = NULL;
	EXPECT_STR(buf, "#a;b\n3;5\n");
	buf.free();
}

TEST(plain_vec)
{
	int[2] x = { 1, 2 };
	EDB edb = EDB(x);
	edb.out(buf, "plain,nohead");
	edb.write();
	edb = NULL;
	EXPECT_STR(buf, "{1, 2}\n");
	buf.free();
}

TEST(plain_enum)
{
	enum X { A, B, C } x = 2;
	EDB edb = EDB(x);
	edb.out(buf, "plain,nohead");
	edb.write();
	edb = NULL;
	EXPECT_STR(buf, "2\n");
	buf.free();
}

TEST(label_obj)
{
	int x = 42;
	EDB edb = EDB(x);
	edb.out(buf, "label");
	edb.write();
	edb = NULL;
	EXPECT_STR(buf, "EDB\t1.0\nint\n@data label\n42\n");
	buf.free();
}

TEST(label_struct)
{
	struct { int a; int b; } x = { 3, 5 };
	EDB edb = EDB(x);
	edb.out(buf, "label,nohead");
	edb.write();
	edb = NULL;
	EXPECT_STR(buf, "#a;b\n3;5\n");
	buf.free();
}

TEST(label_vec)
{
	int[2] x = { 1, 2 };
	EDB edb = EDB(x);
	edb.out(buf, "label,nohead");
	edb.write();
	edb = NULL;
	EXPECT_STR(buf, "{1, 2}\n");
	buf.free();
}

TEST(label_enum)
{
	enum X { A, B , C } x = 2;
	EDB edb = EDB(x);
	edb.out(buf, "label,nohead");
	edb.write();
	edb = NULL;
	EXPECT_STR(buf, "\"C\"\n");
	buf.free();
}

TEST(fmt)
{
	struct { int a; int b; } x = { 3, 5 };
	EDB edb = EDB(x, "DESC");
	edb.out(buf, "fmt[Header: $1]=<${b}:${a}:${a*b}>");
	edb.write();
	edb = NULL;
	EXPECT_STR(buf, "Header: DESC\n<5:3:15>\n");
	buf.free();
}

TEST(data)
{
	int x = 42;
	EDB edb = EDB(x);
	edb.out(buf, "data");
	edb.write();
	edb = NULL;
	EXPECT_STR(buf, "42\n");
	buf.free();
}

TEST(prec)
{
	double x = 1.23456;
	EDB edb = EDB(x);
	edb.out(buf, "data,prec[6]=3");
	edb.write();
	edb = NULL;
	EXPECT_STR(buf, "  1.23\n");
	buf.free();
}

TEST(data_struct)
{
	struct { int a; struct { int b; int c; } s; int d[2]} x = {
		3, { 5, 7 }, { 11, 12 }
	};
	EDB edb = EDB(x);
	edb.out(buf, "data[crlf]");
	edb.write();
	edb = NULL;
	EXPECT_STR(buf, "a;s.b;s.c;d[2]\r\n3;5;7;{11, 12}\r\n");
	buf.free();
}

TEST(data_info)
{
	int x = 42;
	EDB edb = EDB(x);

	UnitTest::stderr("stderr");
	TEST_FORK
	{
		edb.out(ionull, "data=");
		exit(1); // notreached
	}
	CHILD_EXIT(0);
	UnitTest::stderr(NULL);
	EXPECT_CONTENT("stderr", "binary\n");

	UnitTest::stderr("stderr");
	TEST_FORK
	{
		edb.out(ionull, "data=bad_output_name");
		exit(0); // notreached
	}
	CHILD_EXIT(1);
	UnitTest::stderr(NULL);
	MATCH_CONTENT("stderr", "bad_output_name");

	UnitTest::stderr("stderr");
	TEST_FORK
	{
		edb.out(ionull, "data={binary=?}");
		exit(1); // notreached
	}
	CHILD_EXIT(0);
	UnitTest::stderr(NULL);
	EXPECT_CONTENT("stderr", "binary\n\tbinary output\n");
}

TEST(data_binary)
{
	int x = 0x01020304;
	EDB edb = EDB(x);
	edb.out(buf, "data=binary");
	edb.write();
	edb = NULL;
	EXPECT_STR(buf, "\001\002\003\004");
	buf.free();
}

TEST(info)
{
	int x = 42;
	EDB edb = EDB(x);
	UnitTest::stderr("stderr");
	TEST_FORK
	{
		edb.out(buf, "?");
		exit(1); // notreached
	}
	CHILD_EXIT(0);
	UnitTest::stderr(NULL);
	MATCH_CONTENT("stderr", ".");
}

