/*	TeX-Font Verwaltung
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/texutil.h>

static TeXfont_t fontdef[] = {
	{  6,	"tiny",		1.29,	2.6 },
	{  8,	"scriptsize",	1.50,	3.5 },
	{  9,	"footnotesize",	1.63,	4.0 },
	{ 10,	"small",	1.76,	4.3 },
	{ 11,	"normalsize",	1.94,	4.9 },
	{ 12,	"large",	2.11,	5.0 },
	{ 14,	"Large",	2.44,	6.4 },
	{ 17,	"LARGE",	3.05,	7.8 },
};


/*
#define	FONTDIM	tabsize(fontdef)

static int minfont = 0;
static int maxfont = FONTDIM - 2;
*/


TeXfont_t *TeXfont(int psize)
{
	int n;

	n = tabsize(fontdef);

	while (--n > 0 && psize < fontdef[n].size)
		;

	return fontdef + n;
}

/*
void TeXfontwalk(visit)

int (*visit) (TeXfont_t *font);

{
	if	(visit)
	{
		int i;

		for (i = minfont; i < maxfont; i++)
			if (!visit(fontdef + i)) break;
	}
}


void TeXfontinit()

{
	for (minfont = 0; minfont < maxfont - 1; minfont++)
		if (fontdef[minfont].size >= TeXfontsize) break;
}


void *TeXfontlist(func, data, arg)

Func_t *func;
void *data;
void **arg;

{
	int i;
	char *p;

	p = NULL;

	for (i = minfont; i < maxfont; i++)
	{
		io_puts(p, iostd);
		io_printf(iostd, "%dpt", fontdef[i].size);
		p = ", ";
	}

	io_putc('\n', iostd);
	return NULL;
}
*/
