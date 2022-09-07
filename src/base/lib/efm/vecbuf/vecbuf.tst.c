#include <EFEU/UnitTest.h>
#include <EFEU/vecbuf.h>
#include <EFEU/memalloc.h>

/* Tests erfolgen mit einem Integer-Vektor
*/

#include "utest/out.tst.h"

#define	IVAL(p)	((int *) p)[0]

static void vclean(void *ptr)
{
	;
}

static const char *vinfo(int *vec, size_t dim)
{
	IO *io = OUT();
	io_printf(io, "%zu:", dim);

	if	(vec && dim)
	{
		size_t i;

		for (i = 0; i < dim; i++)
			io_printf(io, " %d", vec[i]);
	}

	return utest_out();
}

static const char *vbinfo(VecBuf *buf)
{
	return vinfo(vb_data(buf, 0), vb_size(buf));
}

static int le_4 (const void *a)
{
	return (IVAL(a) <= 4);
}

static int le_2 (const void *a)
{
	return (IVAL(a) <= 2);
}

static int gt_3 (const void *a)
{
	return (IVAL(a) > 3);
}

static int vcomp (const void *a, const void *b)
{
	if	(IVAL(a) < IVAL(b))	return -1;
	if	(IVAL(a) > IVAL(b))	return 1;

	return 0;
}

typedef struct {
	int min;
	int max;
} MKEY;

static void *mkey(int min, int max)
{
	static MKEY key;
	key.min = min;
	key.max = max;
	return &key;
}

static int mcomp (const void *key, const void *d)
{
	const MKEY *k = key;

	if	(IVAL(d) > k->max)	return -1;
	if	(IVAL(d) < k->min)	return 1;

	return 0;
}

static const char *minfo(VecBuf *vb, int min, int max)
{
	int *v = vb_data(vb, 0);
	size_t n = vb_size(vb);
	VecMatch m = vmatch(mkey(min, max), v, n, sizeof v[0], mcomp);

	IO *io = OUT();
	io_printf(io, "%zu[%zu,%zu]:", n, m.start, m.end);

	if	(v && n)
	{
		size_t i = 0;

		while (i < m.start)
			io_printf(io, " %d", v[i++]);

		io_printf(io, " [");

		while (i < m.end)
			io_printf(io, " %d", v[i++]);

		io_printf(io, " ]");

		while (i < n)
			io_printf(io, " %d", v[i++]);
	}

	return utest_out();
}

TEST_DECL(basic)
{
	VecBuf vbuf;
	vb_init(&vbuf, 1, sizeof(int));
	vb_alloc(&vbuf, 2);
	IVAL(vb_next(&vbuf)) = 1;
	IVAL(vb_next(&vbuf)) = 5;
	IVAL(vb_insert(&vbuf, 1, 1)) = 10;
	EXPECT(vb_insert(&vbuf, 1, 0) == NULL);
	EXPECT(vb_insert(&vbuf, 4, 1) == NULL);
	*((int *) vb_next(&vbuf)) = 2;
	EXPECT(vb_size(&vbuf) == 4);

	int *v = vb_data(&vbuf, 0);
	EXPECT_STR(vbinfo(&vbuf), "4: 1 10 5 2");

	v = vb_delete(&vbuf, 1, 2);
	EXPECT(v[0] + v[1] == 15); // Reihenfolge nicht definiert
	EXPECT(vb_delete(&vbuf, 4, 1) == NULL);
	IVAL(vb_insert(&vbuf, 0, 1)) = 3;
	EXPECT(IVAL(vb_data(&vbuf, 2)) == 2);

	vb_clear(&vbuf, vclean);
	vb_free(&vbuf);
	EXPECT(1);
}

#define	D1	"a1a2a3a4"
#define	D2	"b1b2"

TEST_DECL(copy)
{
	VecBuf *buf = vb_create(8, 2);
	EXPECT(vb_copy(NULL, D1, 4) == NULL);
	EXPECT(vb_copy(buf, NULL, 0) == NULL);
	EXPECT(memcmp(vb_copy(buf, D1, 4), D1, 8) == 0);

	void *last = buf->data;
	EXPECT(vb_append(NULL, D2, 2) == NULL);
	EXPECT(vb_append(buf, NULL, 0) == last);
	EXPECT(memcmp(vb_append(buf, D2, 2), D1 D2, 12) == 0);

	EXPECT(vb_fetch(NULL, NULL) == NULL);
	void *d = vb_fetch(buf, NULL);
	EXPECT(d != NULL);
	EXPECT(memcmp(d, D1 D2, 12) == 0);
	vb_delete(buf, 0, 4);
	EXPECT(memcmp(vb_fetch(buf, d), D2, 4) == 0);
	memfree(d);
	vb_destroy(buf);
}

TEST_DECL(qsort)
{
	VECBUF(buf, 16, sizeof(int));
	int *v;
	int key;
	size_t n;

	IVAL(vb_next(&buf)) = 1;
	IVAL(vb_next(&buf)) = 5;
	IVAL(vb_next(&buf)) = 1;
	IVAL(vb_next(&buf)) = 3;
	IVAL(vb_next(&buf)) = 8;
	IVAL(vb_next(&buf)) = 4;
	IVAL(vb_next(&buf)) = 2;
	EXPECT_STR(vbinfo(&buf), "7: 1 5 1 3 8 4 2");

	v = vb_data(&buf, 0);
	n = vb_size(&buf);
	EXPECT(vsplit(v, n, sizeof v[0], NULL) == 7);
	EXPECT_STR(vinfo(v, n), "7: 1 5 1 3 8 4 2");
	EXPECT(vsplit(v, n, sizeof v[0], gt_3) == 3);
	MATCH_STR(vinfo(v, n), "7: [458] [458] [458] [123] [123] [123] [123]");
	EXPECT(vsplit(v, n, sizeof v[0], le_4) == 5);
	MATCH_STR(vinfo(v, n), "7: [1-4] [1-4] [1-4] [1-4] [1-4] [58] [58]");

	vb_qsort(&buf, vcomp);
	EXPECT_STR(vbinfo(&buf), "7: 1 1 2 3 4 5 8");
	vb_uniq(&buf, NULL);
	EXPECT_STR(vbinfo(&buf), "7: 1 1 2 3 4 5 8");
	vb_uniq(&buf, vcomp);
	EXPECT_STR(vbinfo(&buf), "6: 1 2 3 4 5 8");

	v = vb_data(&buf, 0);
	n = vb_size(&buf);
	EXPECT(svsplit(v, n, sizeof v[0], le_4) == 4);
	EXPECT(svsplit(v, n, sizeof v[0], le_2) == 2);
	EXPECT_STR(vinfo(v, n), "6: 1 2 3 4 5 8");

	key = 7;
	EXPECT(vb_bsearch(&buf, &key, vcomp) == NULL);
	EXPECT(bsearch(&key, v, n, sizeof v[0], vcomp) == NULL);
	key = 3;
	EXPECT(IVAL(vb_bsearch(&buf, &key, vcomp)) == 3);
	EXPECT(IVAL(bsearch(&key, v, n, sizeof v[0], vcomp)) == 3);

	VecMatch m = vmatch(mkey(0, 0), v, n, sizeof v[0], NULL);
	EXPECT(m.start == 0 && m.end == n);

	EXPECT_STR(minfo(&buf, 0, 0), "6[0,0]: [ ] 1 2 3 4 5 8");
	EXPECT_STR(minfo(&buf, 10, 10), "6[6,6]: 1 2 3 4 5 8 [ ]");
	EXPECT_STR(minfo(&buf, 3, 5), "6[2,5]: 1 2 [ 3 4 5 ] 8");
	EXPECT_STR(minfo(&buf, 6, 7), "6[5,5]: 1 2 3 4 5 [ ] 8");
	EXPECT_STR(minfo(&buf, 5, 3), "6[3,3]: 1 2 3 [ ] 4 5 8");
	EXPECT_STR(minfo(&buf, 0, 10), "6[0,6]: [ 1 2 3 4 5 8 ]");

	vb_free(&buf);
}

TEST_SUITE
{
	TEST_RUN(basic);
	TEST_RUN(copy);
	TEST_RUN(qsort);
	CLEANUP_OUT;
}
