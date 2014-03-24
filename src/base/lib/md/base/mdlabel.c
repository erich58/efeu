/*	Label generieren
	(c) 1994 Erich Frühstück
*/

#include <EFEU/mdlabel.h>


typedef struct {
	char *name;
	int idx;
} KEY;

static ALLOCTAB(label_tab, "MdAxisLabel", 32, sizeof(MdAxisLabel));

static void del_key (void *ptr);
static int get_idx (VecBuf *tab, const char *name);
static mdaxis *idx2axis (StrPool *sbuf, VecBuf *tab, const char *name);

static void del_key (void *ptr)
{
	memfree(((KEY *) ptr)->name);
}

static int cmp_key (const void *a, const void *b)
{
	return mstrcmp(((const KEY *) a)->name, ((const KEY *) b)->name);
}

static mdaxis *idx2axis (StrPool *sbuf, VecBuf *vb, const char *name)
{
	KEY *key;
	mdaxis *x;
	int n;

	x = new_axis(sbuf, vb->used);
	x->i_name = StrPool_add(x->sbuf, name);
	x->i_desc = 0;

	for (key = vb->data, n = vb->used; n-- > 0; key++)
		x->idx[key->idx].i_name = StrPool_add(x->sbuf, key->name);

	return x;
}


static int get_idx (VecBuf *vb, const char *name)
{
	if	(vb)
	{
		KEY key, *ptr;

		key.name = mstrcpy(name);
		key.idx = vb->used;
		ptr = vb_search(vb, &key, cmp_key, VB_ENTER);
		memfree(key.name);
		return (ptr ? ptr->idx : 0);
	}
	else	return 0;
}


MdAxisLabel *new_label (void)
{
	MdAxisLabel *x;

	x = new_data(&label_tab);
	x->next = NULL;
	x->name = NULL;
	x->len = 0;
	x->dim = 0;
	x->list = NULL;
	vb_init(&x->idxtab, 1024, sizeof(KEY));
	return x;
}


void del_label (MdAxisLabel *x)
{
	del_label(x->next);
	vb_clean(&x->idxtab, del_key);
	del_data(&label_tab, x);
}


MdAxisLabel *set_label (const char *def)
{
	mdlist *list, *l;
	MdAxisLabel *label, **ptr;

	list = str2mdlist(def, MDLIST_NAMEOPT);
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


MdAxisLabel *init_label (const char *name, const char *def)
{
	int dim;
	int i;
	char **list;
	MdAxisLabel *label, **ptr;
	char *fmt;

	dim = mstrsplit(def, ".", &list);
	label = NULL;
	ptr = &label;
	fmt = (dim > 1 ? "%s%d" : "%s");

	for (i = 0; i < dim; i++)
	{
		*ptr = new_label();
		(*ptr)->name = msprintf(fmt, name, i + 1);
		ptr = &(*ptr)->next;
	}

	memfree(list);
	return label;
}


mdaxis *label2axis (StrPool *sbuf, MdAxisLabel *label)
{
	mdaxis *axis, **ptr;

	axis = NULL;
	ptr = &axis;

	while (label != NULL)
	{
		*ptr = idx2axis(sbuf, &label->idxtab, label->name);
		ptr = &(*ptr)->next;
		label = label->next;
	}

	return axis;
}


int save_label (IO *tmp, MdAxisLabel *label, char *p)
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
			i = get_idx(&label->idxtab, p);
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

		i = get_idx(&label->idxtab, x);
		io_putc((i >> 8) & 0xFF, tmp);
		io_putc(i & 0xFF, tmp);
		label = label->next;
	}

	return 0;
}
