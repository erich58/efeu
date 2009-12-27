/*	Zeichenfilter
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include <EFEU/Document.h>
#include <ctype.h>

#define	DEF_TABSTOP	8	/* Standardtabellenposition */
#define	DEF_BREAKCOL	70	/* Standardumbruchspalte */

#define XLIST "ÄÖÜäöüß"

static MergeFilter_t *filter_admin (MergeFilter_t *tg, const MergeFilter_t *src)
{
	int i;
	uchar_t *p;

	if	(tg)
	{
		for (i = 0; i < 256; i++)
			memfree(tg->tab[i]);

		memfree(tg);
		return NULL;
	}

	tg = memalloc(sizeof(MergeFilter_t));
	tg->tabstop = DEF_TABSTOP;
	tg->breakcol = DEF_BREAKCOL;
	tg->tab['\n'] = mstrcpy("\n");
	tg->tab['\t'] = mstrcpy(" ");

	for (i = 32; i < 128; i++)
		tg->tab[i] = msprintf("%c", i);

	for (p = XLIST; *p != 0; p++)
		tg->tab[*p] = msprintf("%c", *p);

	return tg;
}

REFTYPE(MergeFilter_reftype, "MergeFilter", NULL, filter_admin);

/*	Gefilterte Zeichenausgabe
*/

static void stdput (int c, io_t *io)
{
	if	(c == '\n')	io_puts(" \\\n", io);
	else if	(c == '\t')	io_putc(' ', io);
	else if	(c < 32)	io_printf(io, "^%c", c + '@');
	else if	(c >= 127)	io_printf(io, "\\%03o", c);
	else			io_putc(c, io);
}

#define	breakpos(x)	((x)->breakcol && (x)->pos > (x)->breakcol)

void MergeFilter_put (MergeFilter_t *fpar, int c, io_t *out)
{
	if	(fpar == NULL)
	{
		stdput(c, out);
	}
	else if	(c == '\n' || (c == ' ' && breakpos(fpar)))
	{
		io_puts(fpar->tab['\n'], out);
		fpar->pos = 0;
	}
	else if	(c == '\t' && fpar->tabstop)
	{
		char *s = fpar->tab[' '];

		do	io_puts(s, out);
		while	(++fpar->pos % fpar->tabstop != 0);
	}
	else
	{
		char *s = fpar->tab[c & 0xFF];

		if	(s)
		{
			io_puts(s, out);
			fpar->pos++;
		}
	}
}
