/*
Tageszeitklassifikation

$Copyright (C) 2009 Erich Frühstück
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

#include <EFEU/object.h>
#include <EFEU/cmdsetup.h>
#include <EFEU/calendar.h>
#include <EFEU/EfiClass.h>
#include <limits.h>
#include <time.h>

/*	Tageszeitklassifikation
*/

typedef struct {
	REFVAR;
	int dim;
	int unit;
	int (*get_sec) (const EfiObj *base);
} DayTimePar;

static void DayTime_free (void *data)
{
	memfree(data);
}

static RefType DayTime_reftype = REFTYPE_INIT("DayTime", NULL, DayTime_free);

static int DayTime_update (const EfiObj *base, void *par)
{
	if	(par)
	{
		DayTimePar *dt = par;
		int sec = dt->get_sec(base);
		return (sec / dt->unit) % dt->dim + 1;
	}
	else	return 1;
}

static char *sb_time (StrBuf *sb, const char *fmt, int t0, int t1)
{
	sb_trunc(sb);

	while (*fmt != 0)
	{
		if	(fmt[0] == '%' && fmt[1])
		{
			fmt++;

			switch (*fmt)
			{
			case 'h': sb_printf(sb, "%02d", t0 / 3600); break;
			case 'H': sb_printf(sb, "%02d", t1 / 3600); break;
			case 'm': sb_printf(sb, "%02d", (t0 / 60) % 60); break;
			case 'M': sb_printf(sb, "%02d", (t1 / 60) % 60); break;
			case 's': sb_printf(sb, "%02d", t0 % 60); break;
			case 'S': sb_printf(sb, "%02d", t1 % 60); break;
			default: sb_putc(*fmt, sb); break;
			}
		}
		else	sb_putc(*fmt, sb);

		fmt++;
	}

	return sb_strcpy(sb);
}

#define	DESC	"daytime %h:%m:%s - %H:%M:%S"

void DayTime (EfiClassArg *def, const EfiType *base_type,
		const char *opt, const char *arg, void *get)
{
	StrBuf *sb;
	DayTimePar *par;
	EfiType *type;
	const char *fmt;
	char *p;
	int unit;
	int range;
	int n;
	int t;

	unit = str2sec(arg, &p);
	range = (*p == ',') ? str2sec(p + 1, &p) : 24 * 60 * 60;

	if	(!unit)	unit = 1;
	if	(range < unit)	range = unit;

	par = memalloc(sizeof *par);
	par->unit = unit;
	par->dim = (range + unit - 1) / unit;
	par->get_sec = get;

	if	(opt)			fmt = opt;
	else if	(unit % 3600 == 0)	fmt = "%h";
	else if	(unit % 60 == 0)	fmt = "%h:%m";
	else				fmt = "%h:%m:%s";

	type = NewEnumType(NULL, 1);
	sb = sb_acquire();

	for (t = 0, n = 1; n <= par->dim; n++, t += unit)
	{
		int t1 = t + unit - 1;
		char *name = sb_time(sb, fmt, t, t1);
		AddEnumKey(type, name, sb_time(sb, DESC, t, t1), n);
	}

	sb_release(sb);
	def->type = AddEnumType(type);
	def->update = DayTime_update;
	def->par = rd_init(&DayTime_reftype, par);
}
