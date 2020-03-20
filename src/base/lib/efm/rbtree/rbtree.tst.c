#include <EFEU/UnitTest.h>
#include <EFEU/RBTree.h>
#include <EFEU/strbuf.h>

static void *key (int val)
{
	static int vbuf;
	vbuf = val;
	return &vbuf;
}

static int key_cmp (const void *key, const void *data, size_t size)
{
	return *((const int *) key) - *((const int *) data);
}

static int clean_count = 0;

static void key_clean (void *data)
{
	++clean_count;
}

RBT_DECL(rbt,sizeof(int),key_cmp, NULL, key_clean);

static void add(int k)
{
	rbt_search(&rbt, &k);
}

static int find(int k)
{
	int *val = rbt_find(&rbt, &k);

	if	(!val)		return 0;
	if	(*val == k)	return 1;

	PRINT_INT(*val);
	return 0;
}

static int cnt = 0;

static void cvisit(const void *entry, const VISIT which, const int depth)
{
	switch (which)
	{
	case preorder:
	case endorder:
		break;
	case leaf:
	case postorder:
		cnt++;
		break;
	}
}

static void *pvisit(const void *entry, void *ptr)
{
	++*((int *)ptr);
	return NULL;
}

static int count()
{
	int count = 0;
	rbt_pwalk(&rbt, pvisit, &count);
	return count;
}

static void *porder(const void *entry, void *ptr)
{
	int val = *((const int *) entry);
	int *last = ptr;

	if	(*last >= val)
		return ptr;

	*last = val;
	return NULL;
}

static int check_order()
{
	int last = -1;
	return rbt_pwalk(&rbt, porder, &last) == NULL;
}

TEST_SUITE
{
	int i;

	EXPECT_FALSE(find(1));
	add(1);
	EXPECT(find(1));
	EXPECT(count() == 1);
	add(1);
	add(1);
	EXPECT(count() == 1);

	EXPECT_FALSE(rbt_delete(&rbt, key(2)));
	EXPECT(rbt_delete(&rbt, key(1)));
	EXPECT(clean_count == 0);
	EXPECT(count() == 1);
	EXPECT(rbt_delete(&rbt, key(1)));
	EXPECT(rbt_delete(&rbt, key(1)));
	EXPECT(count() == 0);
	EXPECT(clean_count == 1);

	for (i = 0; i < 20; i++)
	{
		add(i);
		add(20 + i);
		add(60 - i);
	}

	EXPECT(check_order());
	EXPECT(count() == 60);
	cnt = 0;
	rbt_walk(&rbt, cvisit);
	EXPECT(cnt == 60);

	clean_count = 0;

	for (i = 20; i < 40; i++)
		rbt_delete(&rbt, &i);

	EXPECT(clean_count == 20);
	EXPECT(count() == 40);
	rbt_destroy(&rbt);
	EXPECT(count() == 0);
}
