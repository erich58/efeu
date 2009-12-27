/*
Tabellenkonstruktion

$Header <EFEU/$1>

$Copyright (C) 2008 Erich Frühstück
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

#ifndef	EFEU_Tabular_h
#define	EFEU_Tabular_h	1

#include <EFEU/strbuf.h>
#include <EFEU/vecbuf.h>
#include <EFEU/refdata.h>
#include <EFEU/io.h>

typedef enum {
	TabularAlign_left = 0,
	TabularAlign_center = 1,
	TabularAlign_right = 2,
} TabularAlign;

typedef enum {
	TabularLinestyle_none = 0,
	TabularLinestyle_single = 1,
	TabularLinestyle_double = 2,
} TabularLinestyle;

typedef struct {
	TabularLinestyle left_border : 2;
	TabularLinestyle right_border : 2;
	TabularAlign align : 2;
	unsigned left_margin : 8;
	unsigned right_margin : 8;
	unsigned width : 10;
} TabularCol;

typedef struct {
	TabularLinestyle hline : 2;	/* Art der Trennlinie */
	TabularAlign align : 2;		/* Zeilenausrichtung */
	unsigned top_margin : 2;	/* Oberer Rand */
	unsigned height : 10;		/* Zeilenhöhe */
} TabularRow;

typedef struct {
	int lnum;	/* Zeilennummer */
	int cpos;	/* Spaltenposition */
	int cdim;	/* Spaltenzahl */
	int i_text;	/* Textstartposition */
	TabularRow row;	/* Zeilenattribute */
	TabularCol col;	/* Spaltenattribute */
} TabularEntry;

typedef struct {
	REFVAR;		/* Referenzzähler */
	IO *io;		/* Ausgabestruktur */
	int margin;	/* Linker Rand */
	StrBuf text;	/* Textbuffer */
	VecBuf col;	/* Spaltendefinitionen */
	VecBuf row;	/* Zeilendefinitionen */
	VecBuf entry;	/* Tabelleneinträge */
	TabularEntry buf;
} Tabular;

Tabular *NewTabular (const char *coldef, const char *opt);
TabularCol *Tabular_getcol (Tabular *tab, int n);
TabularRow *Tabular_getrow (Tabular *tab, int n);
void Tabular_setcol (Tabular *tab, int n);
void Tabular_setrow (Tabular *tab, int n);
void Tabular_column (Tabular *tab, const char *def);
void Tabular_multicol (Tabular *tab, int cdim, const char *def);
void Tabular_newline (Tabular *tab);
void Tabular_endline (Tabular *tab);
void Tabular_entry (Tabular *tab);
void Tabular_hline (Tabular *tab, int n);
void Tabular_cline (Tabular *tab, int n, int p1, int p2);
void Tabular_print (Tabular *tab, IO *out);
void SetupTabular (void);

#endif	/* EFEU/Tabular.h */
