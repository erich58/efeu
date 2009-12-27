/*
EDB-Metadatenfile

$Header <EFEU/$1>

$Copyright (C) 2004 Erich Frühstück
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

#ifndef	EFEU_EDBMeta_h
#define	EFEU_EDBMeta_h	1

#include <EFEU/EDB.h>

typedef struct {
	IO *base;
	IO *ctrl;
	EDB *prev;
	EDB *cur;
	char *desc;
	char *path;
	StrBuf buf;
} EDBMeta;

void EDBMeta_init (EDBMeta *meta, IO *base, IO *ctrl);
void EDBMeta_clean (EDBMeta *meta);
EDB *EDBMeta_edb (EDBMeta *meta);
char *EDBMeta_par (EDBMeta *meta, int flag);

typedef struct EDBMetaDefStruct EDBMetaDef;

struct EDBMetaDefStruct {
	char *name;
	void (*eval) (EDBMetaDef *def, EDBMeta *meta, const char *arg);
	int flag;
	char *desc;
};

void EDBMeta_type (EDBMetaDef *def, EDBMeta *meta, const char *arg);
void EDBMeta_file (EDBMetaDef *def, EDBMeta *meta, const char *arg);
void EDBMeta_paste (EDBMetaDef *def, EDBMeta *meta, const char *arg);

void AddEDBMetaDef (EDBMetaDef *def, size_t dim);
EDBMetaDef *GetEDBMetaDef (const char *name);
void ListEDBMetaDef (IO *out);
void SetupEDBMeta (void);
void EDBMetaInfo (InfoNode *info);

#endif
