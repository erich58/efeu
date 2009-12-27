/*	Kopfdefinition für TeX
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/texutil.h>
#include <EFEU/locale.h>

int TeXcompletion = 1;

void TeXbegin(io_t *io, TeXpgfmt_t *pgfmt)
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


void TeXend(io_t *io)
{
	if	(TeXcompletion)
	{
		io_puts("\\end{document}\n", io);
	}
}
