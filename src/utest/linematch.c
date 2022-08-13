#include <EFEU/UnitTest.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <regex.h>
#include <errno.h>
#include <assert.h>

#define	ALIGN(x, b)	((b) * (((x) + (b) - 1) / (b)))
#define	PLIST_BSIZE	16
#define	DLIST_BSIZE	16

typedef struct {
	const char *line;
	int stat;
	regex_t data;
} PatternEntry;

typedef struct {
	PatternEntry *entry;
	size_t size;
	size_t capacity;
} PatternList;

#define	PLIST_DECL(name)	PatternList name = { NULL, 0, 0 }

typedef struct {
	const char *line;
	size_t first_line;
	size_t last_line;
	regmatch_t first;
	regmatch_t last;
} DataEntry;

typedef struct {
	DataEntry *entry;
	size_t size;
	size_t capacity;
} DataList;

#define	DLIST_DECL(name)	DataList name = { NULL, 0, 0 }

static void plist_clear(PatternList *pl)
{
	while	(pl->size)
	{
		--pl->size;
		regfree(&pl->entry[pl->size].data);
	}
}

static int plist_set(PatternList *pl, UnitTestString *buf, size_t size)
{
	char err_buf[256];
	int errcnt = 0;
	plist_clear(pl);

	if	(pl->capacity < size)
	{
		pl->capacity = ALIGN(size, PLIST_BSIZE);
		pl->entry = realloc(pl->entry,
			pl->capacity * sizeof pl->entry[0]);
		assert(pl->entry);
	}

	pl->size = size;
	PatternEntry *entry = pl->entry;

	for (size_t n = 0; n < size; ++n)
	{
		entry->line = UnitTest_next(buf);
		entry->stat = regcomp(&entry->data, entry->line, REG_EXTENDED);

		if	(entry->stat != 0)
		{
			regerror(entry->stat, &entry->data,
				err_buf, sizeof err_buf);
			UnitTest_error("%s", err_buf);
			++errcnt;
		}

		++entry;
	}

	return errcnt;
}

static void data_init(DataList *data, UnitTestString *buf, size_t size)
{
	if	(data->capacity < size)
	{
		data->capacity = ALIGN(size, DLIST_BSIZE);
		data->entry = realloc(data->entry,
			data->capacity * sizeof data->entry[0]);
		assert(data->entry);
	}

	data->size = size;

	for (size_t n = 0; n < size; ++n)
	{
		data->entry[n].line = UnitTest_next(buf);
		data->entry[n].first.rm_so = -1;
		data->entry[n].last.rm_so = -1;
	}
}

static void list_lines(const char *pfx, UnitTestString *buf, size_t size)
{
	UnitTest_start(buf);

	for (size_t n = 0; n < size; ++n)
	{
		const char *p = UnitTest_next(buf);
		UnitTest_strinfo(NULL, 0, pfx, p, NULL);
	}
}


int UnitTest_linematch(const char *file, int line, 
	const char *str, const char *expr, const char *term)
{
	static DLIST_DECL(dlist);
	static PLIST_DECL(plist);
	static STRING_DECL(buf1);
	static STRING_DECL(buf2);

	if	(term == 0)	term = "(null)";
	else if	(*term == '"')	term = "(string)";

	size_t n1 = UnitTest_lsplit(&buf1, str);
	size_t n2 = UnitTest_lsplit(&buf2, expr);

	if	(!n1 && !n2)
		return UnitTest_result(file, line, 1, "%s = (null)", term);

	if	(!n1)
	{
		UnitTest_result(file, line, 0, "%s ~ (expression)", term);
		list_lines("??", &buf2, n2);
		return 0;
	}

	if	(!n2)
	{
		UnitTest_result(file, line, 0, "%s = (null)", term);
		list_lines("!!", &buf1, n1);
		return 0;
	}

	if	(plist_set(&plist, &buf2, n2) != 0)
	{
		UnitTest_result(file, line, 0, "error in regular expression");

		for (size_t n = 0; n < plist.size; ++n)
		{
			UnitTest_strinfo(NULL, 0,
				plist.entry[n].stat == 0 ? "??" : "!ERROR",
				plist.entry[n].line, NULL);
		}

		return 0;
	}

	data_init(&dlist, &buf1, n1);

//	size_t n = UnitTest_lsplit(&buf1, str);
	return UnitTest_result(file, line, 1, "%s", term);
}
