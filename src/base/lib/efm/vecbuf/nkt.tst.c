#include <EFEU/UnitTest.h>
#include <EFEU/nkt.h>

#include "utest/data.tst.h"

#define	IVAL(p)	((int *) (p))[0]

static void *iptr(int val)
{
	int *data = malloc(sizeof val);
	*data = val;
	return data;
}

static int clean_count = 0;

static void clean(void *data)
{
	free(data);
	++clean_count;	
}

static int visit (const char *name, void *data, void *par)
{
	io_printf(par, "%s=%d;", name, IVAL(data));
	return (IVAL(data) == 42) ? 1 : 0;
}

TEST_SUITE
{
	NameKeyTab *t = nkt_create("Test", sizeof(int), clean);
	int defval = 99;

	nkt_insert(t, "X", iptr(10));
	nkt_insert(t, "I", iptr(1));
	nkt_insert(t, "V", iptr(5));
	nkt_insert(t, "L", iptr(50));
	nkt_insert(t, "C", iptr(100));
	EXPECT_TRUE(nkt_walk(t, visit, out()) == 0);
	EXPECT_STR(data(), "C=100;I=1;L=50;V=5;X=10;");
	EXPECT_STR(rd_ident(t), "NameKeyTab Test[5]"); 

	clean_count = 0;
	nkt_delete(t, "L");
	nkt_delete(t, "C");
	EXPECT_TRUE(clean_count == 2);
	EXPECT_TRUE(nkt_rwalk(t, visit, out()) == 0);
	EXPECT_STR(data(), "X=10;V=5;I=1;");

	clean_count = 0;
	nkt_insert(t, "V", iptr(42));
	EXPECT_TRUE(clean_count == 1);
	EXPECT_TRUE(nkt_walk(t, visit, out()) == 1);
	EXPECT_STR(data(), "I=1;V=42;");

	EXPECT_TRUE(nkt_fetch(t, "Z", &defval) == &defval);
	EXPECT_TRUE(nkt_fetch(NULL, "Z", &defval) == &defval);
	EXPECT_TRUE(IVAL(nkt_fetch(t, "X", NULL)) == 10);

	clean_count = 0;
	rd_deref(t);
	EXPECT_TRUE(clean_count == 3);

	// Code Coverage
	rd_deref(nkt_create("X", 8, NULL));
	nkt_insert(NULL, "x", NULL);
	nkt_delete(NULL, "x");

}
