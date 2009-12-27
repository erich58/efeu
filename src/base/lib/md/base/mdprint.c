/*	Multidimensionale Matrix standardisiert ausgeben
	(c) 1994 Erich Frühstück
*/

#include <EFEU/printobj.h>
#include <EFEU/mdmat.h>
#include <EFEU/pctrl.h>
#include <EFEU/locale.h>

#define H_MAGIC		"##MDMAT   "
#define H_TITLE		"##Titel   "
#define H_TYPE		"##Type    "
#define H_LOCALE	"##Locale  "
#define H_LINES		"##Zeilen  "
#define H_COLUMNS	"##Spalten "
#define H_VERSION	"2.0"

#define	LLABEL	"."
#define	CLABEL	"."

#define	XMARK	MDXFLAG_MARK

static int head_flag = 2;
static int data_flag = 1;
static int mark_flag = 2;
static int zero_flag = 1;
static int label_width = 0;

static int cur_line = 0;
static int max_line = 0;
static int max_col = 0;

extern int PrintFieldWidth;
extern int PrintFloatPrec;

static PrCtrl_t *set_pctrl(const char *def)
{
	char *p, **list;
	PrCtrl_t *pctrl;
	size_t dim;
	int i;

	pctrl = PrCtrl(NULL);
	dim = strsplit(def, "%s", &list);

	for (i = 0; i < dim; i++)
	{
		if	(list[i] == NULL)
			;
		else if	(strcmp("nodata", list[i]) == 0)
			data_flag = 0;
		else if	(strcmp("data", list[i]) == 0)
			data_flag = 1;
		else if	(strcmp("nohead", list[i]) == 0)
			head_flag = 0;
		else if	(strcmp("head", list[i]) == 0)
			head_flag = 2;
		else if	(strcmp("title", list[i]) == 0)
			head_flag = 1;
		else if	(strcmp("nomark", list[i]) == 0)
			mark_flag = 0;
		else if	(strcmp("mark", list[i]) == 0)
			mark_flag = 1;
		else if	(strcmp("nozero", list[i]) == 0)
			zero_flag = 0;
		else if	(strcmp("zero", list[i]) == 0)
			zero_flag = 1;
		else if	(patcmp("label=", list[i], &p))
			label_width = atoi(p);
		else if	(patcmp("landscape", list[i], NULL))
			pctrl_pgfmt = "a4r";
		else if	(patcmp("paper=", list[i], &p))
			pctrl_pgfmt = mstrcpy(p);
		else if	(patcmp("*pt", list[i], NULL))
			pctrl_fsize = atoi(list[i]);
		else if	(patcmp("font=", list[i], &p))
			pctrl_fsize = atoi(p);
		else if	(patcmp("mode=", list[i], &p))
			pctrl = PrCtrl(p);
	}

	memfree(list);
	return pctrl;
}

static void put_label(io_t *io, const char *key, const char *arg)
{
	if	(arg)
	{
		io_ctrl(io, PCTRL_LEFT);
		io_puts(key, io);
		io_mputs(arg, io, ":");
		io_ctrl(io, PCTRL_LINE);
	}
}

static void show_axis(io_t *io, const char *label, mdaxis_t *axis, int flag)
{
	char *delim;

	io_ctrl(io, PCTRL_LEFT);
	io_puts(label, io);

	for (delim = NULL; axis != NULL; axis = axis->next)
	{
		if	((axis->flags & XMARK) == flag)
		{
			io_puts(delim, io);
			io_puts(axis->name, io);
			delim = " ";
		}
	}

	io_ctrl(io, PCTRL_LINE);
}


static void show_header(io_t *io, mdmat_t *md)
{
	put_label(io, H_MAGIC, H_VERSION);
	put_label(io, H_TITLE, md->title);

	io_ctrl(io, PCTRL_LEFT);
	io_puts(H_TYPE, io);
	ShowType(io, md->type);
	io_ctrl(io, PCTRL_LINE);

	if	(Locale.print)
		put_label(io, H_LOCALE, Locale.print->name);

	show_axis(io, H_LINES, md->axis, 0);
	show_axis(io, H_COLUMNS, md->axis, XMARK);
}


static int vardim(Type_t *type, size_t dim)
{
	if	(dim)
	{
		return dim * vardim(type, 0);
	}
	else if	(type->dim)
	{
		return vardim(type->base, type->dim);
	}
	else if	(type->list)
	{
		Var_t *st;
		int n;

		for (n = 0, st = type->list; st != NULL; st = st->next)
			n += vardim(st->type, st->dim);

		return n;
	}
	else	return 1;
}

static int headdim(mdmat_t *md)
{
	mdaxis_t *x;
	int n;

	for (n = 1, x = md->axis; x != NULL; x = x->next)
		if (x->flags & XMARK) n *= x->dim;

	return n * vardim(md->type, 0);
}

static void headline(io_t *io, mdaxis_t *x, Type_t *type, const char *str);

static void headline2(io_t *io, size_t dim, Type_t *type, const char *str)
{
	int i;
	char *p;

	for (i = 0; i < dim; i++)
	{
		p = msprintf("%s[%d]", str, i);
		headline(io, NULL, type, p);
		memfree(p);
	}
}


static void headline(io_t *io, mdaxis_t *x, Type_t *type, const char *str)
{
	while (x != NULL && !(x->flags & XMARK))
		x = x->next;

	if	(x != NULL)
	{
		int i;
		char *p;

		for (i = 0; i < x->dim; i++)
		{
			p = mstrpaste(".", str, x->idx[i].name);
			headline(io, x->next, type, p);
			memfree(p);
		}
	}
	else if	(type->dim != 0)
	{
		headline2(io, type->dim, type->base, str);
	}
	else if	(type->list != NULL)
	{
		Var_t *st;
		char *p;

		for (st = type->list; st != NULL; st = st->next)
		{
			p = mstrpaste(".", str, st->name);

			if	(st->dim)
			{
				headline2(io, st->dim, st->type, p);
			}
			else	headline(io, NULL, st->type, p);

			memfree(p);
		}
	}
	else
	{
		io_ctrl(io, PCTRL_RIGHT);
		io_printf(io, "%*s", PrintFieldWidth, str ? str : CLABEL);
	}
}

static void put_header(io_t *io, mdmat_t *md)
{
	int hd = headdim(md);

	io_ctrl(io, PCTRL_DATA, hd);

	if	(mark_flag == 0)	return;

	io_ctrl(io, PCTRL_HEAD);
	io_ctrl(io, PCTRL_LEFT);

	switch (mark_flag)
	{
	case 1:	io_printf(io, "#%-*d", max(0, label_width - 1), hd); break;
	case 2:	io_printf(io, "%-*d", label_width, hd); break;
	default: break;
	}

	headline(io, md->axis, md->type, NULL);
	io_ctrl(io, PCTRL_LINE);
	io_ctrl(io, PCTRL_EHEAD);
}

/*	Spalten durchwandern
*/

static void t_walk(io_t *io, Type_t *type, size_t dim, char *ptr)
{
	if	(dim)
	{
		int i;

		for (i = 0; i < dim; i++)
		{
			t_walk(io, type, 0, ptr);
			ptr += type->size;
		}
	}
	else if	(type->dim)
	{
		t_walk(io, type->base, type->dim, ptr);
	}
	else if	(type->list)
	{
		Var_t *st;

		for (st = type->list; st != NULL; st = st->next)
			t_walk(io, st->type, st->dim, ptr + st->offset);
	}
	else if	(zero_flag || Obj2bool(ConstObj(type, ptr)))
	{
		io_ctrl(io, PCTRL_VALUE);
		PrintData(io, type, ptr);
	}
	else	io_ctrl(io, PCTRL_EMPTY);
}

static void c_walk(io_t *io, mdaxis_t *x, Type_t *type, char *ptr)
{
	while (x != NULL && !(x->flags & XMARK))
		x = x->next;

	if	(x != NULL)
	{
		int i;

		for (i = 0; i < x->dim; i++)
		{
			c_walk(io, x->next, type, ptr);
			ptr += x->size;
		}
	}
	else	t_walk(io, type, 0, ptr);
}


/*	Zeilen durchwandern
*/

static void l_walk(io_t *io, mdmat_t *md, mdaxis_t *x, const char *label, char *ptr)
{
	while (x != NULL && (x->flags & XMARK))
		x = x->next;

	if	(x != NULL)
	{
		int i;
		char *p;

		for (i = 0; i < x->dim; i++)
		{
			p = mstrpaste(".", label, x->idx[i].name);
			l_walk(io, md, x->next, p, ptr);
			memfree(p);
			ptr += x->size;
		}
	}
	else
	{
		if	(max_line > 0 && io_ctrl(io, PCTRL_YPOS) >= max_line)
		{
			io_ctrl(io, PCTRL_EDATA);
			io_ctrl(io, PCTRL_PAGE);
			put_header(io, md);
			cur_line = 0;
		}

		io_ctrl(io, PCTRL_LEFT);
		io_printf(io, "%-*s", label_width, label ? label : LLABEL);
		c_walk(io, md->axis, md->type, ptr);
		io_ctrl(io, PCTRL_LINE);
		cur_line++;
	}
}


static void show_data(io_t *io, mdmat_t *md)
{
	put_header(io, md);
	cur_line = 0;
	max_line = io_ctrl(io, PCTRL_LINES);
	max_col = io_ctrl(io, PCTRL_COLUMNS);
	l_walk(io, md, md->axis, NULL, md->data);
	io_ctrl(io, PCTRL_EDATA);
}

void md_print(io_t *io, mdmat_t *md, const char *def)
{
	if	(md == NULL)	return;

	io = io_PrCtrl(io, set_pctrl(def));

	switch (head_flag)
	{
	case  2:	show_header(io, md); break;
	case  1:	put_label(io, NULL, md->title); break;
	default:	break;
	}

	if	(data_flag)	show_data(io, md);

	io_close(io);
}
