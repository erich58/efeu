/*
:*:	language filter
:de:	Sprachfilter

$Copyright (C) 2001 Erich Frühstück
This file is part of EFEU.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; see the file COPYING.Library.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <EFEU/LangDef.h>
#include <EFEU/strbuf.h>
#include <EFEU/mstring.h>
#include <EFEU/ioctrl.h>
#include <ctype.h>

#define	LANG_BEG	':'
#define	LANG_SEP	','
#define	LANG_END	':'

#define	LANG_DEF	1
#define	LANG_XDEF	2
#define	LANG_MATCH	3
#define	LANG_ALL	4


/*	:de:Eingabestruktur
*/

typedef struct {
	IO *io;		/* Eingabestruktur */
	int (*newpar) (IO *io, int c, void *par); /* Test auf neuen Absatz */
	void *par;	/* Parameter für Absatztest */
	char *lang;	/* Sprachkennung */
	StrBuf buf;	/* Zeichenbuffer */
	int nsave;	/* Gepufferte Zeichen */
	int nlflag;	/* Flag für neue Zeile */
	int protect;	/* Schutzflag */
} LFPAR;

static int lf_get (IO *io);
static int lf_ctrl (IO *io, int req, va_list list);

/*
:de:
Die Funktion |$1| setzt einen Sprachfilter auf die IO-Struktur <io>.
Dieser filtert alle nicht zur Sprachdefinition <lang> gehörenden Teile
aus dem Eingabestrom.
\par
Der Eingabestrom enthält mehrsprachigen Text, der durch Sprachmarken der Form
|:|<tag>|,|<tag>|:| gekennzeichnet ist.  Die nachfolgenden Zeichen werden nur
kopiert, wenn <lang> mit einem <tag> übereinstimmt.  Sprachmarken haben die
Form <L>|_|<T>, wobei <L> den ISO 639 Sprachcode und <T> den ISO 3166
Ländercode angibt. Der Ländercode ist optional.
\par
Zunächst werden alle Zeichen unverändert kopiert. Mit der ersten Sprachmarke
beginnt die bedingte Verarbeitung. Eine neue Zeile oder die spezielle
Marke |:_:| schließt einen Verarbeitungsblock ab.
\par
Ein Tag der Form |:|<L>|_*:| steht für ein anderes Land der Sprache <L>,
ein Tag der Form |:*:| steht für die Defaultsprache.
Die Defaultmarken können an beliebiger Stelle des bedingten Blocks
stehen.
\par
Beispiel: |:de_AT:Jänner:de_*:Januar:en,*:January:_:|
\par
Ein |:| wird nur interpretiert, wenn ein Stern, ein Unterstreichungszeichen
oder ein Buchstabe nachfolgt.  Zusätzlich kann die Interpretation von |:| mit
einem Gegenschrägstrich aufgehoben werden. Der Gegenschrägstrich bleibt im
Eingabestrom erhalten.  Der Filter berücksichtigt Verbatim-Ausdrücke, die mit
dem Pipe-Symbol begrenzt sind. Innerhalb von zwei Pipe-Symbolen wird |:| nicht
interpretiert.
\par
Steht ein Tag am Zeilenanfang, werden nachfolgende Leerzeichen und Tabulatoren
sowie ein einzelner Zeilenvorschub entfernt.

$SeeAlso
\mref{LangDef(7)}.\br
\mref{getenv(3)},
\mref{setlocale(3)} @PRM.
*/

IO *langfilter (IO *io, const char *lang)
{
	return xlangfilter(io, lang, NULL, NULL);
}

/*
:de:
Die Funktion |$1| ist eine erweiterte Version von |langfilter|, bei der
eine zusätzliche Abgrenzung der Verarbeitungsblöcke mithilfe einer Testfunktion
bestimmt wird. Die Testfunktion wird zu Beginn jeder Zeile mit dem ersten
Zeichen der Zeile aufgerufen. Falls sie einen Wert ungleich 0 liefert,
gilt der Verarbeitungsblock als abgeschlossen. Die Testfunktion kann mithilfe
von <par> parametrisiert werden.
*/

IO *xlangfilter (IO *io, const char *lang,
	int (*isnewpar) (IO *io, int c, void *par), void *tpar)
{
	if	(io != NULL)
	{
		LFPAR *par = memalloc(sizeof(LFPAR));
		par->io = io;

		if	(lang == NULL)
		{
			par->lang = mstrpaste("_", LangDef.language,
				LangDef.territory);
		}
		else	par->lang = mstrcpy(lang);
	
		if	(par->lang == NULL)
			par->lang = memalloc(1);

		sb_init(&par->buf, 0);
		par->newpar = isnewpar;
		par->par = tpar;
		par->nsave = 0;
		par->nlflag = 1;
		io = io_alloc();
		io->get = lf_get;
		io->ctrl = lf_ctrl;
		io->data = par;
	}

	return io;
}

/*	Hilfsfunktionen
*/

static int lf_skey (const char *lang, const char *key)
{
	if	(*key == 0)	return LANG_ALL;
	if	(*key == '_')	return LANG_ALL;
	if	(*key == '*')	return LANG_DEF;

	while (*lang || *key)
	{
		if	(*lang == '_')
		{
			if	(*key == 0)	return LANG_MATCH;
			if	(*lang != *key)			return 0;

			lang++;
			key++;

			if	(*key == 0 || *key == '*')
				return LANG_XDEF;
		}

		if	(*lang != *key)
		{
			if	(*lang == 0 && key[0] == '_' && key[1] == '*')
				return LANG_MATCH;

			return 0;
		}

		lang++;
		key++;
	}

	return LANG_MATCH;
}


static int lf_key (LFPAR *lf)
{
	char *key;
	int c, pos, stype, type;

	pos = sb_getpos(&lf->buf);
	type = 0;

	while ((c = io_getc(lf->io)) != EOF)
	{
		switch (c)
		{
		case LANG_SEP:
		case LANG_END:
			sb_putc(0, &lf->buf);
			key = (char *) lf->buf.data + pos;
			stype = lf_skey(lf->lang, key);

			if	(type < stype)
				type = stype;

			sb_setpos(&lf->buf, pos);
			break;
		default:
			sb_putc(c, &lf->buf);
			break;
		}

		if	(c == LANG_END)	break;
	}

	if	(lf->nlflag)
	{
		do	c = io_getc(lf->io);
		while	(c == ' ' || c == '\t');

		if	(c != '\n')
		{
			io_ungetc(c, lf->io);
			lf->nlflag = 0;
		}
	}

	return type;
}

static int lf_test (IO *io)
{
	int c = io_peek(io);
	return (c != '*' && c != '_' && !isalpha(c));
}

static int lf_fill (LFPAR *lf)
{
	int save_pos;
	int mode, stat;
	int vstat;
	int c;

	sb_trunc(&lf->buf);
	save_pos = 0;
	mode = 2;
	stat = 0;
	vstat = 0;

	while ((c = io_getc(lf->io)) != EOF)
	{
		if	(lf->protect)
		{
			sb_putc(c, &lf->buf);

			if	(c == '\n')
			{
				lf->nlflag = (c == '\n');
				break;
			}

			continue;
		}

		if	(lf->nlflag && lf->newpar &&
				lf->newpar(lf->io, c, lf->par))
		{
			if	(sb_getpos(&lf->buf))
			{
				io_ungetc(c, lf->io);
				break;
			}
		}

		if	(c == '|')
		{
			vstat = !vstat;
		}
		else if	(vstat)
		{
			;
		}
		else if	(c == '\\')
		{
			c = io_getc(lf->io);

			if	(mode && c != LANG_BEG)
				sb_putc('\\', &lf->buf);

			if	(c == EOF)	break;
		}
		else if	(c == LANG_BEG && lf_test(lf->io))
		{
			if	(mode)
				sb_putc(c, &lf->buf);

			c = io_getc(lf->io);

			if	(c == EOF)	break;
		}
		else if	(c == LANG_BEG)
		{
			if	(mode == 2)
				save_pos = sb_getpos(&lf->buf);

			switch (lf_key(lf))
			{
			case LANG_MATCH:
				if	(stat == LANG_DEF || stat == LANG_XDEF)
					sb_setpos(&lf->buf, save_pos);

				stat = LANG_MATCH;
				mode = 2;
				break;
			case LANG_ALL:
				save_pos = sb_getpos(&lf->buf);

				if	(save_pos)
					return save_pos;

				stat = 0;
				mode = 2;
				break;
			case LANG_XDEF:
			case LANG_DEF:
				if	(stat != LANG_MATCH)
				{
					save_pos = sb_getpos(&lf->buf);
					mode = 1;
					stat = LANG_DEF;
				}
				else	mode = 0;

				break;
			default:
				mode = 0;
				break;
			}

			continue;
		}
		else if	(lf->nlflag && c == '\n')
		{
			sb_putc(c, &lf->buf);
			break;
		}

		if	(mode)
			sb_putc(c, &lf->buf);

		lf->nlflag = (c == '\n');
	}

	return sb_getpos(&lf->buf);
}

/*	Zeichen lesen
*/

static int lf_get (IO *io)
{
	LFPAR *lf = io->data;

	if	(lf->nsave == 0)
	{
		lf->nsave = lf_fill(lf);

		if	(lf->nsave == 0)	return EOF;

		sb_begin(&lf->buf);
	}

	lf->nsave--;
	return sb_getc(&lf->buf);
}


/*	Kontrollfunktion
*/

static int lf_ctrl (IO *io, int req, va_list list)
{
	LFPAR *lf = io->data;
	int stat;

	switch (req)
	{
	case IO_CLOSE:

		stat = io_close(lf->io);
		memfree(lf->lang);
		sb_free(&lf->buf);
		memfree(lf);
		return stat;

	case IO_REWIND:

		return EOF;
		
	case IO_IDENT:

		*va_arg(list, char **) = io_xident(lf->io,
			"[%s]%*", lf->lang);
		return 0;

	case IO_PROTECT:

		stat = va_arg(list, int);

		if	(lf->io)
			io_protect(lf->io, stat);

		if	(stat)		lf->protect++;
		else if	(lf->protect)	lf->protect--;

		return lf->protect;

	default:

		return io_vctrl(lf->io, req, list);
	}
}
