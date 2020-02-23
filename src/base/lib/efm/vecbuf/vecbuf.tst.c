#include <EFEU/UnitTest.h>
#include <EFEU/vecbuf.h>
#include <EFEU/memalloc.h>

#define	IVAL(p)	((int *) p)[0]

static void vclean(void *ptr)
{
	;
}

static int vcomp (const void *a, const void *b)
{
	if	(IVAL(a) < IVAL(b))	return -1;
	if	(IVAL(a) > IVAL(b))	return 1;

	return 0;
}


TEST_DECL(basic)
{
	VecBuf vbuf;
	vb_init(&vbuf, 1, sizeof(int));
	vb_alloc(&vbuf, 2);
	IVAL(vb_next(&vbuf)) = 1;
	IVAL(vb_next(&vbuf)) = 5;
	IVAL(vb_insert(&vbuf, 1, 1)) = 10;
	EXPECT_TRUE(vb_insert(&vbuf, 1, 0) == NULL);
	EXPECT_TRUE(vb_insert(&vbuf, 4, 1) == NULL);
	*((int *) vb_next(&vbuf)) = 2;
	EXPECT_TRUE(vb_size(&vbuf) == 4);

	int *v = vb_data(&vbuf, 0);
	EXPECT_TRUE(v[0] == 1);
	EXPECT_TRUE(v[1] == 10);
	EXPECT_TRUE(v[2] == 5);
	EXPECT_TRUE(v[3] == 2);
	v = vb_delete(&vbuf, 1, 2);
	EXPECT_TRUE(v[0] + v[1] == 15); // Reihenfolge nicht definiert
	EXPECT_TRUE(vb_delete(&vbuf, 4, 1) == NULL);
	IVAL(vb_insert(&vbuf, 0, 1)) = 3;
	EXPECT_TRUE(IVAL(vb_data(&vbuf, 2)) == 2);

	vb_clean(&vbuf, vclean);
	vb_free(&vbuf);
	EXPECT_TRUE(1);
}

#define	D1	"a1a2a3a4"
#define	D2	"b1b2"

TEST_DECL(copy)
{
	VecBuf *buf = vb_create(8, 2);
	EXPECT_TRUE(vb_copy(NULL, D1, 4) == NULL);
	EXPECT_TRUE(vb_copy(buf, NULL, 0) == NULL);
	EXPECT_TRUE(memcmp(vb_copy(buf, D1, 4), D1, 8) == 0);

	void *last = buf->data;
	EXPECT_TRUE(vb_append(NULL, D2, 2) == NULL);
	EXPECT_TRUE(vb_append(buf, NULL, 0) == last);
	EXPECT_TRUE(memcmp(vb_append(buf, D2, 2), D1 D2, 12) == 0);

	EXPECT_TRUE(vb_fetch(NULL, NULL) == NULL);
	void *data = vb_fetch(buf, NULL);
	EXPECT_TRUE(data != NULL);
	EXPECT_TRUE(memcmp(data, D1 D2, 12) == 0);
	vb_delete(buf, 0, 4);
	EXPECT_TRUE(memcmp(vb_fetch(buf, data), D2, 4) == 0);
	memfree(data);
	vb_destroy(buf);
}

TEST_DECL(qsort)
{
	VECBUF(buf, 16, sizeof(int));
	int *v;
	int key;

	IVAL(vb_next(&buf)) = 1;
	IVAL(vb_next(&buf)) = 5;
	IVAL(vb_next(&buf)) = 1;
	IVAL(vb_next(&buf)) = 3;
	IVAL(vb_next(&buf)) = 2;

	vb_qsort(&buf, vcomp);
	v = vb_data(&buf, 0);
	EXPECT_TRUE(v[0] == 1);
	EXPECT_TRUE(v[1] == 1);
	EXPECT_TRUE(v[2] == 2);
	EXPECT_TRUE(v[3] == 3);
	EXPECT_TRUE(v[4] == 5);

	vb_uniq(&buf, vcomp);
	v = vb_data(&buf, 0);
	EXPECT_TRUE(v[0] == 1);
	EXPECT_TRUE(v[1] == 2);
	EXPECT_TRUE(v[2] == 3);
	EXPECT_TRUE(v[3] == 5);

	key = 7;
	EXPECT_TRUE(vb_bsearch(&buf, &key, vcomp) == NULL);
	key = 3;
	EXPECT_TRUE(IVAL(vb_bsearch(&buf, &key, vcomp)) == 3);

	vb_free(&buf);
}

TEST_SUITE
{
	TEST_RUN(basic);
	TEST_RUN(copy);
	TEST_RUN(qsort);
}
