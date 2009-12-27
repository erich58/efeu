/*
Datenbankhilfsprogramme

$Header	<EFEU/$1>

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

#ifndef EFEU_DBUTIL_H
#define EFEU_DBUTIL_H	1

#include <EFEU/io.h>

#define	MSG_DB		"db"


/*	EBCDIC-Konvertierungshilfsprogramme
*/

int db_isblank (const uchar_t *buf, int pos, int len);
int db_iskey (const uchar_t *buf, int key, int pos, int len);
unsigned db_xcval (const uchar_t *buf, int pos, int len);
unsigned db_cval (const uchar_t *buf, int pos, int len);
unsigned db_pval (const uchar_t *buf, int pos, int len);
unsigned db_xval (const uchar_t *buf, int pos, int len);
unsigned db_bcdval (const uchar_t *buf, int pos, int len);
unsigned db_a37l (const uchar_t *buf, int pos, int len);
unsigned db_char (const uchar_t *buf, int pos, int len);
char *db_str (const uchar_t *buf, int pos, int len);
double db_double (const uchar_t *buf, int pos, int len);


/*	ASCII-Konvertierungshilfsprogramme
*/

char *txt_load (io_t *io, char *buf, int recl);
unsigned txt_unsigned (const char *buf, int pos, int len);
unsigned txt_base37 (const char *buf, int pos, int len);
char *txt_str (const char *buf, int pos, int len);
char txt_char (const char *buf, int pos, int len);


/*	Zahlendarstellung zur Basis 37
*/

unsigned a37l (const char *s);
char *l37a (unsigned x);

#define	DIG37_SPACE	0
#define	DIG37_0		1
#define	DIG37_1		2
#define	DIG37_2		3
#define	DIG37_3		4
#define	DIG37_4		5
#define	DIG37_5		6
#define	DIG37_6		7
#define	DIG37_7		8
#define	DIG37_8		9
#define	DIG37_9		10
#define	DIG37_A		11
#define	DIG37_B		12
#define	DIG37_C		13
#define	DIG37_D		14
#define	DIG37_E		15
#define	DIG37_F		16
#define	DIG37_G		17
#define	DIG37_H		18
#define	DIG37_I		19
#define	DIG37_J		20
#define	DIG37_K		21
#define	DIG37_L		22
#define	DIG37_M		23
#define	DIG37_N		24
#define	DIG37_O		25
#define	DIG37_P		26
#define	DIG37_Q		27
#define	DIG37_R		28
#define	DIG37_S		29
#define	DIG37_T		30
#define	DIG37_U		31
#define	DIG37_V		32
#define	DIG37_W		33
#define	DIG37_X		34
#define	DIG37_Y		35
#define	DIG37_Z		36

int ebcdic2ascii (int x);

#endif	/* EFEU/dbutil.h */
