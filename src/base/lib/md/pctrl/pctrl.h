/*
Ausgabekontrolle

$Header <EFEU/$1>

$Copyright (C) 1994 Erich Frühstück
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

#ifndef	EFEU_pctrl_h
#define	EFEU_pctrl_h	1

#include <EFEU/efmain.h>
#include <EFEU/efio.h>
#include <EFEU/object.h>
#include <EFEU/konvobj.h>

/*	Ausgabefilter und Druckerkontrolle
*/

typedef struct PCTRLStruct PCTRL;

struct PCTRLStruct {
	IO *io;		/* Eingabestruktur */
	int col;		/* Spalte */
	int line;		/* Zeile */
	int page;		/* Seite */
	const char *delim;	/* Trennzeichen */
	const char *nl;		/* Zeilenvorschub */
	const char *nf;		/* Seitenvorschub */
	int (*put) (int c, PCTRL *pf);
	int (*ctrl) (PCTRL *io, int cmd, va_list list);
};


#define	PCTRL_MASK	('p' << 8)	/* Maske für Steuerflags */

#define	PCTRL_BEGIN	(PCTRL_MASK|0x01)	/* Beginn der Ausgabe */
#define	PCTRL_DATA	(PCTRL_MASK|0x02)	/* Start der Datensektion */
#define	PCTRL_HEAD	(PCTRL_MASK|0x03)	/* Start des Datenheaders */
#define	PCTRL_LINE	(PCTRL_MASK|0x04)	/* Neue Zeile */
#define	PCTRL_PAGE	(PCTRL_MASK|0x05)	/* Neue Seite */
#define	PCTRL_EHEAD	(PCTRL_MASK|0x06)	/* Ende des Datenheaders */
#define	PCTRL_EDATA	(PCTRL_MASK|0x07)	/* Ende der Datensektion */
#define	PCTRL_END	(PCTRL_MASK|0x08)	/* Aufräumen */

#define	PCTRL_FMASK	(0x10)			/* Maske für Wertefelder */

#define	PCTRL_LEFT	(PCTRL_MASK|0x11)	/* Linksbündiger String */
#define	PCTRL_CENTER	(PCTRL_MASK|0x12)	/* Zentrierter String */
#define	PCTRL_RIGHT	(PCTRL_MASK|0x13)	/* Rechtsbündiger String */
#define	PCTRL_VALUE	(PCTRL_MASK|0x14)	/* Zahlenwert ausgeben */
#define	PCTRL_EMPTY	(PCTRL_MASK|0x15)	/* Leerfeld ausgeben */

#define	PCTRL_COLUMNS	(PCTRL_MASK|0x21)	/* Verfügbare Spalten bestimmen */
#define	PCTRL_LINES	(PCTRL_MASK|0x22)	/* Verfügbare Zeilen bestimmen */
#define	PCTRL_XPOS	(PCTRL_MASK|0x23)	/* Aktuelle Spalte bestimmen */
#define	PCTRL_YPOS	(PCTRL_MASK|0x24)	/* Aktuelle Zeile bestimmen */


IO *io_pctrl (IO *io, const char *name);


/*	Spezifische Ausgabe und Steuerprogramme
*/

int std_put (int c, PCTRL *pf);
int tst_put (int c, PCTRL *pf);
int val_put (int c, PCTRL *pf);
int csv_put (int c, PCTRL *pf);
int str_put (int c, PCTRL *pf);
int tex_put (int c, PCTRL *pf);
int sc_put (int c, PCTRL *pf);

int std_ctrl (PCTRL *pf, int cmd, va_list list);
int csv_ctrl (PCTRL *pf, int cmd, va_list list);
int struct_ctrl (PCTRL *pf, int cmd, va_list list);
int data_ctrl (PCTRL *pf, int cmd, va_list list);
int tex_ctrl (PCTRL *pf, int cmd, va_list list);
int tab_ctrl (PCTRL *pf, int cmd, va_list list);
int sc_ctrl (PCTRL *pf, int cmd, va_list list);
int tst_ctrl (PCTRL *pf, int cmd, va_list list);


/*	Globale Anpassungsvariablen
*/

extern int pctrl_fsize;		/* Fontgröße für die Ausgabe */
extern char *pctrl_pgfmt;	/* Seitenformat für die Ausgabe */
extern char *PrintListDelim;	/* Trennzeichen zwischen Datenwerten */

#endif	/* EFEU/pctrl.h */
