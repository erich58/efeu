/*
Multidimensionale Matrix einlesen

$Copyright (C) 1994, 2005 Erich Frühstück
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

#include <EFEU/mdmat.h>
#include <EFEU/mdlabel.h>
#include <EFEU/locale.h>
#include <EFEU/preproc.h>

static char *XNames = NULL;
static char *YNames = NULL;
static EfiType *Type = NULL;
static int has_magic = 0;
static int need_magic = 0;

static unsigned getshort (IO *io)
{
	int a, b;

	a = io_getc(io);
	b = io_getc(io);

	if	(a != EOF && b != EOF)
		return ((a << 8) + b);

	return 0;
}

static void clr_par (void)
{
	memfree(XNames);
	memfree(YNames);
}

static void set_par (const char *def)
{
	char *p, **list;
	size_t dim;
	int i;

	XNames = NULL;
	YNames = NULL;
	Type = &Type_double;

	dim = mstrsplit(def, "%s", &list);

	for (i = 0; i < dim; i++)
	{
		if	(list[i] == NULL)
			;
		else if	(patcmp("x=", list[i], &p))
			XNames = mstrcpy(p);
		else if	(patcmp("y=", list[i], &p))
			YNames = mstrcpy(p);
		else if	(patcmp("locale=", list[i], &p))
			SetLocale(LOC_SCAN, p);
		else if	(patcmp("de", list[i], NULL))
			SetLocale(LOC_SCAN, "de");
		else if	(patcmp("us", list[i], NULL))
			SetLocale(LOC_SCAN, "us");
		else if	(patcmp("magic", list[i], NULL))
			need_magic = 1;
		else if	(patcmp("nomagic", list[i], NULL))
			need_magic = 0;
		else if	(patcmp("type=", list[i], &p))
			Type = str2Type(p);
	}

	memfree(list);
}


/*	Header lesen
*/

#define	H_COMMENT	0
#define	H_TITLE		1
#define	H_HEAD		2
#define	H_TYPE		3
#define	H_ROWS		4
#define	H_COLUMNS	5
#define	H_LOCALE	6
#define	H_MAGIC		7


static struct {
	char *name;
	int code;
} head[] = {
	{ "MD*",	H_MAGIC },
	{ "[Tt]it*",	H_TITLE },
	{ "[Hh]ead*",	H_HEAD },
	{ "[Oo]bj*",	H_TYPE },
	{ "[Tt]yp*",	H_TYPE },
	{ "[Ll]in*",	H_ROWS },
	{ "[Zz]ei*",	H_ROWS },
	{ "[Rr]ow*",	H_ROWS },
	{ "[Cc]ol*",	H_COLUMNS },
	{ "[Ss]pal*",	H_COLUMNS },
	{ "[Ll]oc*",	H_LOCALE },
};

#define	SETPAR(name)	(save = name, name = p, p = save)

static void eval_head (const char *head)
{
	IO *cin = io_cmdpreproc(io_cstr(head));
	CmdEval(cin, NULL);
	io_close(cin);
}

static char *get_header(IO *io)
{
	char *p, *save;
	char *title;
	int i, c;

	title = NULL;

	while ((c = io_getc(io)) != EOF)
	{
		if	(c == '\n')
		{
			continue;
		}
		else if	(c != '#')
		{
			io_ungetc(c, io);
			break;
		}

		c = io_getc(io);

		if	(c == '#')
		{
			io_eat(io, " \t");
			p = io_mgets(io, "%s:;");
			c = H_COMMENT;

			for (i = 0; i < tabsize(head); i++)
			{
				if	(patcmp(head[i].name, p, NULL))
				{
					memfree(p);
					io_eat(io, " \t:;");
					p = io_mgets(io, ":\n");
					c = head[i].code;
					break;
				}
			}

			switch (c)
			{
			case H_MAGIC:	has_magic = 1; break;
			case H_HEAD:	eval_head(p); break;
			case H_TITLE:	SETPAR(title); break;
			case H_ROWS:	SETPAR(YNames); break;
			case H_COLUMNS:	SETPAR(XNames); break;
			case H_TYPE:	Type = str2Type(p); break;
			case H_LOCALE:	SetLocale(LOC_SCAN, p); break;
			default:	break;
			}

			memfree(p);
		}

		while ((c = io_getc(io)) != EOF)
			if (c == '\n') break;
	}
	
	return title;
}

static int new_line(IO *io)
{
	int c;

	while ((c = io_getc(io)) != EOF)
	{
		if	(c == '#')
		{
			io_mcopy(io, NULL, "\n");
		}
		else if	(c == ':' || c == ';')
		{
			io_mcopy(io, NULL, "\n");
		}
		else if	(c != '\n')
		{
			io_ungetc(c, io);
			return c;
		}
	}

	return c;
}

static void sub_copy (IO *io, StrBuf *sb, int beg, int end)
{
	int c;

	sb_putc(beg, sb);

	while ((c = io_mgetc(io, 0)) != EOF)
	{
		switch (c)
		{
		case '(':	sub_copy(io, sb, c, ')'); continue;
		case '{':	sub_copy(io, sb, c, '}'); continue;
		case '[':	sub_copy(io, sb, c, ']'); continue;
		default:	break;
		}

		sb_putc(c, sb);

		if	(c == end)	break;
	}
}

static void do_scan (IO *io, StrBuf *sb)
{
	int c;

	while ((c = io_mgetc(io, 0)) != EOF)
	{
		switch (c)
		{
		case '\\':
			c = io_mgetc(io, 0);

			switch (c)
			{
			case EOF:
				c = '\\';
				break;
			case ' ':
			case '\t':
			case '\n':
			case ':':
			case ';':
				break; default:
				sb_putc('\\', sb);
				break;
			}

			break;
		case ' ':
		case '\t':
		case '\n':
		case ':':
		case ';':
			io_ungetc(c, io);
			return;
		case '(':	sub_copy(io, sb, c, ')'); continue;
		case '{':	sub_copy(io, sb, c, '}'); continue;
		case '[':	sub_copy(io, sb, c, ']'); continue;
		default:	break;
		}

		sb_putc(c, sb);
	}
}

static int mdgetstat = 0;

static char *mdscan(IO *io)
{
	int c;
	int flag;

	flag = 0;
	mdgetstat = 0;

	while ((c = io_eat(io, " \t")) != EOF)
	{
		if	(c == '\n')
		{
			break;
		}
		else if	(c == ':' || c == ';')
		{
			if	(flag)
			{
				mdgetstat = 1;
				return NULL;
			}

			io_getc(io);
			flag = 1;
		}
		else
		{
			StrBuf *sb = sb_acquire();
			do_scan(io, sb);
			return sb_cpyrelease(sb);
		}
	}

	mdgetstat = EOF;
	return NULL;
}

static void clr_oname(char *p, const char *name)
{
	int a;
	int b;

	if	(p == NULL || name == NULL)
		return;

	a = strlen(p);
	b = strlen(name);

	if	(a < b)	return;

	a -= b;

	if	(strcmp(p + a, name) == 0)
	{
		p[a--] = 0;

		if	(p[a] == '.')	p[a] = 0;

		return;
	}
}



static char *get_col(IO *io, const char *name, EfiType *type, int n)
{
	char *last;
	char *p;
	char *subname;
	EfiStruct *st;

	if	(type->list == NULL)
	{
		p = mdscan(io);

		if	(mdgetstat == EOF)
			return NULL;

		clr_oname(p, name);

		if	(p == NULL)
		{
#if	1
			do	memfree(mdscan(io));
			while	(mdgetstat != EOF);
#else
			io_error(io, "[mdmat:193]", NULL);
			p = msprintf("%d", n);
#endif
		}

		return p;
	}

	last = NULL;

	for (st = type->list; st != NULL; st = st->next)
	{
		subname = mstrpaste(".", name, st->name);
		p = get_col(io, subname, st->type, n);
		memfree(subname);

		if	(p == NULL && last == NULL)	break;

		clr_oname(p, name);

		if	(p == NULL)
		{
			io_error(io, "[mdmat:193]", NULL);
			p = msprintf("%d", n);
		}

		if	(last == NULL)
		{
			last = p;
		}
		else if	(strcmp(last, p) != 0)
		{
			io_error(io, "[mdmat:194]", "sm", last, p);
		}
		else	memfree(p);
	}

	return last;
}


static int get_val(IO *io, EfiType *type, char *ptr)
{
	EfiStruct *st;
	EfiObj *obj;
	char *p;

	memset(ptr, 0, type->size);

	if	(type->list == NULL)
	{
		p = mdscan(io);

		if	(mdgetstat == EOF)	return 0;
		if	(p == NULL)		return 1;

		obj = strterm(p);
		memfree(p);
		Obj2Data(obj, type, ptr);
		return 1;
	}

	for (st = type->list; st != NULL; st = st->next)
	{
		if	(!get_val(io, st->type, ptr + st->offset))
			return 0;
	}

	return 1;
}


static mdmat *textmode(IO *io, char *title)
{
	IO *tmp;
	mdmat *md;
	MdAxisLabel *xlabel, *ylabel;
	mdaxis **ptr, *xaxis, *x;
	long *offset;
	size_t linedim, coldim;
	char *p, *buf;
	int i;

/*	Achsen bestimmen
*/
	md = new_mdmat();
	md->sbuf = NewStrPool();
	md->i_name = StrPool_xadd(md->sbuf, title);
	md->type = Type ? Type : &Type_double;
	xlabel = set_label(XNames);
	ylabel = set_label(YNames);

/*	Spaltenbezeichner lesen
*/
	tmp = io_bigbuf(0, NULL);
	io_ctrl(io, ICTRL_TEXT);

	if	(new_line(io) == EOF)
	{
		io_error(io, "[mdmat:192]", 0);
		del_mdmat(md);
		return NULL;
	}

	io_mcopy(io, NULL, "%s:;");
	coldim = 0;

	while ((p = get_col(io, NULL, md->type, coldim)) != NULL)
	{
		if	(xlabel == NULL)
			xlabel = init_label("x", p);

		if	(save_label(tmp, xlabel, p))
			io_error(io, "[mdmat:195]", NULL);

		memfree(p);
		coldim++;
	}

	offset = memalloc(coldim * sizeof(long));
	xaxis = label2axis(md->sbuf, xlabel);
	md_size(xaxis, md->type->size);
	io_rewind(tmp);

	for (i = 0; i < coldim; i++)
	{
		offset[i] = 0;

		for (x = xaxis; x != NULL; x = x->next)
			offset[i] += getshort(tmp) * x->size;
	}

	io_rewind(tmp);

/*	Datenwerte lesen
*/
	buf = memalloc(md->type->size);

	for (linedim = 0; new_line(io) != EOF; linedim++)
	{
		p = io_mgets(io, "%s:;");

		if	(ylabel == NULL)
			ylabel = init_label("y", p);

		if	(save_label(tmp, ylabel, p))
			io_error(io, "[mdmat:195]", NULL);

		memfree(p);

		for (i = 0; get_val(io, md->type, buf); i++)
		{
			if	(i < coldim)
				io_write(tmp, buf, md->type->size);
#if	0
			else	io_error(io, "[mdmat:196]", NULL);
#endif
		}

		if	(i < coldim)
			io_nputc(0, tmp, (coldim - i) * md->type->size);
	}

	memfree(buf);
	io_rewind(tmp);

/*	Achsen zusammenstellen
*/
	md->axis = label2axis(md->sbuf, ylabel);
	ptr = &md->axis;

	while (*ptr != NULL)
		ptr = &(*ptr)->next;

	*ptr = xaxis;

/*	Daten laden
*/
	md_alloc(md);
	memset(md->data, 0, md->size);

	for (i = 0; i < linedim; i++)
	{
		char *pos;
		int j;

		pos = md->data;

		for (x = md->axis; x != xaxis; x = x->next)
			pos += getshort(tmp) * x->size;

		for (j = 0; j < coldim; j++)
			io_read(tmp, pos + offset[j], md->type->size);
	}

	return md;
}


mdmat *md_read(IO *io, const char *def)
{
	int m1, m2;
	mdmat *md;
	char *title;

	if	((m1 = io_getc(io)) == EOF)
		return NULL;

	if	((m2 = io_getc(io)) == EOF)
		return NULL;

	io_ungetc(m2, io);
	io_ungetc(m1, io);
	m1 = (m1 << 8) + m2;

	if	(m1 == MD_MAGIC1 || m1 == MD_MAGIC2)
		return md_load(io, NULL, NULL);

	PushLocale();
	need_magic = 0;
	has_magic = 0;
	set_par(def);
	title = get_header(io);

	if	(need_magic && !has_magic)
	{
		memfree(title);
		md = NULL;
	}
	else	md = textmode(io, title);

	clr_par();
	PopLocale();
	return md;
}
