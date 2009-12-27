/*
EDB - Datenbanken vergleichen

$Copyright (C) 2007 Erich Frühstück
This file is part of EFEU.

EFEU is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

EFEU is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public
License along with EFEU; see the file COPYING.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <EFEU/pconfig.h>
#include <EFEU/efutil.h>
#include <EFEU/printobj.h>
#include <EFEU/Random.h>
#include <EFEU/EDB.h>
#include <EFEU/mdmat.h>
#include <EFEU/preproc.h>
#include <Math/TimeSeries.h>
#include <Math/pnom.h>
#include <Math/mdmath.h>
#include <Math/func.h>
#include <EFEU/StatData.h>

#define	DAT_BLKSIZE	1024
#define	ALIGN(x, y)	((y) * (((x) + (y) - 1) / (y)))

#define	STR(name)	GetResource(name, NULL)
#define	INT(name)	GetIntResource(name, 0)

char *Output = NULL;	/* Ausgabefile */

int maxrec = 0;		/* Maximale Ausgabezahl */
int maxerr = 0;		/* Maximale Fehlerzahl */

#define	CMP_EQ	1	/* Gleiche Daten */
#define	CMP_NE	2	/* Verschiedene Daten */
#define	CMP_LT	3	/* Vorangehende Daten */
#define	CMP_GT	4	/* Nachfolgende Daten */

#if	0
static EfiType *dbtype = NULL;
static CmpDef *dbcmp = NULL;
static StrBuf *datbuf[2];
#endif

typedef struct {
	const char *name;
	EDB *edb;
	char *id;
	StrBuf buf;
	int pos;
} DATA;

static int cmp_buf (StrBuf *a, StrBuf *b)
{
	if	(a->pos != b->pos)
		return 1;

	if	(memcmp(a->data, b->data, a->pos))
		return 1;

	return 0;
}


/*	Darstellungsfunktionen
*/

typedef void (*DFunc) (IO *aus, DATA *a, DATA *b, const char *fmt);

static void put_head (IO *aus, DATA *d, const char *mark, const char *fmt)
{
	io_xprintf(aus, "%s ", mark);
	io_xprintf(aus, fmt, d->pos);
	io_xprintf(aus, " %s\n", mark);
}

static void put_mark (IO *aus, DATA *d, const char *mark)
{
	int i, last;

	for (last = '\n', i = 0; i < d->buf.pos; i++)
	{
		if	(last == '\n')
			io_puts(mark, aus);

		last = io_putc(d->buf.data[i], aus);
	}

	if	(last != '\n')
		io_putc('\n', aus);
}

static void put_std (IO *aus, DATA *d, const char *mark, const char *fmt)
{
	put_head(aus, d, mark, fmt);
	put_mark(aus, d, NULL);
}

static void df_std (IO *aus, DATA *a, DATA *b, const char *fmt)
{
	if	(a && b)
	{
		put_std(aus, a, "***", fmt ? fmt : (a->pos ? "%d" : "EOF"));
		put_std(aus, b, "---", fmt ? fmt : (b->pos ? "%d" : "EOF"));
	}
	else if	(a)
	{
		put_std(aus, a, "***", fmt ? fmt : "-%d");
	}
	else if	(b)
	{
		put_std(aus, b, "---", fmt ? fmt : "+%d");
	}
}

static void buf_write (const char *name, StrBuf *buf)
{
	IO *io = io_fileopen(name, "w");
	io_write(io, buf->data, buf->pos);
	io_close(io);
}

static void df_diff (IO *aus, DATA *a, DATA *b, const char *fmt)
{
	if	(a && b)
	{
		char *tmp1, *tmp2;
		char *cmd;
		IO *io;
		int n, c;
		int last;

		tmp1 = newtemp(NULL, NULL);
		tmp2 = newtemp(NULL, NULL);

		put_head(aus, a, "***", fmt ? fmt : (a->pos ? "%d" : "EOF"));
		put_head(aus, b, "---", fmt ? fmt : (b->pos ? "%d" : "EOF"));
		buf_write(tmp1, &a->buf);
		buf_write(tmp2, &b->buf);
		cmd = msprintf("diff -c1 %s %s", tmp1, tmp2);
		io = io_popen(cmd, "r");
		memfree(cmd);

		for (n = 0; n < 3 && (c = io_getc(io)) != EOF; )
			if (c == '\n') n++;

		last = '\n';

		for (last = '\n'; (c = io_getc(io)) != EOF; last = c)
		{
			if	(last == '\n')
				io_puts("# ", aus);

			io_putc(c, aus);
		}

		if	(last != '\n')
			io_putc('\n', aus);

		io_close(io);
		deltemp(tmp1);
		deltemp(tmp2);

	}
	else if	(a)
	{
		put_head(aus, a, "***", fmt ? fmt : "-%d");
		put_mark(aus, a, "# < ");
	}
	else if	(b)
	{
		put_head(aus, b, "---", fmt ? fmt : "+%d");
		put_mark(aus, b, "# > ");
	}
}

static void df_mark (IO *aus, DATA *a, DATA *b, const char *fmt)
{
	if	(fmt)
	{
		io_xprintf(aus, "@ %s\n", fmt);
	}
	else if	(a && b)
	{
		if	(a->pos && b->pos)
			io_xprintf(aus, "%dc%d\n", a->pos, b->pos);
		else if	(a->pos)
			io_xprintf(aus, "%d,$D\n", a->pos);
		else if	(b->pos)
			io_xprintf(aus, "A%d,$\n", b->pos);
	}
	else if	(a)
	{
		io_xprintf(aus, "%dd\n", a->pos);
	}
	else if	(b)
	{
		io_xprintf(aus, "a%d\n", b->pos);
	}

	if	(a)	put_mark(aus, a, "< ");
	if	(b)	put_mark(aus, b, "> ");
}

static struct {
	char *name;
	char *desc;
	DFunc df;
} DiffTab[] = {
	{ "std", "Standardausgabe der Unterschiede", df_std },
	{ "mark", "Markierung der Unterschiede", df_mark },
	{ "diff", "Unterschiede mit diff darstellen", df_diff },
};

static void list_df (IO *io)
{
	int i;

	for (i = 0; i < tabsize(DiffTab); i++)
		io_xprintf(io, "%s\t%s\n", DiffTab[i].name, DiffTab[i].desc);
}

static DFunc get_df (const char *name)
{
	int i;

	for (i = 0; i < tabsize(DiffTab); i++)
	{
		if	(mstrcmp(DiffTab[i].name, name) == 0)
		{
			return DiffTab[i].df;
		}
	}

	dbg_error(NULL, "[2]", "s", name);
	return NULL;
}

static void data_open (DATA *data, const char *name, const char *filter)
{
	data->name = name ? name : "<stdin>";
	data->edb = edb_filter(edb_fopen(NULL, name), filter);
	data->edb = edb_filter(data->edb, GetResource("Filter", NULL));
	sb_init(&data->buf, 0);
	edb_out(data->edb, io_strbuf(&data->buf), GetResource("Print", NULL));
	data->pos = 0;
}

static void data_close (DATA *data)
{
	rd_deref(data->edb);
	sb_free(&data->buf);
}

static void data_str (DATA *data, const char *str)
{
	sb_trunc(&data->buf);
	sb_puts(data->edb->head, &data->buf);
}

static void data_type (DATA *data)
{
	IO *out;
	sb_trunc(&data->buf);
	out = io_strbuf(&data->buf);
	PrintType(out, data->edb->obj->type, 1);
	io_close(out);
}

static void data_next (DATA *data)
{
	sb_trunc(&data->buf);

	if	(edb_read(data->edb))
	{
		edb_write(data->edb);
		data->pos++;
	}
	else	data->pos = 0;
		
}

int main (int narg, char **arg)
{
	DATA dbuf[2], *d1, *d2;
	char *p;
	IO *aus;
	DFunc df;
	CmpDef *cmp;
	int c;
	int nd, ne;

	SetProgName(arg[0]);
	SetVersion("$Id: edbdiff.c,v 1.7 2008-08-11 21:12:40 ef Exp $");
	SetupStd();
	SetupUtil();
	SetupPreproc();
	SetupEDB();
	SetupTimeSeries();
	SetupRandom();
	SetupMdMat();
	SetupMath();
	SetupMdMath();
	SetupPnom();
	SetupMathFunc();
	SetupStatData();
	SetupDebug();
	ParseCommand(&narg, arg);

	Output = STR("Output");
	maxrec = INT("maxrec");
	maxerr = INT("maxerr");

	if	((p = STR("Diff")) == NULL || *p == '?')
	{
		list_df(ioerr);
		return EXIT_SUCCESS;
	}
	else	df = get_df(p);

	d1 = dbuf;
	d2 = dbuf + 1;

	data_open(d1, STR("DB1"), STR("Filter1"));
	data_open(d2, STR("DB2"), STR("Filter2"));
	aus = io_fileopen(Output, "wz");

	io_xprintf(aus, "*** %s\n", d1->name);
	io_xprintf(aus, "--- %s\n", d2->name);

	if	(cmp_buf(&d1->buf, &d2->buf))
	{
		df(aus, d1, d2, "header");
	}

	if	(mstrcmp(d1->edb->head, d2->edb->head) != 0)
	{
		data_str(d1, d1->edb->head);
		data_str(d2, d2->edb->head);
		df(aus, d1, d2, "head");
	}

	if	(mstrcmp(d1->edb->desc, d2->edb->desc) != 0)
	{
		data_str(d1, d1->edb->desc);
		data_str(d2, d2->edb->desc);
		df(aus, d1, d2, "desc");
	}

	if	(d1->edb->obj->type != d2->edb->obj->type)
	{
		data_type(d1);
		data_type(d2);
		df(aus, d1, d2, "type");
	}
	else
	{
		p = STR("Comp");
		cmp = p ? cmp_create(d1->edb->obj->type, p, NULL) : NULL;

		data_next(d1);
		data_next(d2);
		nd = 0;
		ne = 0;

		while (d1->pos || d2->pos)
		{
			if	(maxrec && nd >= maxrec)
				break;

			if	(maxerr && ne >= maxerr)
				break;

			if	(!d1->pos || !d2->pos)
			{
				df(aus, d1, d2, NULL);
				ne++;
				break;
			}

			nd++;
			c = cmp_data(cmp, d1->edb->obj->data,
				d2->edb->obj->data);

			if	(c < 0)
			{
				df(aus, d1, NULL, NULL);
				data_next(d1);
				ne++;
				continue;
			}

			if	(c > 0)
			{
				df(aus, NULL, d2, NULL);
				data_next(d2);
				ne++;
				continue;
			}

			if	(cmp_buf(&d1->buf, &d2->buf))
			{
				df(aus, d1, d2, NULL);
				ne++;
			}

			data_next(d1);
			data_next(d2);
		}
	}

	data_close(d1);
	data_close(d2);
	io_close(aus);
	return 0;
}
