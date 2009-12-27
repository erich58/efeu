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

#include <EFEU/StrData.h>

#if	0
#include <EFEU/io.h>
#define	DEBUG(ctrl, note)	\
	io_xprintf(ioerr, "StrCtrl_%s: %s\n", ctrl ? ctrl->name : "null", note)
#else
#define	DEBUG(ctrl, note)
#endif

StrData *StrData_init (StrData *buf, StrCtrl *ctrl, const char *def)
{
	if	(!buf)	return NULL;

	if	(ctrl)
	{
		buf->ctrl = ctrl;
		ctrl->init(buf, def);
	}
	else if	(def)
	{
		buf->ctrl = &StrCtrl_const;
		buf->ctrl->init(buf, def);
	}
	else
	{
		buf->ctrl = NULL;
		buf->ptr = NULL;
		buf->key = 0;
	}

	DEBUG(buf->ctrl, "init");
	return buf;
}

char *StrData_get (const StrData *sd)
{
	return (sd && sd->ctrl) ? sd->ctrl->get(sd) : NULL;
}

void StrData_set (StrData *sd, const char *s)
{
	if	(!sd)	return;

	if	(sd->ctrl)
	{
		if	(sd->ctrl->set)
		{
			DEBUG(sd->ctrl, "set");
			sd->ctrl->set(sd, s);
			return;
		}
		else if	(sd->ctrl->clean)
		{
			sd->ctrl->clean(sd, 1);
		}
	}

	if	(s)
	{
		sd->ctrl = &StrCtrl_alloc;
		DEBUG(sd->ctrl, "init");
		sd->ctrl->init(sd, s);
	}
	else
	{
		sd->ctrl = NULL;
		sd->ptr = NULL;
		sd->key = 0;
	}
}

void StrData_mset (StrData *sd, char *s)
{
	if	(!sd)	return;

	if	(sd->ctrl)
	{
		if	(sd->ctrl->set)
		{
			DEBUG(sd->ctrl, "set");
			sd->ctrl->set(sd, s);
			memfree(s);
			return;
		}
		else if	(sd->ctrl->clean)
		{
			sd->ctrl->clean(sd, 1);
		}
	}

	if	(s)
	{
		sd->ctrl = &StrCtrl_alloc;
		sd->ptr = s;
		sd->key = 0;
		DEBUG(sd->ctrl, "init");
	}
	else
	{
		sd->ctrl = NULL;
		sd->ptr = NULL;
		sd->key = 0;
	}
}

void StrData_clean (StrData *sd, int destroy)
{
	if	(sd->ctrl && sd->ctrl->clean)
	{
		sd->ctrl->clean(sd, destroy);
	}
	else
	{
		sd->ctrl = NULL;
		sd->ptr = NULL;
		sd->key = 0;
	}
}

void StrData_copy (StrData *tg, const StrData *src)
{
	if	(!tg)	return;

	DEBUG(src->ctrl, "copy:src");
	DEBUG(tg->ctrl, "copy:tg");

	if	(tg->ctrl)
	{
		if	(tg->ctrl->set)
		{
			tg->ctrl->set(tg, StrData_get(src));
			return;
		}
		else if	(tg->ctrl->clean)
		{
			tg->ctrl->clean(tg, 1);
		}
	}

	if	(src->ctrl)
	{
		if	(src->ctrl->copy)
		{
			src->ctrl->copy(tg, src);
		}
		else
		{
			tg->ctrl = &StrCtrl_alloc;
			tg->ctrl->init(tg, StrData_get(src));
		}
	}
	else
	{
		tg->ctrl = NULL;
		tg->ptr = NULL;
		tg->key = 0;
	}
}
