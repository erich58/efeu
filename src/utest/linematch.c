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

typedef struct {
	const char *line;
	regmatch_t match;
} DataEntry;

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


typedef struct {
	PatternEntry *pattern;
	size_t psize;
	size_t pcapacity;
	DataEntry *data;
	size_t dsize;
	size_t dcapacity;
} LineMatch;

#define	MATCH_DECL(name)	\
LineMatch name = { NULL, 0, 0, NULL, 0, 0 }

static void match_clear(LineMatch *match)
{
	while	(match->psize)
	{
		--match->psize;
		regfree(&match->pattern[match->psize].data);
	}

	match->dsize = 0;
}

static int match_init(LineMatch *match, const char *data, const char *expr)
{
	static STRING_DECL(buf_data);
	static STRING_DECL(buf_expr);
	size_t n;

	match_clear(match);
	n = UnitTest_lsplit(&buf_data, data);

	if	(match->dcapacity < n)
	{
		match->dcapacity = ALIGN(n, DLIST_BSIZE);
		match->data = realloc(match->data,
			match->dcapacity * sizeof match->data[0]);
		assert(match->data);
	}

	match->dsize = n;

	for (n = 0; n < match->dsize; ++n)
		setData(match->data + n, UnitTest_next(&buf_data));

	n = UnitTest_lsplit(&buf_expr, expr);

	if	(match->pcapacity < n)
	{
		match->pcapacity = ALIGN(n, PLIST_BSIZE);
		match->pattern = realloc(match->pattern,
			match->pcapacity * sizeof match->pattern[0]);
		assert(match->pattern);
	}

	match->psize = n;
	int errcnt = 0;

	for (n = 0; n < match->psize; ++n)
		errcnt += setPattern(match->pattern + n,
			UnitTest_next(&buf_expr));

	return errcnt;
}

static int match_cmp(LineMatch *match, int p, int d)
{
	return regexec(&match->pattern[p].data,
		match->data[d].line, 1, &match->data[d].match, 0) == 0;
}

int UnitTest_linematch(const char *file, int line, 
	const char *str, const char *expr, const char *term)
{
	static MATCH_DECL(mdata);
	static STRING_DECL(buf1);
	size_t n;

	if	(term == 0)	term = "(null)";
	else if	(*term == '"')	term = "(string)";

	match_clear(&mdata);

	if	(match_init(&mdata, str, expr) != 0)
	{
		UnitTest_result(file, line, 0, "error in regular expression");

		for (n = 0; n < mdata.psize; ++n)
			showPattern(mdata.pattern + n);

		return 0;
	}

	if	(!mdata.psize && !mdata.dsize)
		return UnitTest_result(file, line, 1, "%s = (null)", term);

	if	(!mdata.dsize)
	{
		UnitTest_result(file, line, 0, "%s ~ (expression)", term);

		for (n = 0; n < mdata.psize; ++n)
			showPattern(mdata.pattern + n);

		return 0;
	}

	if	(!mdata.psize)
	{
		UnitTest_result(file, line, 0, "%s = (null)", term);

		for (n = 0; n < mdata.dsize; ++n)
			showData(mdata.data + n);

		return 0;
	}

	/* Gute Vergleiche */

	for (n = 0; n < mdata.dsize && n < mdata.psize; ++n)
	{
		if	(!match_cmp(&mdata, n, n))
			break;
	}

	int res = (n == mdata.dsize && n == mdata.psize);
	UnitTest_result(file, line, res, "%s ~ (expression)", term);

	for (size_t i = 0; i < n; ++i)
		showData(mdata.data + i);

	if	(res)
		return res;

	/* List Bad matches
	*/

	size_t p = n;
	size_t d = n;

	while (p < mdata.psize && d < mdata.dsize)
	{
		if	(match_cmp(&mdata, p, d))
		{
			showData(mdata.data + d);
		}
		else if	(match_cmp(&mdata, p, d + 1))
		{
			showData(mdata.data + d);
			++d;
			showData(mdata.data + d);
		}
		else if	(match_cmp(&mdata, p + 1, d))
		{
			showPattern(mdata.pattern + p);
			++p;
			showData(mdata.data + d);
		}
		else
		{
			showPattern(mdata.pattern + p);
			showData(mdata.data + d);
		}

		++p;
		++d;
	}

	while (d < mdata.dsize)
	{
		showData(mdata.data + d);
		++d;
	}

	while (p < mdata.psize)
	{
		showPattern(mdata.pattern + p);
		++p;
	}

	return res;
}
