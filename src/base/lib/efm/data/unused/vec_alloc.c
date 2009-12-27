/*	Speicherverwaltung für Datenvektoren
	(c) 1998 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5

	Version 1.0
*/

#include <EFEU/data.h>
#include <EFEU/vecbuf.h>

#define	UNIT	1.	/* Einheit für die Freigabe */

#define	FMT_NEW "vec_alloc: %8.0f Byte angefordert.\n"
#define	FMT_USED "vec_alloc: %8.0f Byte neu vergeben.\n"
#define	FMT_FREE "vec_alloc: %8.0f Byte freigegeben.\n"

#define	ERR_FREE "vec_free: Pointer %p ist nicht in der Zuweisungstabelle.\n"

size_t vec_alloc_request = 0;
size_t vec_alloc_total = 0;
size_t vec_alloc_free = 0;
int vec_alloc_debug_flag = 0;

typedef struct {
	void *data;		/* Datenpointer */
	size_t size;		/* Speichergröße */
} vec_alloc_t;

#if	0
static int cmp_ptr (const vec_alloc_t *a, const vec_alloc_t *b)
{
	if	(a->data < b->data)	return -1;
	else if	(a->data > b->data)	return 1;

	return 0;
}

static VECBUF (blk_used, 512, sizeof(vec_alloc_t));
static VECBUF (blk_free, 512, sizeof(vec_alloc_t));

/*	test_size definiert, bis zu welcher Übergröße ein bereits
	definierter Datenblock verwendet werden kann.
*/

#define	test_size(s, b) 	(5 * (s) >= 4 * (b))

void *vec_alloc (size_t size, size_t dim)
{
	size *= dim;

	if	(size)
	{
		vec_alloc_t buf;
		register vec_alloc_t *tab, *ptr;
		register size_t i, n;

		tab = blk_free.data;
		ptr = NULL;
		n = blk_free.used;

		for (i = blk_free.used; i > 0; i--)
		{
			if	(tab[i].size < size)	continue;

			if	(ptr == NULL || tab[i].size < ptr->size)
			{
				ptr = tab + i;
				n = i;

				if	(ptr->size == size)	break;
			}
		}

		if	(ptr && test_size(size, ptr->size))
		{
			ptr = vb_delete(&blk_free, n, 1);
			vec_alloc_free -= ptr->size;

			if	(vec_alloc_debug_flag)
				fprintf(stderr, FMT_USED, ptr->size / UNIT);
		}
		else
		{
			vec_alloc_total += size;
			buf.size = size;
			buf.data = lmalloc(buf.size);
			ptr = &buf;

			if	(vec_alloc_debug_flag)
				fprintf(stderr, FMT_NEW, ptr->size / UNIT);
		}

		ptr = vb_search(&blk_used, ptr, (comp_t) cmp_ptr, VB_ENTER);
		vec_alloc_request++;
		return ptr->data;
	}
	else	return NULL;
}

void vec_free (void *entry, size_t size, size_t dim)
{
	if	(entry)
	{
		vec_alloc_t buf, *ptr;

		buf.data = entry;
		buf.size = size * dim;
		ptr = vb_search(&blk_used, &buf, (comp_t) cmp_ptr, VB_DELETE);

		if	(ptr)
		{
			vec_alloc_t *x = vb_next(&blk_free);
			x->data = ptr->data;
			x->size = ptr->size;
			vec_alloc_free += ptr->size;

			if	(vec_alloc_debug_flag)
				fprintf(stderr, FMT_FREE, ptr->size / UNIT);
		}
		else	fprintf(stderr, ERR_FREE, entry);
	}
}

#define	STAT_UNIT	1000000.

void vec_alloc_stat(void)
{
	fprintf(stderr, "vec_alloc():");
	fprintf(stderr, " %9.6f - %9.6f Mbyte",
		vec_alloc_total / STAT_UNIT, vec_alloc_free / STAT_UNIT);
	fprintf(stderr, " %6ld + %6ld blk",
		blk_used.used, blk_free.used);
	fprintf(stderr, " %8ld requests", vec_alloc_request);
	putc('\n', stderr);
}
#else
void *vec_alloc (size_t size, size_t dim)
{
	vec_alloc_total += size;
	vec_alloc_request++;
	return lmalloc(size * dim);
}

void vec_free (void *entry, size_t size, size_t dim)
{
	vec_alloc_total -= size;
	lfree(entry);
}

#define	STAT_UNIT	1000000.

void vec_alloc_stat(void)
{
	fprintf(stderr, "vec_alloc():");
	fprintf(stderr, " %9.6f Mbyte", vec_alloc_total / STAT_UNIT);
	fprintf(stderr, " %8ld requests", vec_alloc_request);
	putc('\n', stderr);
}
#endif
