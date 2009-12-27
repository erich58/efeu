/*
Hilfsprogramme zur TeX-Aufbereitung

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

#include <EFEU/efmain.h>
#include <EFEU/efio.h>
#include <EFEU/mdmat.h>

#ifndef	_EFEU_texutil_h
#define	_EFEU_texutil_h	1

typedef struct {
	int size;	/* Basisgröße */
	char *name;	/* Schriftname */
	double width;	/* Ziffernbreite in mm */
	double height;	/* Zeilenhöhe in mm */
} TeXfont_t;


TeXfont_t *TeXfont (int psize);

/*
void TeXfontinit (void);
void TeXfontwalk (int (*visit) (TeXfont_t *font));
*/
void *TeXfontlist (Func_t *func, void *data, void **arg);

typedef struct {
	char *name;	/* Papiername */
	double margin;	/* Linker Rand in mm */
	double topskip;	/* Oberer Rand in mm */
	double width;	/* Papierbreite in mm */
	double height;	/* Papierhöhe in mm */
} TeXpgfmt_t;


TeXpgfmt_t *TeXpgfmt (const char *name);
void *TeXpglist (Func_t *func, void *data, void **arg);

void TeXbegin (io_t *io, TeXpgfmt_t *pgfmt);
void TeXend (io_t *io);
extern int TeXcompletion;

#endif	/* EFEU/texutil.h */
