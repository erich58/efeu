/*
Arbeiten mit Pixeldateien

$Header <EFEU/$1>

$Copyright (C) 2000 Erich Frühstück
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

#ifndef EFEU_Pixmap_h
#define EFEU_Pixmap_h	1

#include <EFEU/refdata.h>
#include <EFEU/io.h>

typedef struct {
	REFVAR;		/* Referenzvariablen */
	size_t rows;	/* Zahl der Zeilen */
	size_t cols;	/* Zahl der Spalten */
	uchar_t *data;	/* Datenvektor */
} Pixmap_t;

extern reftype_t Pixmap_reftype;

Pixmap_t *NewPixmap (size_t rows, size_t cols, int color);

Pixmap_t *LoadPixmap (io_t *io);
void SavePixmap (Pixmap_t *pix, io_t *io);

void Pixmap_vadjust (Pixmap_t *pix, int idx, int offset);
void SetupPixmap (void);

#endif	/* EFEU/Pixmap.h */
