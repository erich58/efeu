/*
:*:time range for counting
:de:Zeitbereich für Zählungen

$Copyright (C) 2006 Erich Frühstück
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
A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include <EFEU/mdcount.h>
#include <EFEU/parsearg.h>

#define W_OPT "Unbekannte Zeitachsensoption \"$1\" wurde ignoriert.\n"

static VECBUF(buf_time, 32, sizeof(TimeRange));

static int t_time (MdClass *sel, const void *ptr)
{
	MdPtrClass *p = (MdPtrClass *) sel;
	return ((int *) p->ptr)[0];
}

static MdPtrClass s_time = {
	"time",		/* name */
	"Zeitachse",	/* desc */
	0,		/* dim */
	NULL,		/* label */
	t_time,		/* classify */
	NULL,		/* pointer */
};

void MdCountPar_range (MdCountPar *par, const char *def)
{
	int type;
	int i;
	char *buf, *opt, *p;
	char *label;
	AssignArg *x;
	mdaxis *axis;
	int noaxis;

	if	(!par || !def)
		return;

	buf = NULL;

	if	(*def == '[')
	{
		int n;

		for (n = 0, def++; *def; n++)
		{
			if	(def[n] == ']')
			{
				buf = mstrncpy(def, n);
				def += n + 1;
				break;
			}
		}
	}

	noaxis = 0;
	label = NULL;

	for (opt = buf; (x = assignarg(opt, &p, " \t\n,")); opt = p)
	{
		if	(mstrcmp(x->name, "name") == 0)
		{
			label = mstrcpy(x->arg);
		}
		else if	(mstrcmp(x->name, "noaxis") == 0)
		{
			noaxis = 1;
		}
		else	dbg_note(NULL, W_OPT, "s", x->name);

		memfree(x);
	}

	memfree(buf);

	buf_time.used = 0;
	type = strTimeRange(&buf_time, def);
	par->time_dim = buf_time.used;
	par->time_range = buf_time.data;
	par->time_idx = 0;

	if	(!par->time_dim || noaxis)
	{
		memfree(label);
		return;
	}

	switch (type)
	{
	case 'j':	s_time.name = "jahr"; break;
	case 'm':	s_time.name = "monat"; break;
	default:	break;
	}

	if	(label)
		s_time.name = label;

/*	Zeitzähler generieren
*/
	s_time.dim = par->time_dim;
	s_time.ptr = &par->time_idx;
	s_time.label = memalloc(par->time_dim * sizeof s_time.label[0]);

	for (i = 0; i < par->time_dim; i++)
	{
		s_time.label[i].name = par->time_range[i].label;
		s_time.label[i].desc = NULL;
	}

	axis = md_classaxis(s_time.name, &s_time, NULL);
	axis->next = par->axis;
	par->axis = axis;
}

