/*	Speicherplatzanforderung
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.6

Falls die Speicherplatzanforderung fehlschlägt, kann nicht
auf die Standardfehlerroutinen zurückgegriffen werden, da
diese eventuell selbst ein Speichersegment anfordern müssen.
*/

#include <EFEU/memalloc.h>
#include <malloc.h>

#define	MEMCPY	0	/* memcpy, memset protokollieren */
#define	LCHECK	0	/* Langer Testbereich */

#define	ERR	"sorry: malloc(%lu) failed\n"


/*	Segmentkopf
*/

#if	LCHECK
#define	CHECK_SIZE	32
#define	CHECK_MASK	"12345678123456781234567812345678"
#else
#define	CHECK_SIZE	8
#define	CHECK_MASK	"12345678"
#endif

typedef struct {
#if	REFBYTEORDER
	size_t size;
	char check[CHECK_SIZE];
#else
	char check[CHECK_SIZE];
	size_t size;
#endif
} MEMHEAD;


/*	Protokollierung
*/

static FILE *logfile = NULL;
static size_t levent = 1;

static void print_event(char *name, void *p, size_t size)
{
	if	(logfile == NULL)
	{
		char *p = getenv("MEMLOG");
		logfile = p ? fopen(p, "w") : NULL;
	}

	if	(logfile == NULL)	logfile = stderr;

	fprintf(logfile, "%lu\t%07lu\t%s\t%lu\n", (size_t) p, levent++,
		name, size);
	fflush(logfile);
}

/*	Speicherfeldverwaltung
*/

#define	PTR_BSIZE	1024

static void **ptr_tab = NULL;
static size_t tab_size = 0;

static void tab_realloc()
{
	void **old_tab;
	size_t i, old_size;

	old_tab = ptr_tab;
	old_size = tab_size;
	tab_size += PTR_BSIZE;
	ptr_tab = malloc(tab_size * sizeof(void *));

	if	(ptr_tab == NULL)
	{
		fprintf(stderr, ERR, tab_size * sizeof(void *));
		exit(EXIT_FAILURE);
		return;
	}

	for (i = 0; i < old_size; i++)
		ptr_tab[i] = old_tab[i];

	for (i = old_size; i < tab_size; i++)
		ptr_tab[i] = NULL;

	if	(old_size)
		print_event("deltab", old_tab, old_size * sizeof(void *));

	print_event("newtab", ptr_tab, tab_size * sizeof(void *));
}

static void tab_check()
{
	size_t i;

	for (i = 0; i < tab_size; i++)
		lcheck(ptr_tab[i]);
}

static void add_entry(void *p, size_t size)
{
	size_t i;

	print_event("alloc", p, size);

	for (i = 0; i < tab_size; i++)
	{
		if	(ptr_tab[i] == NULL)
		{
			ptr_tab[i] = p;
			return;
		}
	}

	tab_realloc(p);
	ptr_tab[i] = p;
}


static void clr_entry(void *p, size_t size)
{
	size_t i;

	for (i = 0; i < tab_size; i++)
	{
		if	(ptr_tab[i] == p)
		{
			ptr_tab[i] = NULL;
			print_event("free", p, size);
			return;
		}
	}

	print_event("unknown", p, size);
}

/*	Anfordern eines Speichersegmentes
*/

void *lmalloc(size_t size)
{
	MEMHEAD *head;
	char *p;

	tab_check();

	if	(size == 0)	return NULL;

	head = malloc(size + sizeof(MEMHEAD) + CHECK_SIZE);

	if	(head)
	{
		p = (char *) (head + 1);
		head->size = size;
		memcpy(head->check, CHECK_MASK, CHECK_SIZE);
		memcpy(p + size, CHECK_MASK, CHECK_SIZE);
		add_entry(p, size);
		return p;
	}
	else
	{
		fprintf(stderr, ERR, size);
		exit(EXIT_FAILURE);
		return NULL;
	}
}


/*	Freigabe eines Speichersegmentes
*/

void lfree(void *p)
{
	tab_check();

	if	(p != NULL)
	{
		/*
		mprobe(((MEMHEAD *) p)[-1]);
		*/
		lcheck(p);
		clr_entry(p, ((MEMHEAD *) p)[-1].size);
		free(((MEMHEAD *) p) -1);
	}
}


/*	Speicherfeld überprü+fen
*/

void lcheck(void *p)
{
	MEMHEAD *head;

	if	(p == NULL)	return;

	head = ((MEMHEAD *) p) - 1;

	if	(memcmp(head->check, CHECK_MASK, CHECK_SIZE) != 0)
		print_event("errbeg", p, head->size);

	if	(memcmp(((char *) p) + head->size, CHECK_MASK, CHECK_SIZE) != 0)
		print_event("errend", p, head->size);
}


#if	MEMCPY

/*	Speicherfeld initialisieren
*/

void *memset(void *p, int c, size_t size)
{
	size_t i;

	print_event("memset", p, size);

	for (i = 0; i < size; i++)
		((char *) p)[i] = c;

	return p;
}

/*	Speicherfeld kopieren
*/

void *memcpy(void *desc, const void *src, size_t size)
{
	size_t i;

	print_event("memcpy", desc, size);

	for (i = 0; i < size; i++)
		((char *) desc)[i] = ((char *) src)[i];

	return desc;
}
#endif
