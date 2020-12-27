#!/usr/bin/env .build/script.tst

/* Einfacher Vektor */

DataBase db = DataBase(int);
db += 5;
db += 2;
EXPECT_REPR(db, "DB: int[2]");
db.insert(1, 2);
EXPECT_REPR(db, "DB: int[4]");
EXPECT_STR((List_t) db, "{5, 0, 0, 2}");
EXPECT(dim(db) == 4);
db.sort(cmp);
EXPECT_STR((List_t) db, "{0, 0, 2, 5}");
EXPECT(db[2] == 2);

TEST(bad_index)
{
	ApplPath = UnitTest::tmp();
	TEST_FILE("efmain.msg", "35	bad index\n");
	UnitTest::stderr("stderr");
	db[5];
	EXPECT_CONTENT("stderr", "bad index\n");
}

{
	StrBuf s;
	db.save(s, false);
	EXPECT_STR((str) s, "#this\n0\n0\n2\n5\n");
}

db.delete(2, 1);
EXPECT_STR((List_t) db, "{0, 0, 5}");
db.clean();
EXPECT_REPR(db, "DB: int[0]");

db.load("#this\n2\n1\n7\n5\n");
EXPECT_STR((List_t) db, "{1, 2, 5, 7}");
db.fsave(UnitTest::tmp("db"), false);
EXPECT_CONTENT("db", "#this\n1\n2\n5\n7\n");
db.clean();
db.fload(UnitTest::tmp("db"));
EXPECT_STR((List_t) db, "{1, 2, 5, 7}");
EXPECT(db.index(9) == 4);
db = NULL;
db.sort(cmp);
EXPECT(db.index(9) == 0);

/* Komplexe Datenstruktur */

struct DATA { str name; int a; int v[1]; };
virtual int cmp(DATA a, DATA b) return cmp(a.name, b.name);

db = DataBase(DATA);

TEST_FILE("db", "#name;a;v\n\"x\";1;{12};999\n\"a\";2;{1}\n");
db.fload(UnitTest::tmp("db"));

db.fsave(UnitTest::tmp("out"), false);
EXPECT_CONTENT("out", "#name;a;v\n" + string !
"x";1;{12}
"a";2;{1}
!);

db.fsave(UnitTest::tmp("out"), true);
EXPECT_CONTENT("out", string !
name = "x"
a = 1
v = {12}

name = "a"
a = 2
v = {1}
!);

db.fsave(UnitTest::tmp("out"), false, NULL, "name -a +v");
EXPECT_CONTENT("out", "#name;v\n" + string !
"x";{12}
"a";{1}
!);

db.fsave(UnitTest::tmp("out"), false, NULL, "#1:2");
EXPECT_CONTENT("out", "#a;v\n" + string !
1;{12}
2;{1}
!);

db.fsave(UnitTest::tmp("out"), false, NULL, "#-2");
EXPECT_CONTENT("out", "#a\n" + string !
1
2
!);

db.fsave(UnitTest::tmp("out"), false, NULL, "+");
EXPECT_CONTENT("out", "#name;a;v\n" + string !
"x";1;{12}
"a";2;{1}
!);

db.sort(cmp);
db.fsave(UnitTest::tmp("out"), false);
EXPECT_CONTENT("out", "#name;a;v\n" + string !
"a";2;{1}
"x";1;{12}
!);

TEST_FILE("update", "\
name = \"x\"\n\
a = 5\n\
v = { 31 }\n\
\n\
/" + "* Kommentar */\n\
name = \"b\"\n\
% Anderes Kommentar
a = 8\n\
v = { 33 }\n\
");

db.fload(UnitTest::tmp("update"), [ y.a = x.a ]);
db.fsave(UnitTest::tmp("out"), false, NULL);
EXPECT_CONTENT("out", "#name;a;v\n" + string !
"a";2;{1}
"b";8;{33}
"x";5;{12}
!);

str DATA()
{
	return sprintf("%s:%d:%d", this.name, this.a, this.v[0]);
}

EXPECT_STR(db[0], "a:2:1");
DATA key = { "b", 11, {22} };
EXPECT(db.index(key) == 1);
EXPECT_STR(db[1], "b:8:33");
EXPECT_STR(db[key], "b:8:33");
EXPECT_STR(db.get(key), "b:8:33");
db += key;
EXPECT_STR(db[1], "b:11:22");
db -= key;
EXPECT_STR(db[1], "x:5:12");
key.name = "z";
EXPECT_STR(db[key], NULL);
db.clean();
