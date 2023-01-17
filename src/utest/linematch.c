#include <EFEU/UnitTest.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <regex.h>
#include <errno.h>
#include <assert.h>

typedef struct PatternEntry PatternEntry;

struct PatternEntry {
	const char *line;
	int stat;
	regex_t data;
	PatternEntry *next;
};

typedef struct DataEntry DataEntry;

struct DataEntry {
	const char *line;
	regmatch_t match;
	int accepted;
	DataEntry *next;
};

typedef struct {
	PatternEntry *optional;
	PatternEntry *expected;
	DataEntry *dlist;
	int sloppy;
} LineMatch;

#define	MATCH_DECL(name)	\
LineMatch name = { NULL, NULL, NULL, 1 }

static PatternEntry **pushPattern(PatternEntry **base, const char *line)
{
	PatternEntry *p = malloc(sizeof *p);
	p->line = line;
	p->next = 0;
	p->stat = regcomp(&p->data, p->line, REG_EXTENDED);

	if	(p->stat != 0)
	{
		char err_buf[256];
		regerror(p->stat, &p->data, err_buf, sizeof err_buf);
		UnitTest_error("%s", err_buf);
	//	free(p);
	//	return base;
	}

	*base = p;
	return &(*base)->next;
}

static void showPattern(PatternEntry *p, const char *id)
{
	static STRING_DECL(buf);

	if	(!id)
		return;

	UnitTest_clear(&buf);
	const char *key;

	if	(*id == '!')
	{
		UnitTest_puts(UnitTest_color(2), &buf);
		key = "!?";
	}
	else
	{
		UnitTest_puts(UnitTest_color(3), &buf);
		key = "??";
	}

	UnitTest_puts(p->stat == 0 ? id + 1 : "error", &buf);
	UnitTest_puts(UnitTest_color(0), &buf);
	UnitTest_putc('\t', &buf);
	UnitTest_qputs(p->line, '/', &buf);
	UnitTest_info(NULL, 0, key, "%s", UnitTest_str(&buf));
}

static PatternEntry *popPattern(PatternEntry *base, const char *pfx)
{
	if	(base)
	{
		PatternEntry *p = base;
		base = base->next;
		showPattern(p, pfx);
		regfree(&p->data);
		free(p);
	}

	return base;
}

static DataEntry **pushData(DataEntry **base, const char *string)
{
	DataEntry *data = malloc(sizeof *data);
	data->line = string;
	data->next = NULL;
	data->accepted = 0;
	data->match.rm_so = -1;
	*base = data;
	return &(*base)->next;
}

static void showData(DataEntry *d)
{
	static STRING_DECL(buf);

	UnitTest_clear(&buf);
	const char *key = ".";
	int colored = 0;

	if	(d->match.rm_so < 0)
	{
		UnitTest_puts(UnitTest_color(2), &buf);
		UnitTest_puts("bad", &buf);
		UnitTest_puts(UnitTest_color(0), &buf);
		key = "!!";
	}
	else if	(d->accepted)
	{
		UnitTest_puts(".", &buf);
		key = "?~";
	}
	else
	{
		UnitTest_puts(UnitTest_color(1), &buf);
		UnitTest_puts("good", &buf);
		UnitTest_puts(UnitTest_color(0), &buf);
		key = "*~";
	}

	UnitTest_putc('\t', &buf);

	for (int n = 0; d->line[n]; ++n)
	{
		if	(n == d->match.rm_so)
		{
			UnitTest_puts(UnitTest_color(d->accepted ? 3 : 1), &buf);
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
	UnitTest_info(NULL, 0, key, "%s", buf.data);
}

static DataEntry *popData(DataEntry *base)
{
	if	(base)
	{
		DataEntry *data = base;
		base = base->next;
		showData(data);
		free(data);
	}

	return base;
}

static void sloppy(DataEntry *d)
{
	d->match.rm_so = 0;
	d->match.rm_eo = 0;
	d->accepted = 1;
}

static int match_exact(DataEntry *d, PatternEntry *p)
{
	if	(d && p && p->stat == 0)
		return regexec(&p->data, d->line, 1, &d->match, 0) == 0;
	
	return 0;
}

static int match_any(DataEntry *d, PatternEntry *p)
{
	while (p)
	{
		if	(match_exact(d, p))
		{
			d->accepted = 1;
			return 1;
		}

		p = p->next;
	}

	return 0;
}

static void setOption(LineMatch *match, const char *opt)
{
	if	(strstr(opt, "fussy"))
		match->sloppy = 0;

	if	(strstr(opt, "sloppy"))
		match->sloppy = 1;
}

static void match_clear(LineMatch *match)
{
	while (match->expected && match->dlist)
	{
		if	(match->expected->stat != 0)
		{
			match->expected = popPattern(match->expected, "!error");
		}
		else if	(match_exact(match->dlist, match->expected))
		{
			match->expected = popPattern(match->expected, NULL);
			match->dlist = popData(match->dlist);
		}
		else if	(match_any(match->dlist, match->optional))
		{
			match->dlist = popData(match->dlist);
		}
		else if	(match_exact(match->dlist->next, match->expected))
		{
			match->dlist = popData(match->dlist);
			match->expected = popPattern(match->expected, NULL);
			match->dlist = popData(match->dlist);
		}
		else if	(match_exact(match->dlist, match->expected->next))
		{
			match->expected = popPattern(match->expected, "!error");
			match->expected = popPattern(match->expected, NULL);
			match->dlist = popData(match->dlist);
		}
		else
		{
			match->expected = popPattern(match->expected, "!error");
			match->dlist = popData(match->dlist);
		}
	}

	while (match->dlist)
	{
		match_any(match->dlist, match->optional);
		match->dlist = popData(match->dlist);
	}

	while (match->expected)
		match->expected = popPattern(match->expected, "!missing");

	while (match->optional)
		match->optional = popPattern(match->optional, "?default");
}

static void match_init(LineMatch *match, const char *data, const char *expr)
{
	static STRING_DECL(buf_data);
	static STRING_DECL(buf_expr);
	const char *p;

	match_clear(match);
	match->sloppy = 1;

	/* Vergleichsvektor */

	UnitTest_lsplit(&buf_expr, expr);
	PatternEntry **exp = &match->expected;
	PatternEntry **opt = &match->optional;

	while ((p = UnitTest_next(&buf_expr)))
	{
		if	(*p == '@')
			setOption(match, p + 1);
		else if	(*p == '*')
			opt = pushPattern(opt, p + 1);
		else
			exp = pushPattern(exp, p);
	}

	/* Datenvektor */

	UnitTest_lsplit(&buf_data, data);
	DataEntry **dp = &match->dlist;

	while ((p = UnitTest_next(&buf_data)))
		dp = pushData(dp, p);
}

int UnitTest_linematch(const char *file, int line, 
	const char *str, const char *expr, const char *term)
{
	static MATCH_DECL(mdata);

	if	(!term)	term = "NULL";

	match_init(&mdata, str, expr);

	PatternEntry *p = mdata.expected;
	DataEntry *d = mdata.dlist;

	while (d)
	{
		if	(match_exact(d, p))
			p = p->next;
		else if	(match_any(d, mdata.optional))
			;
		else if	(mdata.sloppy)
			sloppy(d);
		else
			break;

		d = d->next;
	}

	int res = (!p && !d);
	UnitTest_result(file, line, res, "linematch(%s)", term);

	while (mdata.expected != p)
		mdata.expected = popPattern(mdata.expected, NULL);

	while (mdata.dlist != d)
		mdata.dlist = popData(mdata.dlist);

	match_clear(&mdata);
	return res;
}
