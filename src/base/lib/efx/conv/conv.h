/*
Objektkonvertierung

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

#ifndef	EFEU_conv_h
#define	EFEU_conv_h	1

#include <EFEU/efmain.h>
#include <EFEU/efio.h>
#include <EFEU/object.h>


/*	Distanzen
*/

#define	D_REJECT	-1	/* Konvertierung ablehnen */
#define	D_MATCH		0	/* Übereinstimung */

#define	D_PROMOTE	0x1	/* Geförderte Konvertierung */
#define	D_BASE		0x2	/* Kompatible Basis */
#define	D_CONVERT	0x4	/* Normale Konvertierung */
#define	D_ACCEPT	0x8	/* Akzeptierte Übernahme */
#define	D_VAARG		0x10	/* Variable Argumentzahl */

#define	D_RESTRICTED	0x20	/* Eingeschränkte Konvertierung */
#define	D_CREATE	0x40	/* Konstruktor verwenden */
#define	D_EXPAND	0x80	/* Expansion eines Basisobjekts */

#define	D_MAXDIST	0x100	/* Maximaldistanz */


/*	Funktionsargumente Konvertieren
*/

typedef struct {
	EfiFunc *func;	/* Konvertierungsfunktion */
	EfiType *type;	/* Konvertierungsbasis */
	int dist;	/* Konvertierungsdistanz */
} EfiConv;


EfiConv *GetArgConv (const EfiType *src, const EfiType *tg);
void ArgConv (EfiConv *conv, void *tg, void *src);

EfiObj *ConvObj (const EfiObj *obj, EfiType *def);
EfiObj *LvalCast (EfiObj *obj, EfiType *def);

EfiFunc *SearchFunc (EfiVirFunc *tab, EfiFuncArg *arg,
	size_t narg, EfiConv **conv);

/*	Objekte konvertieren
*/

EfiConv *GetConv (EfiConv *buf, const EfiType *otype, const EfiType *ntype);
void ConvData (EfiConv *conv, void *tg, void *src);
int ConvDist (const EfiType *src, const EfiType *tg);

#endif	/* EFEU/conv_h */
