/*
Filter für Terminalausgabe

$Copyright (C) 1995 Erich Frühstück
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

#include <EFEU/object.h>
#include <EFEU/pconfig.h>
#include <EFEU/ioctrl.h>

#define	DEF_TERM	getenv("TERM")

#define	FONT_ROMAN	0
#define	FONT_BOLD	1
#define	FONT_ITALIC	2

#define	VAR_TERM	0
#define	VAR_LINES	1
#define	VAR_COLUMNS	2
#define	VAR_INIT	3
#define	VAR_EXIT	4
#define	VAR_CLEAR	5
#define	VAR_ROMAN	6
#define	VAR_BOLD	7
#define	VAR_ITALIC	8
#define	VAR_ENDBOLD	9
#define	VAR_ENDITALIC	10
#define	VAR_FILTER	11
#define	VAR_DIM		12

#define	SETVAR(x, i, n, t, p)	\
x->var[i].name = n, x->var[i].type = t, x->var[i].data = &x->p, x->var[i].dim = 0


typedef struct {
	io_t *io;		/* Ausgabestruktur */
	int cur_att;		/* Aktuelles Attribut */
	int set_att;		/* Gesetztes Attribut */
	io_put_t filter;	/* Ausgabefilter */
	VarTab_t *vartab;	/* Variablentabelle */
	Var_t *var;		/* Variablendefinitionen */
	int Lines;		/* Zahl der Zeilen */
	int Columns;		/* Zahl der Spalten */
	char *Term;		/* Terminalname */
	char *Init;		/* Initialisierung */
	char *Exit;		/* Aufräumen */
	char *Clear;		/* Bildschirm löschen */
	char *Bold;		/* Steuersequenz für Fettschrift */
	char *Italic;		/* Steuersequenz für Kursiv */
	char *Roman;		/* Steuersequenz für Roman */
	char *EndBold;		/* Steuersequenz für Fettschrift abschalten */
	char *EndItalic;	/* Steuersequenz für Kursiv abschalten */
} TRM;


static int trm_put (int c, TRM *trm);
static int trm_ctrl (TRM *trm, int req, va_list list);
static int set_font (TRM *trm, int font);


io_t *io_termout(io_t *io, const char *term)
{
	TRM *trm;

	if	(io == NULL)	return NULL;

	trm = memalloc(sizeof(TRM));
	memset(trm, 0, sizeof(TRM));
	trm->io = io;
	trm->filter = (io_put_t) io_putc;
	trm->cur_att = trm->set_att = 0;
	io = io_alloc();
	io->put = (io_put_t) trm_put;
	io->ctrl = (io_ctrl_t) trm_ctrl;
	io->data = trm;
	trm->Term = mstrcpy(term ? term : DEF_TERM);

/*	Variablentabelle generieren
*/
	trm->var = ALLOC(VAR_DIM, Var_t);
	memset(trm->var, 0, VAR_DIM * sizeof(Var_t));
	SETVAR(trm, VAR_TERM, "TERM", &Type_str, Term);
	SETVAR(trm, VAR_COLUMNS, "COLUMNS", &Type_int, Columns);
	SETVAR(trm, VAR_LINES, "LINES", &Type_int, Lines);
	SETVAR(trm, VAR_CLEAR, "CLEAR", &Type_str, Clear);
	SETVAR(trm, VAR_INIT, "Init", &Type_str, Init);
	SETVAR(trm, VAR_EXIT, "Exit", &Type_str, Exit);
	SETVAR(trm, VAR_ROMAN, "RomanFont", &Type_str, Roman);
	SETVAR(trm, VAR_BOLD, "BoldFont", &Type_str, Bold);
	SETVAR(trm, VAR_ITALIC, "ItalicFont", &Type_str, Italic);
	SETVAR(trm, VAR_ENDBOLD, "EndBold", &Type_str, EndBold);
	SETVAR(trm, VAR_ENDITALIC, "EndItalic", &Type_str, EndItalic);
	trm->vartab = VarTab("TERM", VAR_DIM);
	AddVar(trm->vartab, trm->var, VAR_DIM);

/*	Werte laden
*/
	PushVarTab(RefVarTab(trm->vartab), NULL);
	applfile(trm->Term, APPL_TRM);
	io_psub(trm->io, trm->Init);
	PopVarTab();
	return io;
}


/*	Zeichen ausgeben
*/

static int trm_put(int c, TRM *trm)
{
	if	(trm->cur_att != trm->set_att)
		set_font(trm, trm->cur_att);

	return io_putc(c, trm->io);
}


/*	Kontrollfunktion
*/

static int trm_ctrl(TRM *trm, int req, va_list list)
{
	int stat;

	switch (req)
	{
	case IO_COLUMNS:	return trm->Columns;
	case IO_LINES:		return trm->Lines;
	case IO_BOLD_FONT:	trm->cur_att = FONT_BOLD; return 0;
	case IO_ITALIC_FONT:	trm->cur_att = FONT_ITALIC; return 0;
	case IO_ROMAN_FONT:	trm->cur_att = FONT_ROMAN; return 0;

	case IO_CLOSE:

		set_font(trm, FONT_ROMAN);
		PushVarTab(trm->vartab, NULL);
		io_psub(trm->io, trm->Exit);
		PopVarTab();
		stat = io_close(trm->io);
		memfree(trm);
		return stat;

	default:

		return io_vctrl(trm->io, req, list);
	}
}

static int set_font(TRM *trm, int font)
{
	char *p;
	int n;

	switch (trm->set_att)
	{
	case FONT_ITALIC:	n = io_puts(trm->EndItalic, trm->io); break;
	case FONT_BOLD:		n = io_puts(trm->EndBold, trm->io); break;
	default:		n = 0;
	}

	switch (font)
	{
	case FONT_ROMAN:	p = trm->Roman; break;
	case FONT_ITALIC:	p = trm->Italic; break;
	case FONT_BOLD:		p = trm->Bold; break;
	default:		p = NULL;
	}

	trm->set_att = font;
	return n + io_puts(p, trm->io);
}
