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
	size_t width;	/* Breite */
	size_t height;	/* Height */
	unsigned char *data;	/* Datenvektor */
	void (*fillMode)(unsigned char *data, int color);
} EPixmap;

EPixmap *NewPixmap (size_t width, size_t height, int color);
EPixmap *LoadPixmap (IO *io);
void SavePixmap (EPixmap *pix, IO *io);
EPixmap *Pix_clip(EPixmap *pix, int x0, int y0,
	size_t width, size_t height, int color);

void Pix_gray(EPixmap *pix);
void Pix_bw(EPixmap *pix);
void Pix_grayhist(EPixmap *pix, IO *out);
void Pix_fill(EPixmap *pix, int x0, int y0, int x1, int y1, int color);
void Pix_line(EPixmap *pix, int x0, int y0, int x1, int y1, int w, int color);
void Pix_frame(EPixmap *pix, int x0, int y0, int x1, int y1, int w, int color);

void SetupPixmap (void);

#endif	/* EFEU/Pixmap.h */
