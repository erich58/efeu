/*
:*:locale date for efeu
:de:Sprachanpassung

$Header	<EFEU/$1>

$Copyright (C) 1997 Erich Frühstück
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

#ifndef	EFEU_locale_h
#define	EFEU_locale_h	1

#include <EFEU/config.h>

typedef struct {
	char *name;
	char *thousands_sep;
	char *decimal_point;
	char *negative_sign;
	char *positive_sign;
	char *zero_sign;
} LCValueDef;

extern LCValueDef LCValue[];

typedef struct {
	char *name;
	char **month;
	char **weekday;
} LCDateDef;

extern LCDateDef LCDate[];

typedef struct {
	LCValueDef *scan;
	LCValueDef *print;
	LCDateDef *date;
} LocaleDef;

extern LocaleDef Locale;

void PushLocale (void);
void PopLocale (void);

#define	LOC_SCAN	0x1
#define	LOC_PRINT	0x2
#define	LOC_DATE	0x4
#define	LOC_ALL		0xff

void SetLocale (unsigned flags, const char *name);

#endif	/* EFEU/locale.h */
