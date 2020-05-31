#!/usr/bin/env .build/script.tst

{
	StrPool p = NULL;
	EXPECT_REPR(p, "NULL");
}

{
	StrPool p = StrPool();
	EXPECT_REPR(p, "StrPool: 8/8 + 0/0");
	EXPECT(p.add("data") == 8);
	EXPECT(p.printf("%d", 42) == 8 + 5);
	p.save(UnitTest::tmp("data"));
	p.cvar(OUT(), "name");
	EXPECT_OUT(string !

static char name_data[16] = "StrPool\0"
	"data\0"
	"42";

static StrPool name = {
	REFDATA(&StrPool_reftype), NULL,
	name_data, NULL,
	16, 16, 0, 0,
};

!);

	p = StrPool(UnitTest::tmp("data"));
	EXPECT_REPR(p, "StrPool: 8/16 + 0/0");
	EXPECT(p.start() == 0);
	EXPECT(p.next() == 8);
	EXPECT(p.next() == 8 + 5);
	EXPECT(p.next() == 0);
	EXPECT(p.add("text") == 16);

	p.cvar(OUT(), "zz");
	EXPECT_OUT(string !

static char zz_data[21] = "StrPool\0"
	"data\0"
	"42\0"
	"text";

static StrPool zz = {
	REFDATA(&StrPool_reftype), NULL,
	zz_data, NULL,
	21, 21, 0, 0,
};

!);

	EXPECT_STR(p[0ul], NULL);
	EXPECT_STR(p[8ul], "data");
	EXPECT_STR(p[13ul], "42");
	EXPECT_STR(p[0], NULL);
	EXPECT_STR(p[8], "data");
	EXPECT_STR(p[13], "42");
}
