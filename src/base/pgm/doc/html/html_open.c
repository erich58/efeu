/*
HTML-Ausgabefilter

$Copyright (C) 2000 Erich Frühstück
This file is part of EFEU.

EFEU is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

EFEU is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public
License along with EFEU; see the file COPYING.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <EFEU/io.h>
#include <EFEU/ioctrl.h>
#include <EFEU/mstring.h>
#include <EFEU/parsub.h>
#include <EFEU/MakeDepend.h>
#include <EFEU/pconfig.h>
#include <EFEU/patcmp.h>
#include <EFEU/LangDef.h>
#include <HTML.h>
#include <DocCtrl.h>
#include <ctype.h>

#define	BUF_SIZE	(1024 * 1024)
#define	TMP_PFX		"html_"


/*	Eingabestruktur
*/

typedef struct {
	IO *io;	/* Aktuelle Ausgabestruktur */
	IO *tmp;	/* Temporärer Buffer */
	char *path;	/* Bibliothek */
	char *title;	/* Dokumenttitel */
	char *fname;	/* Aktuelle Datei */
	StrBuf *ref;	/* Referenz */
	StrBuf *toc;	/* Inhaltsverzeichnis */
	StrBuf *lof;	/* Übersichtsverzeichnis */
	VecBuf idx;	/* Indexvektor */
	VecBuf lbl;	/* Labelvektor */
	int chap;	/* Kapitelnummer */
	int sec;	/* Abschnittsnummer */
	int last;	/* Letztes ausgegebene Zeichen */
	int protect;	/* Schutzmodus */
	int app;	/* Flag für Anhang */
	int anum;	/* Ankernummer */
} HPOST;


/*	Aufspaltung generieren
*/

static void hpost_split(HPOST *hpost)
{
	memfree(hpost->fname);

	if	(hpost->chap < 0)
	{
		hpost->fname = msprintf("app_%d.html", -hpost->chap);
	}
	else	hpost->fname = msprintf("chap_%d.html", hpost->chap);

	io_printf(hpost->tmp, "<>%s\n", hpost->fname);
}


/*	Indexeinträge
*/

typedef struct {
	int num;	/* Indexnummer */
	char *fname;	/* Aktuelle Datei */
	char *name;	/* Eintragsname */
	char *label;	/* Bezeichnung */
} HIDX;

static int hidx_cmp (const void *pa, const void *pb)
{
	const HIDX *a = pa;
	const HIDX *b = pb;
	int n = lexcmp(a->name, b->name);

	if	(n != 0)		return n;
	else if	(a->num < b->num)	return -1;
	else if	(a->num > b->num)	return 1;
	else				return 0;
}

static void hpost_idx (HPOST *hpost, va_list list)
{
	HIDX *x = vb_next(&hpost->idx);
	x->num = hpost->idx.used;
	x->fname = mstrcpy(hpost->fname);
	x->name = mstrcpy(va_arg(list, char *));
	x->label = mstrcpy(va_arg(list, char *));
	io_printf(hpost->tmp, "<A NAME=\"IDX%d\"></A>", x->num);
}

static void hpost_lbl (HPOST *hpost, va_list list)
{
	HIDX *x = vb_next(&hpost->lbl);
	x->num = hpost->lbl.used;
	x->fname = mstrcpy(hpost->fname);
	x->name = mstrcpy(va_arg(list, char *));
	x->label = NULL;
	io_printf(hpost->tmp, "<A NAME=\"LBL%d\"></A>", x->num);
}

/*	Zeichen ausgeben
*/

static int hpost_put (int c, void *ptr)
{
	HPOST *hpost = ptr;
	hpost->last = c;
	return io_putc(c, hpost->tmp);
}

static void hpost_open (HPOST *hpost, const char *name, const char *title)
{
	char *p;

	p = mstrpaste("/", hpost->path, name);
	hpost->io = io_fileopen(p, "wdz");
	memfree(p);

	io_puts("<!DOCTYPE HTML PUBLIC ", hpost->io);
	io_puts("\"-//W3C//DTD HTML 3.2 Final//EN\">\n", hpost->io);
	io_puts("<HTML>\n<HEAD>\n<TITLE>", hpost->io);

	if	(title || hpost->title)
	{
		io_puts(hpost->title, hpost->io);

		if	(title && hpost->title)
			io_puts(" - ", hpost->io);

		io_puts(title, hpost->io);
	}
	else	io_langputs(":*:article:de:Artikel", hpost->io);

	io_puts("</TITLE>\n</HEAD>\n<BODY>\n", hpost->io);
}

static void hpost_close (HPOST *hpost)
{
	if	(!hpost->io)	return;

	io_puts("\n<P><HR><P>\n", hpost->io);
	io_langputs(":*:go to\n:de:Gehe zum\n", hpost->io);

	if	(hpost->ref->pos)
	{
		sb_putc(0, hpost->ref);
		io_puts((char *) hpost->ref->data, hpost->io);
		io_langputs(" :*:section:de:Abschnitt", hpost->io);
		io_puts(",\n", hpost->io);
	}

	io_puts("<A HREF=\"main.html\">", hpost->io);
	io_langputs(":*:start of document:de:Dokumentanfang", hpost->io);
	io_puts("</A>,\n", hpost->io);
	io_puts("<A HREF=\"toc.html\">", hpost->io);
	io_langputs(":*:contents:de:Inhalt", hpost->io);
	io_puts("</A>,\n", hpost->io);
	io_puts("<A HREF=\"index.html\">", hpost->io);
	io_langputs(":*:index:de:Index", hpost->io);
	io_puts("</A>\n", hpost->io);
	io_puts("</BODY>\n", hpost->io);
	io_puts("</HTML>\n", hpost->io);
	io_close(hpost->io);
	hpost->io = NULL;
	sb_clear(hpost->ref);
}

static void hpost_next (HPOST *hpost)
{
	char *name, *title;
	char *fmt_prev, *fmt_next;
	int c;

	name = io_mgets(hpost->tmp, "%s");

	fmt_prev = "<A HREF=%#s>previous</A>";
	fmt_next = "<A HREF=%#s>next</A>";

	if	(mstrcmp(LangDef.language, "de") == 0)
	{
		fmt_prev = "<A HREF=%#s>nächsten</A>";
		fmt_next = "<A HREF=%#s>vorigen</A>";
	}

	do	c = io_getc(hpost->tmp);
	while	(c == ' ' || c == '\t');

	if	(c != '\n')
	{
		io_ungetc(c, hpost->tmp);
		title = io_xgets(hpost->tmp, "\n");
	}
	else	title = NULL;

	if	(hpost->io)
	{
		if	(hpost->ref->pos)
			sb_puts(", ", hpost->ref);

		sb_printf(hpost->ref, fmt_prev, name);
		hpost_close(hpost);
		sb_printf(hpost->ref, fmt_next, hpost->fname);
	}

	hpost_open (hpost, name, title);
	memfree(hpost->fname);
	hpost->fname = name;
	memfree(title);
}

static void hpost_ref (HPOST *hpost)
{
	HIDX *lbl;
	size_t n;
	char *name;

	name = io_mgets(hpost->tmp, ">");
	io_getc(hpost->tmp);

	for (lbl = hpost->lbl.data, n = hpost->lbl.used; n-- > 0; lbl++)
	{
		if	(mstrcmp(lbl->name, name) == 0)
		{
			io_printf(hpost->io, "<A HREF=\"%s#LBL%d\">",
				lbl->fname, lbl->num);
			io_printf(hpost->io, "[%s]</A>", name);
			memfree(name);
			return;
		}
	}

	io_printf(hpost->io, "?%s?", name);
	memfree(name);
	return;
}

/*	Kapitel und Abschnittsnummer
*/

static char *hpost_num(HPOST *hpost)
{
	StrBuf *buf = new_strbuf(0);

	if	(hpost->chap > 0)
		sb_printf(buf, "%d", hpost->chap);

	if	(hpost->sec > 0)
	{
		if	(hpost->chap > 0)
			sb_putc('.', buf);

		sb_printf(buf, "%d", hpost->sec);
	}

	if	(hpost->chap > 0 || hpost->sec > 0)
		sb_putc(' ', buf);

	return sb2str(buf);
}

static void hpost_copy (HPOST *hpost)
{
	int c;

	while ((c = io_getc(hpost->tmp)) != EOF)
	{
		if	(c == '<')
		{
			c = io_getc(hpost->tmp);

			if	(c == '>')
			{
				hpost_next(hpost);
				continue;
			}
			else if	(c == '*')
			{
				hpost_ref(hpost);
				continue;
			}

			io_putc('<', hpost->io);
		}

		io_putc(c, hpost->io);
	}
}

static char *sec_beg[] = { "<H1>", "<H2>" };
static char *sec_end[] = { "</H1>\n", "</H2>\n" };
static char *toc_beg[] = { "<H2>", NULL };
static char *toc_end[] = { "</H2>\n", NULL };

static void hpost_sec (HPOST *hpost, const char *label, int type)
{
	char *p = hpost_num(hpost);

	hpost->anum++;

	io_puts(sec_beg[type], hpost->tmp);
	io_printf(hpost->tmp, "<A NAME=\"A%d\">", hpost->anum);
	io_puts(p, hpost->tmp);
	io_puts(label, hpost->tmp);
	io_puts("</A>", hpost->tmp);
	io_puts(sec_end[type], hpost->tmp);

	sb_puts(toc_beg[type], hpost->toc);
	sb_printf(hpost->toc, "<A HREF=\"%s#A%d\">",
		hpost->fname, hpost->anum);

	sb_puts(p, hpost->toc);
	sb_puts(label, hpost->toc);
	sb_puts("</A>", hpost->toc);
	sb_puts(toc_end[type], hpost->toc);
	sb_puts("<BR>\n", hpost->toc);
	memfree(p);
}

static void hpost_create (HPOST *hpost)
{
	HIDX *idx;
	char *lbl_toc, *lbl_lof, *lbl_index;
	size_t n;

	lbl_toc = "table of contents";
	lbl_lof = "list of figures";
	lbl_index = "index";

	if	(mstrcmp(LangDef.language, "de") == 0)
	{
		lbl_toc = "Inhalt";
		lbl_lof = "Abbildungsverzeichnis";
		lbl_index = "Index";
	}

	io_rewind(hpost->tmp);
	hpost_copy(hpost);
	hpost_close(hpost);
	io_close(hpost->tmp);

	hpost_open(hpost, "toc.html", lbl_toc);
	sb_putc(0, hpost->toc);
	io_puts((char *) hpost->toc->data, hpost->io);
	hpost_close(hpost);

	hpost_open(hpost, "lof.html", lbl_lof);
	io_puts((char *) hpost->lof->data, hpost->io);
	hpost_close(hpost);

	vb_qsort(&hpost->idx, hidx_cmp);
	hpost_open(hpost, "index.html", lbl_index);

	for (idx = hpost->idx.data, n = hpost->idx.used; n-- > 0; idx++)
	{
		io_printf(hpost->io, "<A HREF=\"%s#IDX%d\">",
			idx->fname, idx->num);
		io_puts(idx->label, hpost->io);
		io_puts("</A><BR>\n", hpost->io);
		memfree(idx->fname);
		memfree(idx->name);
		memfree(idx->label);
	}

	hpost_close(hpost);
	vb_free(&hpost->idx);
	vb_free(&hpost->lbl);
}


/*	Kontrollfunktion
*/

static int hpost_ctrl (void *ptr, int req, va_list list)
{
	HPOST *hpost = ptr;

	switch (req)
	{
	case IO_CLOSE:
		hpost_create(hpost);
		del_strbuf(hpost->toc);
		del_strbuf(hpost->lof);
		del_strbuf(hpost->ref);
		memfree(hpost->path);
		memfree(hpost->title);
		memfree(hpost->fname);
		memfree(hpost);
		break;
	case IO_IDENT:
		*va_arg(list, char **) = mstrcpy(hpost->path);
		break;
	case DOC_CMD_APP:
		hpost->app = 1;
		break;
	case DOC_CMD_IDX:
		hpost_idx(hpost, list);
		break;
	case DOC_REF_LABEL:
		hpost_lbl(hpost, list);
		break;
	case DOC_REF_STD:
	case DOC_REF_PAGE:
	case DOC_REF_VAR:
		io_printf(hpost->tmp, "<*%s>", va_arg(list, char *));
		break;
	case HTML_TITLE:
		hpost->title = va_arg(list, char *);
		break;
	case HTML_CHAP:
		if	(hpost->app)
		{
			if	(hpost->chap > 0)
				hpost->chap = -1;
			else	hpost->chap--;
		}
		else	hpost->chap++;

		hpost->sec = 0;
		hpost_split(hpost);
		hpost_sec(hpost, va_arg(list, char *), 0);
		break;
	case HTML_SEC:
		if	(hpost->app)
		{
			if	(hpost->sec > 0)
				hpost->sec = -1;
			else	hpost->sec--;
		}
		else	hpost->sec++;

		hpost_sec(hpost, va_arg(list, char *), 1);
		break;
	default:
		return EOF;
	}

	return 0;
}

static void create_dir (const char *name)
{
	char *p;

	if	(MakeDepend)
		return;

	p = mstrpaste(" ", "mkdir -p", name);
/*	Manche Systeme liefern Fehler, wenn Bibliothek bereits existiert!
	if	(system(p) != 0)
		exit(EXIT_FAILURE);
*/
	system(p);
	memfree(p);
}


/*
Die Funktion |$1| dient zur Ausgabe eines HTML-Dokuments in
mehreren Teildateien. Als Argument wird die Biblothek
für die Teildateien angegeben.
Die Bibliothek <dir> wird bei Bedarf eingerichtet.

$SeeAlso
\mref{diropen(3)}.
*/

IO *html_open (const char *dir, const char *path)
{
	HPOST *par;
	IO *io;

	create_dir(dir);

	par = memalloc(sizeof(HPOST));
	par->tmp = io_bigbuf(BUF_SIZE, TMP_PFX);
	par->path = mstrcpy(dir);
	par->ref = new_strbuf(0);
	par->toc = new_strbuf(0);
	par->lof = new_strbuf(0);
	vb_init(&par->idx, 512, sizeof(HIDX));
	vb_init(&par->lbl, 512, sizeof(HIDX));
	par->fname = mstrcpy("main.html");
	io_printf(par->tmp, "<>%s\n", par->fname);

	io = io_alloc();
	io->put = hpost_put;
	io->ctrl = hpost_ctrl;
	io->data = par;
	return io;
}

