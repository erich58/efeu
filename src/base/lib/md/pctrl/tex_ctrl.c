/*	Ausgabekontrolle für TeX
	(c) 1994 Erich Frühstück
*/

#include <EFEU/pctrl.h>
#include <EFEU/locale.h>

static int quote_flag = 0;

/*	Fontdefinitionen
*/

typedef struct {
	int size;	/* Basisgröße */
	char *name;	/* Schriftname */
	double width;	/* Ziffernbreite in mm */
	double height;	/* Zeilenhöhe in mm */
} TEXFONT;


static TEXFONT fontdef[] = {
	{  6,	"tiny",		1.29,	2.6 },
	{  8,	"scriptsize",	1.50,	3.5 },
	{  9,	"footnotesize",	1.63,	4.0 },
	{ 10,	"small",	1.76,	4.3 },
	{ 11,	"normalsize",	1.94,	4.9 },
	{ 12,	"large",	2.11,	5.0 },
	{ 14,	"Large",	2.44,	6.4 },
	{ 17,	"LARGE",	3.05,	7.8 },
};


static TEXFONT *font = NULL;

static TEXFONT *TeXfont(int psize)
{
	int n;

	n = tabsize(fontdef);

	while (--n > 0 && psize < fontdef[n].size)
		;

	return fontdef + n;
}


/*	Seitenformat
*/

typedef struct {
	char *name;	/* Papiername */
	double margin;	/* Linker Rand in mm */
	double topskip;	/* Oberer Rand in mm */
	double width;	/* Papierbreite in mm */
	double height;	/* Papierhöhe in mm */
} PGFMT;


static PGFMT pgfmttab[] = {
	{ "a4",		16.,	19.,	170.,	245. },
	{ "a4r",	15.,	18.,	250.,	160. },
	{ "schmal",	20.,	10.,	50.,	220. },
	{ "kurz",	20.,	15.,	220.,	50. },
	{ "klein",	20.,	10.,	50.,	50. },
};

static PGFMT *pgfmt = NULL;


static PGFMT *texpgfmt(const char *name)
{
	int i;

	for (i = 0; i < tabsize(pgfmt); i++)
		if (strcmp(name, pgfmttab[i].name) == 0)
			return (pgfmttab + i);

	return pgfmttab;
}

static int TeXcompletion = 1;

static void TeXbegin(IO *io, PGFMT *pgfmt)
{
	if	(TeXcompletion)
	{
		io_puts("\\documentclass[11pt,a4paper]{article}\n", io);
		io_puts("\\usepackage[latin1]{inputenc}\n", io);
		io_puts("\\usepackage{german}\n", io);
		io_puts("\\dateaustrian\n", io);

		PushLocale();
		SetLocale(LOC_PRINT, "us");
		io_printf(io, "\\topmargin=%gmm\n", pgfmt->topskip);
		io_printf(io, "\\oddsidemargin=%gmm\n", pgfmt->margin);
		io_printf(io, "\\textwidth=%gmm\n", pgfmt->width);
		io_printf(io, "\\textheight=%gmm\n", pgfmt->height);
		PopLocale();

		io_puts("\\voffset=-25mm\\hoffset=-25mm\n", io);
		io_puts("\\headheight=0mm\\headsep=0mm\n", io);
		io_puts("\\evensidemargin=\\oddsidemargin\n", io);
		io_puts("\\footskip=15mm\n", io);
		io_puts("\\parskip\\baselineskip\n", io);
		io_puts("\\begin{document}\n", io);
		io_puts("\\raggedright\n", io);
	}
}

static void TeXend(IO *io)
{
	if	(TeXcompletion)
		io_puts("\\end{document}\n", io);
}

int tex_ctrl(PCTRL *pf, int cmd, va_list list)
{
	if	(!pgfmt)	pgfmt = texpgfmt(pctrl_pgfmt);
	if	(!font)		font = TeXfont(pctrl_fsize);

	switch (cmd)
	{
	case PCTRL_COLUMNS:

		return (int) (pgfmt->width / font->width);

	case PCTRL_LINES:

		return (int) (pgfmt->height / font->height);

	case PCTRL_BEGIN:

		pgfmt = texpgfmt(pctrl_pgfmt);
		font = TeXfont(pctrl_fsize);
		pf->delim = " & ";
		TeXbegin(pf->io, pgfmt);
		io_putc('\\', pf->io);
		io_puts(font->name, pf->io);
		io_putc('\n', pf->io);
		pf->put = NULL;
		break;

	case PCTRL_DATA:

		io_puts("\n\\begin{tabular}{|l|", pf->io);
		io_printf(pf->io, "*{%d}{r|}", va_arg(list, int));
		io_puts("}\n\\hline\n", pf->io);
		pf->nl = " \\\\\n";
		break;

	case PCTRL_EHEAD:

		io_puts("\\hline\n", pf->io);
		break;

	case PCTRL_EDATA:

		io_puts("\\hline\n", pf->io);
		io_puts("\\end{tabular}\n", pf->io);
		pf->nl = "~\\\\\n";
		break;

	case PCTRL_END:

		TeXend(pf->io);
		break;

	case PCTRL_LEFT:
	case PCTRL_CENTER:
	case PCTRL_RIGHT:
	case PCTRL_EMPTY:
	
		quote_flag = 0;
		pf->put = tex_put;
		break;

	case PCTRL_VALUE:

		quote_flag = 0;
		pf->put = std_put;
		break;

	default:	

		break;
	}

	return 0;
}

int tab_ctrl(PCTRL *pf, int cmd, va_list list)
{
	TeXcompletion = 0;
	return tex_ctrl(pf, cmd, list);
}


int tex_put(int c, PCTRL *pf)
{
	switch (c)
	{
	case '%': case '$': case '#': case '_':
	case '&': case '{': case '}':

		io_putc('\\', pf->io);
		io_putc(c, pf->io);
		break;

	case '^': case '~':

		io_putc('\\', pf->io);
		io_putc(c, pf->io);
		io_putc('~', pf->io);
		break;

	case '|': case '[': case ']': case '<': case '>':

		io_putc('$', pf->io);
		io_putc(c, pf->io);
		io_putc('$', pf->io);
		break;

	case '"':

		quote_flag = !quote_flag;
		io_puts(quote_flag ? "\"`" : "\"´", pf->io);
		break;

	case '\\': io_puts("$\\backslash$", pf->io); break;
	case '*':  io_puts("$\\ast$", pf->io); break;

	case 'Ä': io_puts("\"A", pf->io); break;
	case 'Ö': io_puts("\"O", pf->io); break;
	case 'Ü': io_puts("\"U", pf->io); break;
	case 'ä': io_puts("\"a", pf->io); break;
	case 'ö': io_puts("\"o", pf->io); break;
	case 'ü': io_puts("\"u", pf->io); break;
	case 'ß': io_puts("\"s", pf->io); break;
	case EOF:	break;

	default:

		if	(c < ' ' || c >= 127)
		{
			io_puts("$\\bullet$", pf->io);
		}
		else	io_putc(c, pf->io);

		break;
	}

	return c;
}
