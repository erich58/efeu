/*	Große Datenmengen sortieren
	(c) 1997 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/bigsort.h>
#include <EFEU/procenv.h>

extern char *tempnam (const char *, const char *);

#define	MAX_MERGE	16
#define	DEF_SIZE	(1024 * 1024)
#define	DEF_DIM		(32 * 1024)
#define	FTAB_BSIZE	(32)

#define	TEMPNAM_PFX	"Sort"
#define	TEMPNAM_DIR	"."

#define	MODE_SET	0
#define	MODE_GET	1

int BigSortDebug = 0;

typedef struct {
	char *name;
	FILE *file;
	size_t count;
	size_t recl;
	char *fmt;
	size_t bufsize;
	size_t size;
	void *data;
} Merge_t;


/*	Daten sortieren
*/

static int comp_recl;
static comp_t comp_func;

static int std_comp(const void *a, const void *b)
{
	return memcmp(a, b, comp_recl);
}

static int ptr_comp(const BigSortPtr_t *a, const BigSortPtr_t *b)
{
	return comp_func(a->data, b->data);
}

static void bs_sort(BigSort_t *bs)
{
	if	(bs->idx <= 1)
	{
		;
	}
	else if	(bs->recl)
	{
		comp_recl = bs->recl;
		qsort(bs->data, bs->idx, bs->recl, bs->comp);
	}
	else
	{
		comp_func = bs->comp;
		qsort(bs->ptr, bs->idx, sizeof(BigSortPtr_t),
			(comp_t) ptr_comp);
	}
}


/*	Satzängen Ein/Ausgeben
*/

static size_t get_recl(FILE *file)
{
	unsigned recl;
	return (fread(&recl, sizeof(unsigned), 1, file) == 1) ? recl : 0;
}

static size_t put_recl(FILE *file, size_t recl)
{
	unsigned x = recl;
	return (fwrite(&x, sizeof(unsigned), 1, file) == 1) ? x : 0;
}


/*	Temporäre Mischdatei generieren
*/

static void merge_create(Merge_t *merge, size_t recl)
{
	merge->name = tempnam(TEMPNAM_DIR, TEMPNAM_PFX);
	memnotice(merge->name);
	merge->file = merge->name ? fopen(merge->name, "w") : NULL;
	merge->count = 0;
	merge->recl = recl;
	merge->fmt = NULL;
	merge->data = 0;
	merge->size = 0;

	if	(merge->file == NULL)
		message(merge->name, MSG_BIGSORT, 1, 0);

	switch (BigSortDebug)
	{
	case  0:
		merge->fmt = NULL;
		break;
	case  1:
		merge->fmt = ".";
		break;
	default:
		fprintf(stderr, "%s[", merge->name);
		merge->fmt = "%ld]\n"; 
		break;
	}
}


/*	Mischdateien öffnen/schließen
*/

static void merge_open(Merge_t *merge, size_t size, const char *delim)
{
	merge->file = merge->name ? fopen(merge->name, "r") : NULL;
	merge->count = 0;
	merge->fmt = NULL;

	if	(merge->file == NULL)
		message(merge->name, MSG_BIGSORT, 2, 0);

	merge->bufsize = size;
	merge->size = 0;
	merge->data = memalloc(merge->bufsize);

	switch (BigSortDebug)
	{
	case  0:	break;
	case  1:	break;
	default:	fprintf(stderr, "%s%s", merge->name, delim); break;
	}
}


static void merge_close(Merge_t *merge)
{
	if	(merge->file)
	{
		fclose(merge->file);
		merge->file = NULL;
		memfree(merge->data);
		merge->data = NULL;
		merge->bufsize = 0;

		if	(merge->fmt)
			fprintf(stderr, merge->fmt, merge->count);
	}
}


/*	Temporäre Mischdatei löschen
*/

static void merge_delete(Merge_t *merge)
{
	merge_close(merge);

	if	(merge->name)
	{
		remove(merge->name);
		memfree(merge->name);
		merge->name = NULL;
	}
}


/*	Datensatz aus Mischdatei laden
*/

static void *merge_read (Merge_t *merge)
{
	if	(merge->file == NULL)
		return NULL;

	merge->size = merge->recl ? merge->recl : get_recl(merge->file);

	if	(merge->bufsize < merge->size)
	{
		merge->bufsize = merge->size;
		memfree(merge->data);
		merge->data = memalloc(merge->bufsize);
	}

	if	(merge->size && fread(merge->data, merge->size, 1, merge->file) == 1)
		return merge->data;

	merge_delete(merge);
	return NULL;
}


/*	Datensätze in Mischdatei schreiben
*/

static void merge_write(Merge_t *merge, char *data, size_t size)
{
	if	(merge->file == NULL)	return;

	if	(merge->recl == 0 && put_recl(merge->file, size) != size)
	{
		message(merge->name, MSG_BIGSORT, 3, 0);
		merge_close(merge);
	}

	if	(fwrite(data, size, 1, merge->file) != 1)
	{
		message(merge->name, MSG_BIGSORT, 4, 0);
		merge_close(merge);
	}

	merge->count += size;
}


/*	Daten zwischenspeichern
*/

static void bs_save(BigSort_t *bs)
{
	Merge_t *merge;

	merge = vb_next(&bs->ftab);
	merge_create(merge, bs->recl);
	bs_sort(bs);

	if	(bs->recl == 0)
	{
		size_t i;

		for (i = 0; i < bs->idx; i++)
			merge_write(merge, bs->ptr[i].data, bs->ptr[i].size);
	}
	else	merge_write(merge, bs->data, bs->recl * bs->idx);

	merge_close(merge);
	bs->idx = 0;
	bs->pos = 0;
}

static void bs_load(BigSort_t *bs, size_t pos)
{
	Merge_t *merge = bs->ftab.data;

	if	(pos < bs->ftab.used && merge_read(merge + pos) == NULL)
	{
		merge = vb_delete(&bs->ftab, pos, 1);
		merge_delete(merge);
	}
}


static char *bs_merge(BigSort_t *bs, size_t *size)
{
	int i;
	char *ptr;
	Merge_t *merge;
	
	if	(bs->ftab.used == 0)
		return NULL;

	comp_func = bs->comp;
	comp_recl = bs->recl;
	merge = bs->ftab.data;
	ptr = merge[0].data;
	bs->idx = 0;

	for (i = 1; i < bs->ftab.used; i++)
	{
		if	(bs->comp(merge[i].data, ptr) < 0)
		{
			bs->idx = i;
			ptr = merge[bs->idx].data;
		}
	}

	*size = merge[bs->idx].size;
	return ptr;
}


/*	Sortierbuffer generieren
*/

BigSort_t *BigSort (size_t recl, size_t dim, size_t size, comp_t comp)
{
	BigSort_t *bs;

	bs = memalloc(sizeof(BigSort_t));
	bs->recl = recl;

	if	(dim)		bs->dim = dim;
	else if	(recl && size)	bs->dim = size / recl;
	else if	(size)		bs->dim = size / sizeof(BigSortPtr_t);
	else			bs->dim = DEF_DIM;

	if	(recl)	bs->size = bs->dim * recl;
	else if	(size)	bs->size = size;
	else		bs->size = DEF_SIZE;

	bs->idx = 0;
	bs->pos = 0;
	bs->data = memalloc(bs->size);
	bs->comp = comp ? comp : std_comp;
	vb_init(&bs->ftab, FTAB_BSIZE, sizeof(Merge_t));
	bs->ptr = recl ? NULL : memalloc(bs->dim * sizeof(BigSortPtr_t));

	if	(BigSortDebug > 1)
	{
		fprintf(stderr, "BigSort: %d %d %d\n", (int) bs->recl,
			(int) bs->dim, (int) bs->size);
	}

	return bs;
}

void *BigSortNext (BigSort_t *bs, size_t recl)
{
	void *ptr;

	if	(bs->idx >= bs->dim || bs->pos + recl > bs->size)
	{
		bs_save(bs);

		if	(recl > bs->size)
		{
			memfree(bs->data);
			bs->size = recl;
			bs->data = memalloc(bs->size);
		}
	}

	ptr = bs->data + bs->pos;

	if	(bs->recl)
	{
		bs->pos += bs->recl;
		bs->idx++;
	}
	else if	(recl)
	{
		bs->ptr[bs->idx].size = recl;
		bs->ptr[bs->idx].data = ptr;
		bs->pos += recl;
		bs->idx++;
	}

	return ptr;
}


static void bs_premerge(BigSort_t *bs, Merge_t *aus, Merge_t *ptr, size_t dim) 
{
	int i, j, n;

	for (i = j = 0; i < dim; i++)
	{
		merge_open(ptr + i, bs->size, " ");

		if	(merge_read(ptr + i))
			ptr[j++] = ptr[i];
	}

	dim = j;

	while (dim > 0)
	{
		n = 0;

		for (i = 1; i < dim; i++)
			if	(bs->comp(ptr[i].data, ptr[n].data) < 0)
				n = i;

		merge_write(aus, ptr[n].data, ptr[n].size);

		if	(!merge_read(ptr + n))
			for (dim--; n < dim; n++)
				ptr[n] = ptr[n + 1];
	}
}

#if	0
static void bs_merge1(Merge_t *aus, Merge_t *m1)
{
	while (m1->data)
	{
		merge_write(aus, m1->data, m1->size);
		merge_read(m1);
	}
}

static int bs_merge2(comp_t comp, Merge_t *aus, Merge_t *m1, Merge_t *m2)
{
	while (m1->data && m2->data)
	{
		if	(comp(m1->data, m2->data) > 0)
		{
			merge_write(aus, m2->data, m2->size);

			if	(!merge_read(m2))
				return bs_merge1(aus, m1);
		}
		else
		{
			merge_write(aus, m1->data, m1->size);

			if	(!merge_read(m1))
				return bs_merge1(aus, m2);
		}
	}

	bs_merge1(aus, m1);
	bs_merge1(aus, m2);
}
#endif

void BigSortReorg (BigSort_t *bs)
{
	int i;
	Merge_t *merge;

	if	(bs->ftab.used == 0)
	{
		if	(BigSortDebug > 1)
			fprintf(stderr, "sort ");

		bs_sort(bs);

		if	(BigSortDebug)
			fprintf(stderr, "[");

		bs->dim = bs->idx;
		bs->idx = 0;
		return;
	}

	bs_save(bs);
	bs->size = 0;
	memfree(bs->data);

	while (bs->ftab.used > MAX_MERGE)
	{
		int step, n, k;
		Merge_t merge;
		Merge_t *ptr;

		step = (bs->ftab.used + MAX_MERGE - 1) / MAX_MERGE;

		while (step > MAX_MERGE)
			step = (step + MAX_MERGE - 1) / MAX_MERGE;

		ptr = bs->ftab.data;

		if	(BigSortDebug)
			fprintf(stderr, "{");

		n = k = 0;

		while (k < bs->ftab.used)
		{
			if	(k + 1 == bs->ftab.used)
			{
				ptr[n++] = ptr[k];
				break;
			}

			if	(k + step >= bs->ftab.used)
				step = bs->ftab.used - k;

			merge_create(&merge, bs->recl);
			bs_premerge(bs, &merge, ptr + k, step);
			merge_close(&merge);
			ptr[n++] = merge;
			k += step;
		}

		bs->ftab.used = n;

		if	(BigSortDebug)
			fprintf(stderr, "}");
	}
	
	if	(BigSortDebug)
		fprintf(stderr, "[");

	merge = bs->ftab.data;

	for (i = 0; i < bs->ftab.used; i++)
	{
		merge_open(merge + i, bs->size, "\n\t");
		bs_load(bs, i);
	}

	bs->idx = bs->ftab.used;
}


void *BigSortData (BigSort_t *bs, size_t *sptr)
{
	void *ptr;
	size_t size;

	if	(bs->ftab.used)
	{
		bs_load(bs, bs->idx);
		ptr = bs_merge(bs, &size);
	}
	else if	(bs->idx < bs->dim)
	{
		if	(bs->recl)
		{
			size = bs->recl;
			ptr = bs->data + bs->idx * bs->recl;
		}
		else
		{
			size = bs->ptr[bs->idx].size;
			ptr = bs->ptr[bs->idx].data;
		}

		bs->idx++;
	}
	else
	{
		size = 0;
		ptr = NULL;
	}

	if	(sptr)	*sptr = size;

	return ptr;
}


void BigSortClose (BigSort_t *bs)
{
	memfree(bs->data);
	memfree(bs->ptr);
	memfree(bs);

	switch (BigSortDebug)
	{
	case  0:	break;
	case  1:	fputs("]\n", stderr); break;
	default:	fputs("done]\n", stderr); break;
	}
}
