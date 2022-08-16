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

#define	VECTOR(type)	struct {  \
	type *entry; \
	size_t size; \
	size_t capacity; \
}

#define	VECTOR_INIT	{ NULL, 0, 0 }

#define	VECTOR_RESERVE(var, n)	\
	if	((var).capacity < n) \
	{ \
		(var).capacity = ALIGN(n, 16); \
		(var).entry = realloc((var).entry, \
			(var).capacity * sizeof (var).entry[0]); \
		assert((var).entry); \
	}

typedef struct {
	const char *line;
	int stat;
	regex_t data;
} PatternEntry;

typedef VECTOR(PatternEntry) Pattern;

typedef struct {
	const char *line;
	regmatch_t match;
} DataEntry;

typedef VECTOR(DataEntry) Data;

typedef struct {
	Pattern pattern;
	Data data;
} LineMatch;

#define	MATCH_DECL(name)	\
LineMatch name = { VECTOR_INIT, VECTOR_INIT }

static int setPattern(PatternEntry *p, const char *line)
{
	p->line = line;
	p->stat = regcomp(&p->data, p->line, REG_EXTENDED);

	if	(p->stat != 0)
	{
		char err_buf[256];
		regerror(p->stat, &p->data, err_buf, sizeof err_buf);
		UnitTest_error("%s", err_buf);
		return 1;
	}

	return 0;
}

static void showPattern(PatternEntry *p)
{
	UnitTest_strinfo(NULL, 0, p->stat == 0 ? "??" : "!ERROR",
		p->line, NULL);
}

static void setData(DataEntry *data, const char *string)
{
	data->line = string;
	data->match.rm_so = -1;
}

static void showData(DataEntry *d)
{
	static STRING_DECL(buf);

	if	(d->match.rm_so < 0)
	{
		UnitTest_info(NULL, 0, "!!", "%s", d->line);
		return;
	}

	UnitTest_clear(&buf);
	int colored = 0;

	for (int n = 0; d->line[n]; ++n)
	{
		if	(n == d->match.rm_so)
		{
			UnitTest_puts(UnitTest_color(1), &buf);
			colored = 1;
		}
		else if	(n == d->match.rm_eo)
		{
			UnitTest_puts(UnitTest_color(0), &buf);
			colored = 0;
		}

		UnitTest_putc(d->line[n], &buf);
	}

	if	(colored)
		UnitTest_puts(UnitTest_color(0), &buf);

	UnitTest_putc(0, &buf);
	UnitTest_info(NULL, 0, "*~", "%s", buf.data);
}


static void match_clear(LineMatch *match)
{
	while	(match->pattern.size)
	{
		--match->pattern.size;
		regfree(&match->pattern.entry[match->pattern.size].data);
	}

	match->data.size = 0;
}

static void match_init(LineMatch *match, const char *data, const char *expr)
{
	static STRING_DECL(buf_data);
	static STRING_DECL(buf_expr);
	size_t n;

	match_clear(match);
	n = UnitTest_lsplit(&buf_data, data);
	VECTOR_RESERVE(match->data, n);
	match->data.size = n;

	for (n = 0; n < match->data.size; ++n)
		setData(match->data.entry + n, UnitTest_next(&buf_data));

	n = UnitTest_lsplit(&buf_expr, expr);
	VECTOR_RESERVE(match->pattern, n);
	match->pattern.size = n;

	for (n = 0; n < match->pattern.size; ++n)
		setPattern(match->pattern.entry + n, UnitTest_next(&buf_expr));
}

static int match_cmp(LineMatch *match, int p, int d)
{
	if	(match->pattern.entry[p].stat != 0)
		return 0;
	return regexec(&match->pattern.entry[p].data,
		match->data.entry[d].line, 1, &match->data.entry[d].match, 0) == 0;
}

int UnitTest_linematch(const char *file, int line, 
	const char *str, const char *expr, const char *term)
{
	static MATCH_DECL(mdata);
	size_t n;

	if	(term == 0)	term = "NULL";

	match_init(&mdata, str, expr);

	/* Gute Vergleiche */

	for (n = 0; n < mdata.data.size && n < mdata.pattern.size; ++n)
	{
		if	(!match_cmp(&mdata, n, n))
			break;
	}

	int res = (n == mdata.data.size && n == mdata.pattern.size);
	UnitTest_result(file, line, res, "linematch(%s)", term);

	for (size_t i = 0; i < n; ++i)
		showData(mdata.data.entry + i);

	if	(res)
		return res;

	/* List Bad matches
	*/

	size_t p = n;
	size_t d = n;

	while (p < mdata.pattern.size && d < mdata.data.size)
	{
		if	(match_cmp(&mdata, p, d))
		{
			showData(mdata.data.entry + d);
		}
		else if	(match_cmp(&mdata, p, d + 1))
		{
			showData(mdata.data.entry + d);
			++d;
			showData(mdata.data.entry + d);
		}
		else if	(match_cmp(&mdata, p + 1, d))
		{
			showPattern(mdata.pattern.entry + p);
			++p;
			showData(mdata.data.entry + d);
		}
		else
		{
			showPattern(mdata.pattern.entry + p);
			showData(mdata.data.entry + d);
		}

		++p;
		++d;
	}

	while (d < mdata.data.size)
	{
		showData(mdata.data.entry + d);
		++d;
	}

	while (p < mdata.pattern.size)
	{
		showPattern(mdata.pattern.entry + p);
		++p;
	}

	return res;
}
