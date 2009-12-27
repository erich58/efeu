/*
EDB-Datenbankverknüpfung

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

#include <EFEU/EDB.h>
#include <EFEU/nkt.h>
#include <EFEU/Resource.h>
#include <EFEU/EDBJoin.h>

#define	INFO_NAME	"join"

#define	INFO_HEAD	\
	":*:data base join definitions" \
	":de:Datenbankverknüpfungen"

static VECBUF(join_buf, 128, sizeof(EDBJoin *));

static int join_cmp (const void *a, const void *b)
{
	EDBJoin **ka = (EDBJoin **) a;
	EDBJoin **kb = (EDBJoin **) b;

	if	((*ka)->t1 < (*kb)->t1)	return -1;
	if	((*ka)->t1 > (*kb)->t1)	return 1;
	if	((*ka)->t2 < (*kb)->t2)	return -1;
	if	((*ka)->t2 > (*kb)->t2)	return 1;

	return 0;
}

void AddEDBJoin (EDBJoin *def)
{
	vb_search(&join_buf, &def, join_cmp, VB_REPLACE);
}

EDBJoin *GetEDBJoin (EfiType *t1, EfiType *t2)
{
	EDBJoin key, *kp, **rp;
	key.t1 = t1;
	key.t2 = t2;
	kp = &key;
	rp = vb_search(&join_buf, &kp, join_cmp, VB_SEARCH);
	return rp ? *rp : NULL;
}

static void join_info (IO *out, InfoNode *info)
{
	io_printf(out, "JOIN\n");
}


void EDBJoinInfo (InfoNode *info)
{
	static int setup_done = 0;

	if	(setup_done)	return;

	setup_done = 1;
	info = AddInfo(info, INFO_NAME, INFO_HEAD, join_info, NULL);
}
