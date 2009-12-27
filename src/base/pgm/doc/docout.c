/*	Dokumentausgabefilter
	(c) 1999 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <DocOut.h>
#include <EFEU/mstring.h>
#include <EFEU/procenv.h>
#include <EFEU/parsub.h>
#include <EFEU/ioctrl.h>
#include <EFEU/Info.h>


/*	Tabelle mit Ausgabefunktionen
*/


static DocType_t TypeTab[] = {
	{ "test", "Testausgabe",
		DocOut_test, NULL },
	{ "term", "Terminalausgabe",
		DocOut_term, NULL },


	{ "SynTeX", "LaTeX-Dokument im Synthesis-Standard",
		DocOut_syntex, NULL },
	{ "SynPS", "PostScript-Dokument im Synthesis-Standard",
		DocOut_syntex, "tex2ps -n3 -" },

	{ "man", "Handbuchsource",
		DocOut_mroff, NULL },
	{ "nroff", "Nroff-formatiertes Handbuch",
		DocOut_mroff, "groff -Tlatin1 -t -man" },

	{ "sgml", "SGML-Ausgabe",
		DocOut_sgml, NULL },
	{ "txt", "Textausgabe mit Attributen",
		DocOut_sgml, "efeudoc_sgml -t txt -" },
	{ "filter", "Textausgabe ohne Attribute",
		DocOut_sgml, "efeudoc_sgml -t filter -" },
	{ "latex", "LaTeX-Dokument über SGML",
		DocOut_sgml, "efeudoc_sgml -t latex -" },
	{ "ps", "Postscript-Dokument über SGML",
		DocOut_sgml, "efeudoc_sgml -t ps -" },
	{ "cgi", "HTML-Dokument mit CGI-Kopf",
		DocOut_sgml, "efeudoc_sgml -t cgi -" },
	{ "html", "HTML-Dokument",
		DocOut_sgml, "efeudoc_sgml -t html -" },
};


DocType_t *GetDocType (const char *name)
{
	int i;

	for (i = 0; i < tabsize(TypeTab); i++)
		if	(mstrcmp(name, TypeTab[i].name) == 0)
			return TypeTab + i;

	return NULL;
}

/*	Ausgabefilter auflisten
*/

static void print_type (io_t *io, InfoNode_t *base)
{
	int i;

	for (i = 0; i < tabsize(TypeTab); i++)
		io_printf(io, "%s\t%s\n", TypeTab[i].name, TypeTab[i].desc);
}

void DocOutInfo (const char *name, const char *desc)
{
	AddInfo(NULL, name, mstrcpy(desc), print_type, NULL);
}


/*	Dokumentausgabe öffnen
*/

io_t *DocFilter (DocType_t *type, io_t *io)
{
	return (type && type->filter) ? type->filter(io) : io;
}

io_t *DocOut (DocType_t *type, const char *name)
{
	io_t *io;

	if	(type && type->post)
	{
		if	(name)
		{
			char *p = mstrpaste(*name == '|' ? NULL : " > ",
				type->post, name);
			io = io_popen(p, "w");
			memfree(p);
		}
		else	io = io_popen(type->post, "w");
	}
	else	io = io_fileopen(name, "wz");

	if	(type && type->filter)
		io = type->filter(io);

	return io;
}
