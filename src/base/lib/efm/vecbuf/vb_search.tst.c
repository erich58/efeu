#include <EFEU/UnitTest.h>
#include <EFEU/vecbuf.h>
#include <EFEU/memalloc.h>

typedef struct {
	int key;
	int val;
} DATA;

#define	EXPECT_DATA(p, k, v)	EXPECT(isdata(p, k, v))

static void *set(void *ptr, int key, int val)
{
	if	(ptr)
	{
		DATA *data = ptr;
		data->key = key;
		data->val = val;
		return data;
	}

	return NULL;
}

static int isdata(const void *ptr, int key, int val)
{
	if	(ptr)
	{
		const DATA *data = ptr;
		return data->key == key && data->val == val;
	}

	return 0;
}

static int comp (const void *va, const void *vb)
{
	const DATA *a = va;
	const DATA *b = vb;

	if	(a->key < b->key)	return -1;
	if	(a->key > b->key)	return 1;

	return 0;
}


TEST_SUITE
{
	VecBuf *vb = vb_create(8, sizeof(DATA));
	DATA d;
	set(vb_next(vb), 7, 70);
	set(vb_next(vb), 4, 40);
	set(vb_next(vb), 8, 80);
	vb_qsort(vb, comp);

	EXPECT_DATA(vb_search(vb, set(&d, 5, 50), comp, VB_ENTER), 5, 50);
	EXPECT_DATA(&d, 0, 0);
	EXPECT_DATA(vb_search(vb, set(&d, 1, 10), comp, VB_ENTER), 1, 10);
	EXPECT_DATA(&d, 0, 0);
	EXPECT_DATA(vb_search(vb, set(&d, 1, 11), comp, VB_ENTER), 1, 10);
	EXPECT_DATA(&d, 1, 11);

	EXPECT(vb_search(vb, set(&d, 3, 30), comp, VB_REPLACE) == NULL);
	EXPECT_DATA(&d, 0, 0);
	EXPECT(vb_search(vb, set(&d, 3, 31), comp, VB_REPLACE) == &d);
	EXPECT_DATA(&d, 3, 30);

	EXPECT(vb_size(vb) == 6);
	EXPECT_DATA(vb_data(vb, 0), 1, 10);
	EXPECT_DATA(vb_data(vb, 1), 3, 31);
	EXPECT_DATA(vb_data(vb, 2), 4, 40);
	EXPECT_DATA(vb_data(vb, 3), 5, 50);
	EXPECT_DATA(vb_data(vb, 4), 7, 70);
	EXPECT_DATA(vb_data(vb, 5), 8, 80);

	EXPECT(vb_search(vb, set(&d, 2, 0), comp, VB_SEARCH) == NULL);
	EXPECT_DATA(vb_search(vb, set(&d, 3, 0), comp, VB_SEARCH), 3, 31);
	EXPECT_DATA(vb_search(vb, set(&d, 5, 0), comp, VB_SEARCH), 5, 50);

	EXPECT(vb_bsearch(vb, set(&d, 2, 0), comp) == NULL);
	EXPECT_DATA(vb_bsearch(vb, set(&d, 5, 0), comp), 5, 50);

	EXPECT_DATA(vb_search(vb, set(&d, 3, 0), comp, VB_DELETE), 3, 31);
	EXPECT(vb_search(vb, set(&d, 3, 0), comp, VB_DELETE) == NULL);

	EXPECT(vb_size(vb) == 5);
	vb_destroy(vb);
}
