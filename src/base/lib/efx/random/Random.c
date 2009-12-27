/*
Implementation des Zufallszahlengenerator

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

#include <EFEU/Random.h>
#include <EFEU/strbuf.h>
#include <EFEU/mstring.h>

static char *rident (const void *data)
{
	const Random *rd = data;

	if	(rd->type)
	{
		StrBuf *sb = sb_create(0);
		sb_puts(rd->type->name, sb);
		sb_putc(' ', sb);

		if	(rd->type->ident)
		{
			IO *io = io_strbuf(sb);
			rd->type->ident(io, rd->data);
			io_close(io);
		}
		
		return sb2str(sb);
	}

	return NULL;
}

static void rclean (void *data)
{
	Random *tg = data;

	if	(tg->type->clean)
		tg->type->clean(tg->data);

	memfree(tg);
}

static RefType Random_reftype = REFTYPE_INIT("Random", rident, rclean);


/*	Initialisierungsfunktion
*/

Random *NewRandom (RandomType *type, unsigned int sval)
{
	Random *rand = memalloc(sizeof(Random));

	if	(type == NULL)	type = &RandomType_std;

	rand->type = type;
	rand->data = NULL;

	if	(type->init)
		rand->data = type->init(sval);
	else if	(type->seed)
		type->seed(rand->data, sval);

	return rd_init(&Random_reftype, rand);
}

Random *str2Random (const char *arg, char **ptr)
{
	RandomType *type = NULL;
	unsigned int sval = 1;

	if	(arg)
	{
		if	(*arg == ':')
		{
			char *p = (char *) ++arg;

			while (*arg != 0 && *arg != ':')
				arg++;

			p = mstrncpy(p, arg - p);
			type = GetRandomType(p);
			memfree(p);

			if	(*arg == ':')	arg++;
		}

		sval = strtoul(arg, ptr, 0);
	}
	else if	(ptr)
	{
		*ptr = NULL;
	}

	return NewRandom(type, sval);
}


/*	Zufallsgenerator kopieren
*/

Random *CopyRandom (Random *src)
{
	if	(src && src->type->copy)
	{
		Random *tg = memalloc(sizeof(Random));
		tg->type = src->type;
		tg->data = src->type->copy(src->data);
		return rd_init(&Random_reftype, tg);
	}

	return NULL;
}


/*	Zufallszahlengenerator initialisieren
*/

static unsigned int state = 1;

void SeedRandom (Random *rd, unsigned int sval)
{
	if	(rd && rd->type)
		rd->type->seed(rd->data, sval);
	else	state = sval ? sval : 0;
}


/*	Zufallszahl ermitteln
*/

double UniformRandom (Random *rd)
{
	if	(rd && rd->type)
		return rd->type->rand(rd->data);

      	state = ((state * 1103515245) + 12345) & 0x7fffffff;
	return state / 2147483648.;
}

#if	0
/*	Zugriff auf random(3) deaktivieren
*/

void srandom (unsigned int sval)
{
	static int warn_flag = 1;

	if	(warn_flag)
	{
		fprintf(stderr, "ACHTUNG: srandom() wurde aufgerufen!\n");
		warn_flag = 0;
	}

	state = sval ? sval : 0;
}

int random (void)
{
	static int warn_flag = 1;

	if	(warn_flag)
	{
		fprintf(stderr, "ACHTUNG: random() wurde aufgerufen!\n");
		warn_flag = 0;
	}

      	state = ((state * 1103515245) + 12345) & 0x7fffffff;
	return state;
}
#endif
