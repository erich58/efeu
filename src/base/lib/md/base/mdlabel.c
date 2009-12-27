/*	Label generieren
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/mdlabel.h>


typedef struct {
	char *name;
	int idx;
} KEY;

static ALLOCTAB(key_tab, 32, sizeof(KEY));
static ALLOCTAB(label_tab, 32, sizeof(MdLabel_t));


static void del_key (void *ptr);
static int get_idx (xtab_t *tab, const char *name);
static mdaxis_t *idx2axis (xtab_t *tab, const char *name);

static void del_key(void *ptr)
{
	FREE(((KEY *) ptr)->name);
	del_data(&key_tab, ptr);
}


static mdaxis_t *idx2axis(xtab_t *tab, const char *name)
{
	mdaxis_t *x;
	int i;

	x = new_axis(tab->dim);
	x->name = mstrcpy(name);

	for (i = 0; i < tab->dim; i++)
	{
		KEY *key;

		key = tab->tab[i];
		x->idx[key->idx].name = mstrcpy(key->name);
	}

	return x;
}


static int get_idx(xtab_t *tab, const char *name)
{
	KEY *x, *y;

	if	(tab == NULL)	return 0;

	x = new_data(&key_tab);
	x->name = mstrcpy(name);
	x->idx = tab->dim;
	y = xsearch(tab, x, XS_ENTER);

	if	(x != y)	del_key(x);

	return (y ? y->idx : 0);
}


MdLabel_t *new_label(void)
{
	MdLabel_t *x;

	x = new_data(&label_tab);
	x->next = NULL;
	x->name = NULL;
	x->len = 0;
	x->dim = 0;
	x->list = NULL;
	x->idx = xcreate(0, skey_cmp);
	return x;
}


void del_label(MdLabel_t *x)
{
	del_label(x->next);
	xdestroy(x->idx, del_key);
	del_data(&label_tab, x);
}


MdLabel_t *set_label(const char *def)
{
	mdlist_t *list, *l;
	MdLabel_t *label, **ptr;

	list = mdlist(def, MDLIST_NAMEOPT);
	label = NULL;
	ptr = &label;

	for (l = list; l != NULL; l = l->next)
	{
		*ptr = new_label();
		(*ptr)->name = l->name;
		(*ptr)->len = (l->option ? atoi(l->option) : 0);
		(*ptr)->dim = l->dim;
		(*ptr)->list = l->list;
		ptr = &(*ptr)->next;
	}

	return label;
}


MdLabel_t *init_label(const char *name, const char *def)
{
	int dim;
	int i;
	char **list;
	MdLabel_t *label, **ptr;
	char *fmt;

	dim = strsplit(def, ".", &list);
	label = NULL;
	ptr = &label;
	fmt = (dim > 1 ? "%s%d" : "%s");

	for (i = 0; i < dim; i++)
	{
		*ptr = new_label();
		(*ptr)->name = msprintf(fmt, name, i + 1);
		ptr = &(*ptr)->next;
	}

	return label;
}


mdaxis_t *label2axis(MdLabel_t *label)
{
	mdaxis_t *axis, **ptr;

	axis = NULL;
	ptr = &axis;

	while (label != NULL)
	{
		*ptr = idx2axis(label->idx, label->name);
		ptr = &(*ptr)->next;
		label = label->next;
	}

	return axis;
}


int save_label(io_t *tmp, MdLabel_t *label, char *p)
{
	char *x;
	int i;

	while (label != NULL)
	{
		if	(p == NULL)
		{
			io_putc(0, tmp);
			io_putc(0, tmp);
			return 1;
		}

		if	(label->next == NULL)
		{
			i = get_idx(label->idx, p);
			io_putc((i >> 8) & 0xFF, tmp);
			io_putc(i & 0xFF, tmp);
			break;
		}

		x = p;

		for (i = 0; p[i] != 0; i++)
		{
			if	(p[i] == '.')
			{
				x = mstrncpy(p, i++);
				p += i;
				break;
			}
		}

		i = get_idx(label->idx, x);
		io_putc((i >> 8) & 0xFF, tmp);
		io_putc(i & 0xFF, tmp);
		label = label->next;
	}

	return 0;
}
