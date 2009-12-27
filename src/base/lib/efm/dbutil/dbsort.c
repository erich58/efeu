/*	Große Dateien sortieren
	(c) 1995 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/vecbuf.h>
#include <EFEU/dbutil.h>
#include <EFEU/procenv.h>

extern char *tempnam (const char *, const char *);

char *dbsort_tmpdir = NULL;
char *dbsort_prefix = "SF";
int dbsort_debug = 0;

typedef int (*put_t) (const void *data, DBFILE *file);


#define	NFILES	8	/* Maximale Zahl der offenen Teilfiles */


/*	Aufräumen
*/

static void cleanup(char **name, size_t dim)
{
	int i;

	for (i = 0; i < dim; i++)
	{
		if	(name[i] != NULL)
		{
			fprintf(stderr, "rm %s\n", name[i]);
			remove(name[i]);
			memfree(name[i]);
		}
	}
}


/*	Temporären Filenamen generieren
*/

static char *getname(void)
{
	char *p;

	p = tempnam(dbsort_tmpdir, dbsort_prefix);
	memnotice(p);
	return p;
}


/*	Vergleichsfunktionen
*/

static DBFILE *do_cmp(comp_t cmp, DBFILE *a, DBFILE *b)
{
	if	(a == NULL || a->buf == NULL)
		return (b && b->buf) ? b : NULL;

	if	(b == NULL || b->buf == NULL)
		return a;

	return (cmp(a->buf, b->buf) > 0 ? b : a);
}


#define	CFUNC(name, cmd)	\
static DBFILE *name(comp_t cmp, DBFILE **x) { return cmd; }

CFUNC(cmp1, (x[0] && x[0]->buf) ? x[0] : NULL)
CFUNC(cmp2, do_cmp(cmp, x[0], x[1]))
CFUNC(cmp3, do_cmp(cmp, cmp2(cmp, x), x[2]))
CFUNC(cmp4, do_cmp(cmp, cmp2(cmp, x), cmp2(cmp, x + 2)))
CFUNC(cmp5, do_cmp(cmp, cmp3(cmp, x), cmp2(cmp, x + 3)))
CFUNC(cmp6, do_cmp(cmp, cmp3(cmp, x), cmp3(cmp, x + 3)))
CFUNC(cmp7, do_cmp(cmp, cmp4(cmp, x), cmp3(cmp, x + 4)))
CFUNC(cmp8, do_cmp(cmp, cmp4(cmp, x), cmp4(cmp, x + 4)))


static void merge(comp_t comp, char **name, size_t dim, DBFILE *out, put_t put)
{
	int i;
	DBFILE **file;
	DBFILE *x;
	DBFILE *(*cfunc)(comp_t comp, DBFILE **x);

	switch (dim)
	{
	case  0:	return;
	case  1:	cfunc = cmp1; break;
	case  2:	cfunc = cmp2; break;
	case  3:	cfunc = cmp3; break;
	case  4:	cfunc = cmp4; break;
	case  5:	cfunc = cmp5; break;
	case  6:	cfunc = cmp6; break;
	case  7:	cfunc = cmp7; break;
	case  8:	cfunc = cmp8; break;
	default:	message("dbsort", MSG_DB, 33, 0); return;
	}

/*	Debug - Information ausgeben
*/
	if	(dbsort_debug)
	{
		fputs("merge", stderr);

		for (i = 0; i < dim; i++)
		{
			putc(' ', stderr);
			fputs(name[i], stderr);
		}

		fputs(" -> ", stderr);
		fputs(fileident(out->file), stderr);
		putc('\n', stderr);
	}

/*	Dateien öffnen
*/
	file = memalloc(dim * sizeof(DBFILE *));

	for (i = 0; i < dim; i++)
	{
		file[i] = db_open(name[i], "r", out->reclen);
		db_get(file[i]);
	}

/*	Mischen
*/
	while ((x = cfunc(comp, file)) != NULL)
	{
		put(x->buf, out);
		db_get(x);
	}

/*	Aufräumen
*/
	for (i = 0; i < dim; i++)
	{
		db_close(file[i]);
		remove(name[i]);
		memfree(name[i]);
		name[i] = NULL;
	}
}


static int xmerge(comp_t comp, char **name, size_t dim, size_t reclen)
{
	DBFILE *file;
	char *p;

	if	(dim < 2)	return 0;

	p = getname();

	if	(p == NULL)	return 31;

	file = db_open(p, "w", reclen);
	merge(comp, name, dim, file, db_put);
	db_close(file);

	name[0] = p;
	return 0;
}


/*	Datei in Blöcken lesen und Zwischenspeichern
*/

static size_t db_presort (DBFILE *in, size_t blksize, comp_t comp, char ***ptr)
{
	FILE *out;
	vecbuf_t namebuf;
	static void *buf;
	char *p;
	ulong_t n, k;

	if	(blksize == 0)	blksize = 1024;

	vb_init(&namebuf, 16, sizeof(char *));
	buf = lmalloc(in->reclen * blksize);

	while ((n = db_read(buf, blksize, in)) != 0)
	{
		if	((p = getname()) == NULL)
		{
			cleanup(namebuf.data, namebuf.used);
			message("dbsort", MSG_DB, 31, 0);
			procexit(EXIT_FAILURE);
		}

		memcpy(vb_next(&namebuf), &p, sizeof(char *));

		if	(dbsort_debug)
			fprintf(stderr, "Block %d: ", (int) namebuf.used);

		qsort(buf, n, in->reclen, comp);
		out = fileopen(p, "w");
		k = fwrite(buf, in->reclen, n, out);
		fileclose(out);

		if	(k != n)
		{
			char b1[24], b2[24];
			cleanup(namebuf.data, namebuf.used);
			sprintf(b1, "%lu", n);
			sprintf(b2, "%lu", k);
			message("dbsort", MSG_DB, 32, 3, p, b1, b2);
		}

		if	(dbsort_debug)
			fprintf(stderr, "%ld Byte -> %s\n", n * in->reclen, p);
	}

	lfree(buf);
	memnotice(namebuf.data);
	*ptr = namebuf.data;
	return namebuf.used;
}


/*	Reduktion der Zwischendateien durch partielles mischen
*/

static size_t reduce(comp_t comp, size_t reclen, char **name, size_t dim, int mod)
{
	int i, j, n;
	int err;

	n = mod * (dim / mod);
	err = 0;

	for (i = 0; i < n; i += mod)
		if ((err = xmerge(comp, name + i, mod, reclen))) break;

	if	(err == 0)
		err = xmerge(comp, name + n, dim - n, reclen);

	if	(err)
	{
		cleanup(name, dim);
		message("dbsort", MSG_DB, err, 0);
		return 0;
	}

	for (i = j = 0; i < dim; i++)
		if (name[i]) name[j++] = name[i];

	return j;
}


/*	Dateien mischen
*/

void db_fsort (const char *iname, const char *oname, size_t elsize,
	db_read_t read, db_write_t write, size_t blksize, comp_t comp, int uniq)
{
	DBFILE *in;
	DBFILE *out;
	size_t dim, mod;
	char **name;

	if	(elsize == 0)	return;

	in = db_xopen(iname, "rz", elsize, read, NULL);
	dim = db_presort(in, blksize, comp, &name);
	db_close(in);

	while (dim > NFILES)
	{
		mod = (dim + NFILES - 1) / NFILES;

		if	(mod > NFILES)	mod = NFILES;

		dim = reduce(comp, elsize, name, dim, mod);
	}

	out = db_xopen(oname, "wz", elsize, NULL, write);
	merge(comp, name, dim, out, uniq ? db_uniq : db_put);
	db_close(out);

	memfree(name);
}
