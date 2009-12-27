/*
:*:string data carrier
:de:Träger für Zeichenketten

$Header	<EFEU/$1>

$Copyright (C) 2007 Erich Frühstück
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

#ifndef	EFEU_StrData_h
#define	EFEU_StrData_h	1

#include <EFEU/memalloc.h>

typedef struct StrCtrl StrCtrl;
typedef struct StrData StrData;

struct StrCtrl {
	const char *name;	/* Parametername */
	const char *label;	/* Parameterbezeichnung */
	void (*init) (StrData *data, const char *str);
	void (*clean) (StrData *data, int destroy);
	char *(*get) (const StrData *data);
	void (*set) (StrData *data, const char *str);
	void (*copy) (StrData *tg, const StrData *src);
};

struct StrData {
	StrCtrl *ctrl;
	void *ptr;
	size_t key;
};

StrData *StrData_init (StrData *data, StrCtrl *ctrl, const char *def);
char *StrData_get (const StrData *data);
void StrData_set (StrData *data, const char *s);
void StrData_mset (StrData *data, char *s);
void StrData_clean (StrData *data, int destroy);
void StrData_copy (StrData *tg, const StrData *src);

void StrData_strbuf (StrData *data);

extern StrCtrl StrCtrl_const;
extern StrCtrl StrCtrl_alloc;
extern StrCtrl StrCtrl_strbuf;
extern StrCtrl StrCtrl_pool;

#endif	/* EFEU/StrData.h */
