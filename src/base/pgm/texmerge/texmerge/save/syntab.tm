/*	Hilfsfunktionen zur Tabellengenerierung
	(c) 1997 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#ifndef	_TM_SYNTAB
#define	_TM_SYNTAB	1

#include <tablabel.tm>


/*	Tabellenumgebung
*/

struct SynTab {
	IO io;		/* Ausgabestruktur */
	int cols;	/* Aktuelle Spaltenzahl */
};

SynTab SynTab (IO io, bool clrpage = true)
{
	if	(clrpage)	io << "\n\\clearpage\n";

	io << "\\begin{SynTable}\n";
	return { io, 0 };
}

SynTab SynTab (IO io, int margin, bool clrpage = true)
{
	if	(clrpage)	io << "\n\\clearpage\n";

	fprintf(io, "\\begin{SynTable}[%d]\n", margin);
	return { io, 0 };
}

void SynTab ()
	this.io << "\\end{SynTable}\n";

void SynTab::printf(str fmt, ...)
	fprintf(this.io, fmt, va_list)

void SynTab::line(str arg)
	fprintf(this.io, "%s\n", arg)
	
void SynTab::caption(str hname, str sname)
	this.printf("\\caption{%s}\n\\subcaption{%s}\n", hname, sname)

void SynTab::def(str name, str def)
	this.printf("\\def\\%s{%s}\n", name, def)

virtual void SynTab::note(str name)
	this.printf("\\note{%s}\n", name);

virtual void SynTab::note(...)
	for (x in va_list) this.printf("\\note{%s}\n", x);

/*
virtual void SynTab::cmdline(str cmd)
	this.printf("\\%s\n", cmd)

virtual void SynTab::cmdline(str cmd, str arg)
	this.printf("\\%s{%s}\n", cmd, arg)

virtual void SynTab::cmdline(str cmd, str opt, str arg)
	this.printf("\\%s[%s]{%s}\n", cmd, opt, arg)
*/

void SynTab::begin(TabLabel col, int width = 2, int height = 2)
{
	if	(!col)	col = CDummyTabLabel;

	this.printf("\n\\begin{SynTab}{%d}", height);

	str delim = "{";

	for (x in col.tab)
	{
		this.printf("%s%d", delim, x.width ? x.width : width);
		delim = ",";
	}

	this.io << "}\n\\thislinebold\n"
	this.io << col.special;
	this.io << "\\norule";

	for (x in col.tab)
		this.printf(" & %s", x.label);

	this.io << " \\cr\n";
	this.cols = dim(col.tab);
}

virtual void SynTab::tabline(str label)
{
	this.io << label;

	for (int i = 0; i < this.cols; i++)
		this.io << " &";

	this.io << " \\cr\n";
}

virtual void SynTab::tabline(str label, ...)
{
	PrintListDelim = " & ";
	PrintListBegin = NULL;
	PrintListEnd = NULL;
	fmt_str = "%*s";

	this.io << label << " & ";
	this.io << va_list;
	this.io << " \\cr\n";
}

void SynTab::body(TabLabel line, mdmat md, VirFunc idx = NULL, int lag = 0)
{
	if	(!line)	line = LDummyTabLabel;

	this.io << line.special;

	for (x in line.tab)
	{
		this.io << x.special;

		if	(md && x.name)
			this.tabline(x.label, md.data(x.name, idx, lag));
		else	this.tabline(x.label);
	}
}

void SynTab::end()
{
	this.cols = 0;
	this.io << "\\end{SynTab}\n\n";
}

#endif	/* _TM_SYNTAB */
