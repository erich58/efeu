/*
Bilderverwaltung

$Header <EFEU/$1>

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

#ifndef	EFEU_oldpixmap_h
#define	EFEU_oldpixmap_h	1

#include <EFEU/efmain.h>
#include <EFEU/object.h>
#include <EFEU/refdata.h>

#define	MSG_PIXMAP	"pixmap"
#define	PIX_CDIM	0xFF	/* Zahl der Farben */

typedef struct {
#if	REVBYTEORDER
	unsigned char blue;
	unsigned char green;
	unsigned char red;
	unsigned char idx;
#else
	unsigned char idx;
	unsigned char red;
	unsigned char green;
	unsigned char blue;
#endif
} COLOR;

int ColorValue(double x);
COLOR SetColor(double x, double y, double z);
int GetColor(COLOR color, COLOR *tab, size_t dim);

typedef struct {
	REFVAR;		/* Referenzzähler */
	int colors;	/* Farbenzahl */
	int rows;	/* Zeilenzahl */
	int cols;	/* Spaltenzahl */
	COLOR color[PIX_CDIM];	/* Farbvektor */
	unsigned char *pixel;	/* Punktefeld */
} OldPixMap;

extern RefType OldPixMap_reftype;

OldPixMap *new_OldPixMap (int rows, int cols);

#define	refer_OldPixMap(pix)	rd_refer(pix)
#define	deref_OldPixMap(pix)	rd_deref(pix)

OldPixMap *load_OldPixMap (const char *name);
void save_OldPixMap (OldPixMap *pix, const char *name);
void print_OldPixMap (OldPixMap *pix, FILE *file);

OldPixMap *read_PPMFile (FILE *file);
void OldPixMapToPPM (OldPixMap *pix, const char *name);
void OldPixMapToXPM2 (OldPixMap *pix, const char *name);


/*	Schnittstelle zu Befehlsinterpreter
*/

extern EfiType Type_OldPixMap;
extern EfiType Type_Color;

extern COLOR Buf_Color;

#define	Obj_OldPixMap(x)	NewPtrObj(&Type_OldPixMap, (x))
#define	Obj_Color(x)	NewObj(&Type_Color, (Buf_Color = (x), &Buf_Color))

#define	Val_OldPixMap(x)	((OldPixMap **) x)[0]
#define	Val_Color(x)	((COLOR *) x)[0]


void OldPixMap_Histogramm (EfiFunc *func, void *rval, void **arg);
void OldPixMap_Slice (EfiFunc *func, void *rval, void **arg);
void OldPixMap_Dither (EfiFunc *func, void *rval, void **arg);
void OldPixMap_TeX (EfiFunc *func, void *rval, void **arg);


/*	Initialisierung
*/

void SetupOldPixMap (void);

#endif /* EFEU/oldpixmap.h */

