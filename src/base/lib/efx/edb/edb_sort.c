/*	EDB-Datenbank sortieren

$Copyright (C) 2004 Erich Frühstück
This file is part of EFEU.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; see the file COPYING.Library.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <EFEU/EDB.h>
#include <EFEU/Debug.h>
#include <EFEU/ioctrl.h>
#include <EFEU/stdtype.h>
#include <EFEU/EDBFilter.h>

#define	FMT_UNIT "edb_sort: undefined unit key $2.\n"

#if	1
#define	EDB_SORT_MIN	0xfff0		/* 64 kB - 16 */
#define	EDB_SORT_STD	0x3fffff0	/* 64 MB - 16 */
#define	EDB_MAX_MERGE	128
#else
#define	EDB_SORT_MIN	16
#define	EDB_SORT_STD	64
#define	EDB_MAX_MERGE	3
#endif

#define	EDB_MIN_DIM	4	/* Für sehr große Datenstrukturen */

size_t edb_sort_space = 0;

#define	FTAB_BLK	1024	/* Buffergröße für Filetabelle */
#define	TMP_DIR		"."	/* Bibliothek für temporäre Dateien */
#define	TMP_SORT	"Sort"	/* Prefix für temporäre Dateien */
#define	TMP_MERGE	"Merge"	/* Prefix für temporäre Dateien */

#define	NAME	"edb_sort"	/* Debug - Name */

/*
Hilfsfunktion zur Testausgabe
*/

static int debug_depth = 0;

static void debug (const char *fmt, ...)
{
	static int sync = 0;
	static FILE *log = NULL;

	if	(sync < DebugChangeCount)
	{
		sync = DebugChangeCount;

		if	(log)
		{
			fprintf(log, "%s: STOP debugging\n", NAME);
			fileclose(log);
		}

		log = filerefer(LogFile(NAME, DBG_DEBUG));

		if	(log)
			fprintf(log, "%s: START debugging\n", NAME);
	}

	if	(log)
	{
		va_list list;

		va_start(list, fmt);
		fprintf(log, "%s: %*s", NAME, debug_depth, "");
		vfprintf(log, fmt, list);
		va_end(list);
		fputc('\n', log);
	}
}

/*
Tabelle mit temporären Dateinamen
*/

static void ftab_init (VecBuf *ftab)
{
	vb_init(ftab, FTAB_BLK, sizeof(char *));
	debug("ftab_init()\n");
}

static void ftab_free (VecBuf *ftab)
{
	vb_free(ftab);
	debug("ftab_free()\n");
}

static char *ftab_next (VecBuf *ftab)
{
	char **ptr = vb_next(ftab);
	*ptr = newtemp(TMP_DIR, TMP_SORT);
	debug(" %s", *ptr);
	return *ptr;
}

/*
*/

typedef struct {
	unsigned char *ptr;	/* Datenpointer im Buffer */
	unsigned size;		/* Datenlänge */
} POS;

typedef struct {
	unsigned char *data;	/* Datenpointer */
	POS *pos;	/* Positionsvektor */
	IO *io;		/* IO-Struktur für Datenbuffer */
	unsigned dpos;	/* Aktuelle Datenposition */
	unsigned dlim;	/* Limit für Datenspeicher */
	unsigned pidx;	/* Index im Positionsvektor */
	unsigned space;	/* Größe des Datenbuffers */
	unsigned mdim;	/* Maximaler Positionsindex */
	unsigned ksize;	/* Schlüssellänge */
	CmpDef *cmp;	/* Vergleichsfunktion */
	EfiObj *obj1;	/* Erstes Objekt für Vergleich */
	EfiObj *obj2;	/* Zweites Objekt für Vergleich */
} DBUF;

static int dbuf_get (void *ptr)
{
	DBUF *buf = ptr;
	return buf->data[buf->dpos++];
}

static int dbuf_put (int c, void *ptr)
{
	DBUF *buf = ptr;

	if	(buf->dpos < buf->dlim)
		buf->data[buf->dpos] = c;

	buf->dpos++;
	return c;
}

static unsigned get_ksize (CmpDef *cmp)
{
	CmpDefEntry *entry;
	unsigned ksize;

	if	(!cmp)	return 0;
		
	ksize = 0;

	for (entry = cmp->list; entry; entry = entry->next)
	{
		if	(entry->type->flags & TYPE_MALLOC)
			return 0;

		ksize += entry->dim * entry->type->size;
	}

	return ksize;
}

static void write_key (IO *out, CmpDef *cmp, char *data)
{
	CmpDefEntry *entry;

	for (entry = cmp->list; entry; entry = entry->next)
		io_write(out, data + entry->offset,
			entry->dim * entry->type->size);
}

static void dbuf_init (DBUF *buf, EfiType *type, CmpDef *cmp)
{
	size_t space;

	if	(edb_sort_space > EDB_SORT_MIN)	space = edb_sort_space;
	else if	(edb_sort_space)		space = EDB_SORT_MIN;
	else					space = EDB_SORT_STD;

	buf->ksize = get_ksize(cmp);

	if	(buf->ksize > space)
		buf->ksize = 0;

	buf->mdim = space / sizeof buf->pos[0];
	buf->space = buf->mdim * sizeof buf->pos[0];
	buf->pidx = buf->mdim;
	buf->dpos = 0;
	buf->dlim = buf->space;
	buf->data = lmalloc(buf->space);
	buf->pos = (void *) buf->data;
	buf->io = io_alloc();
	buf->io->get = dbuf_get;
	buf->io->put = dbuf_put;
	buf->io->ctrl = NULL;
	buf->io->data = buf;
	debug("mdim = %u, space = %u", buf->mdim, buf->space);
	buf->cmp = cmp;
	buf->obj1 = cmp ? NewObj(type, NULL) : NULL;
	buf->obj2 = cmp ? NewObj(type, NULL) : NULL;
}

static void dbuf_free (DBUF *buf)
{
	rd_deref(buf->io);
	lfree(buf->data);
	UnrefObj(buf->obj1);
	UnrefObj(buf->obj2);
}

static CmpDef *cdef = NULL;
static EfiObj *cmp1 = NULL;
static EfiObj *cmp2 = NULL;

static int key_cmp (const void *a_ptr, const void *b_ptr)
{
	CmpDefEntry *entry;
	char *a = (char *) ((POS *) a_ptr)->ptr;
	char *b = (char *) ((POS *) b_ptr)->ptr;

	for (entry = cdef->list; entry; entry = entry->next)
	{
		int r = entry->cmp(entry, a, b);

		if	(r)	return entry->invert ? -r : r;

		a += entry->dim * entry->type->size;
		b += entry->dim * entry->type->size;
	}

	return 0;
}

static int std_cmp (const void *a_ptr, const void *b_ptr)
{
	const POS *a = a_ptr;
	const POS *b = b_ptr;
	int r = memcmp(a->ptr, b->ptr, a->size < b->size ? a->size : b->size);

	if	(r)			return r;
	else if	(a->size < b->size)	return -1;
	else if	(a->size > b->size)	return 1;
	else				return 0;
}

static int ext_get (void *ptr)
{
	unsigned char **p = ptr;
	return *((*p)++);
}

static IO ext_io = STD_IODATA(ext_get, NULL, NULL, "<cmp_buf>");

static int ext_cmp (const void *a_ptr, const void *b_ptr)
{
	const POS *a = a_ptr;
	const POS *b = b_ptr;
	unsigned char *p;

	p = a->ptr;
	ext_io.data = &p;
	ReadData(cmp1->type, cmp1->data, &ext_io);
	p = b->ptr;
	ext_io.data = &p;
	ReadData(cmp2->type, cmp2->data, &ext_io);
	return cmp_data(cdef, cmp1->data, cmp2->data);
}

static int read_binary (EfiType *type, void *data, void *par)
{
	if	(io_peek(par) == EOF)
		return 0;

	return ReadData(type, data, par) != 0;
}

static size_t write_binary (EfiType *type, void *data, void *par)
{
	return WriteData(type, data, par);
}

static void preload (EDB *edb)
{
	char *name;

	name = newtemp(TMP_DIR, TMP_MERGE);
	debug("preload %s", name);

	edb->write = write_binary;
	edb->opar = io_fileopen(name, "wb");

	while (edb_read(edb))
		edb_write(edb);

	edb_closeout(edb);
	edb->read = read_binary;
	edb->ipar = io_fileopen(name, "rb");
	deltemp(name);
}

static EDB *tmp_merge (EfiType *type, CmpDef *cmp, char **list, size_t dim)
{
	EDB *tab[EDB_MAX_MERGE];
	EDB *edb;
	size_t n, k;

	if	(dim <= 1)
	{
		edb = edb_create(type);

		if	(dim)
		{
			edb->read = read_binary;
			edb->ipar = io_fileopen(*list, "rb");
			debug("open %s", *list);
			deltemp(*list);
		}

		rd_deref(cmp);
		return edb;
	}

	if	(dim > EDB_MAX_MERGE)
	{
		n = EDB_MAX_MERGE;
		k = (dim + n - 1) / n;
	}
	else
	{
		n = dim;
		k = 1;
	}

	debug("merge");
	debug_depth += 4;

	for (n = 0; dim > 0; n++)
	{
		if	(k > dim)	k = dim;

		tab[n] = tmp_merge (type, rd_refer(cmp), list, k);

		if	(k > 1)
			preload(tab[n]);

		dim -= k;
		list += k;
	}

	debug_depth -= 4;
	edb = edb_merge(cmp, tab, n);
	return edb;
}

static int dbuf_load (VecBuf *ftab, DBUF *buf, EDB *base)
{
	FILE *file;
	unsigned i, j, n;
	unsigned char *p;
	EfiObj *obj;
	unsigned start;

	obj = base->obj;
	buf->dpos = 0;
	buf->pidx = buf->mdim - 1;
	buf->dlim = buf->pidx * sizeof buf->pos[0];
	start = buf->dpos;

	while (buf->dpos < buf->dlim && edb_read(base))
	{
		if	(buf->ksize)
			write_key(buf->io, buf->cmp, obj->data);

		WriteData(obj->type, obj->data, buf->io);

		if	(buf->dpos > buf->dlim)
		{
			if	(start == 0)
			{
				IO *out = io_fileopen(ftab_next(ftab), "wb");
				WriteData(obj->type, obj->data, out);
				io_close(out);
				debug("space = %u, big data", buf->dpos);
				return 1;
			}

			edb_unread(base);
			break;
		}

		buf->pos[buf->pidx].ptr = buf->data + start;
		buf->pos[buf->pidx].size = buf->dpos - start;
		/*
		debug("%2d: %8u %8u", buf->pidx, start,
			buf->pos[buf->pidx].size);
		*/
		buf->dlim = --buf->pidx * sizeof buf->pos[0];
		start = buf->dpos;
	}

	buf->dpos = start;
	buf->pidx++;

	if	(buf->pidx == buf->mdim)
		return 0;

	n = buf->mdim - buf->pidx;

	if	(n <= 1)
	{
		;
	}
	else if	(buf->ksize)
	{
		cdef = buf->cmp;
		qsort(buf->pos + buf->pidx, n, sizeof buf->pos[0], key_cmp);
	}
	else if	(buf->cmp)
	{
		cdef = buf->cmp;
		cmp1 = buf->obj1;
		cmp2 = buf->obj2;
		qsort(buf->pos + buf->pidx, n, sizeof buf->pos[0], ext_cmp);
	}
	else	qsort(buf->pos + buf->pidx, n, sizeof buf->pos[0], std_cmp);

	file = fileopen(ftab_next(ftab), "wb");

	for (i = buf->pidx; i < buf->mdim; i++)
	{
		p = buf->pos[i].ptr + buf->ksize;
		j = buf->pos[i].size - buf->ksize;

		while (j-- > 0)
			putc(*p++, file);
	}

	fileclose(file);
	debug("space = %u, unused = %d, dim = %u",
		buf->dpos, (buf->pidx * sizeof buf->pos[0]) - buf->dpos,
		buf->mdim - buf->pidx);

	return n;
}

static EDB *sort_dbuf (EDB *edb, CmpDef *cmp)
{
	DBUF buf;
	VecBuf ftab;
	EfiType *type;

	type = edb->obj->type;
	ftab_init(&ftab);
	dbuf_init(&buf, type, cmp);

	while (dbuf_load(&ftab, &buf, edb))
		;

	dbuf_free(&buf);
	debug("fdim=%u\n", (unsigned) ftab.used);
	edb = tmp_merge(type, cmp, ftab.data, ftab.used);
	ftab_free(&ftab);
	return edb;
}


static int cmp_fix (const void *a_ptr, const void *b_ptr)
{
	return cmp_data(cdef, a_ptr, b_ptr);
}

static void fix_save (VecBuf *ftab, EfiType *type, CmpDef *cmp,
	char *p, size_t n)
{
	IO *out;

	if	(n)
	{
		cdef = cmp;
		qsort(p, n, type->size, cmp_fix);
	}

	out = io_fileopen(ftab_next(ftab), "wb");

	while (n-- > 0)
	{
		WriteData(type, p, out);
		p += type->size;
	}

	io_close(out);
}

static EDB *sort_fix (EDB *edb, CmpDef *cmp)
{
	size_t n, dim;
	void *buf;
	char *p;
	EfiType *type;
	VecBuf ftab;

	if	(!edb->read)	return edb;

	if	(edb_sort_space > EDB_SORT_MIN)	dim = edb_sort_space;
	else if	(edb_sort_space)		dim = EDB_SORT_MIN;
	else					dim = EDB_SORT_STD;

	type = edb->obj->type;
	dim /= type->size;

	if	(dim < EDB_MIN_DIM)	dim = EDB_MIN_DIM;

	buf = lmalloc(dim * type->size);
	debug("size = %u, dim = %u", (unsigned) type->size, (unsigned) dim);

	n = 0;
	p = buf;
	ftab_init(&ftab);

	while (edb_read(edb))
	{
		CopyData(type, p, edb->obj->data);

		if	(++n >= dim)
		{
			fix_save(&ftab, type, cmp, buf, n);
			n = 0;
			p = buf;
		}
		else	p += type->size;
	}

	if	(n)
		fix_save(&ftab, type, cmp, buf, n);

	edb_closein(edb);
	lfree(buf);
	debug("fdim=%u\n", (unsigned) ftab.used);
	edb = tmp_merge(type, cmp, ftab.data, ftab.used);
	ftab_free(&ftab);
	return edb;
}


EDB *edb_sort (EDB *base, CmpDef *cmp)
{
	EDB *edb;
	EfiType *type;

	type = base->obj->type;

	if	(!cmp || (type->flags & TYPE_MALLOC))
	{
		edb = sort_dbuf(base, cmp);
	}
	else	edb = sort_fix(base, cmp);

	edb->desc = mstrcpy(base->desc);
	rd_deref(base);
	return edb;
}

static EDB *fdef_sort (EDBFilter *filter, EDB *base,
	const char *opt, const char *arg)
{
	if	(opt)
	{
		char *p;
		double lim;
		
		lim = C_strtod(opt, &p);

		switch (*p)
		{
		case  0:	break;
		case 'k':	lim *= 1024.; break;
		case 'M':	lim *= 1024. * 1024.; break;
		case 'G':	lim *= 1024. * 1024. * 1024.; break;
		default:	dbg_note("edb", FMT_UNIT, "c", *p); break;
		}

		edb_sort_space = lim + 0.5;
	}

	return edb_sort(base, arg ?
		cmp_create(base->obj->type, arg, NULL) : NULL);
}

EDBFilter EDBFilter_sort = EDB_FILTER(NULL,
	"sort", "[size]=cmp", fdef_sort, NULL,
	":*:sort data"
	":de:Daten sortieren"
);
