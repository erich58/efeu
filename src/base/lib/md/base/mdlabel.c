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
static int get_idx (VecBuf *tab, char *name);
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


static int get_idx (VecBuf *vb, char *name)
{
	KEY key, *ptr;

	key.name = name;
	key.idx = vb->used;
	ptr = vb_search(vb, &key, cmp_key, VB_ENTER);
	memfree(key.name);
	return (ptr ? ptr->idx : 0);
}


MdAxisLabel *new_label (void)
{
	MdAxisLabel *x;

	x = new_data(&label_tab);
	x->next = NULL;
	x->name = NULL;
	vb_init(&x->idxtab, 1024, sizeof(KEY));
	return x;
}


void del_label (MdAxisLabel *x)
{
	if	(x)
	{
		del_label(x->next);
		vb_clear(&x->idxtab, del_key);
		del_data(&label_tab, x);
	}
}


MdAxisLabel *set_label (const char *def)
{
	MdAxisLabel *label, **ptr;
	char **list;

	size_t dim = mstrsplit(def, "%s", &list);
	label = NULL;
	ptr = &label;

	for (size_t i = 0; i < dim; i++)
	{
		*ptr = new_label();
		(*ptr)->name = mstrcpy(list[i]);
		ptr = &(*ptr)->next;
	}

	memfree(list);
	return label;
}


MdAxisLabel *init_label (const char *name, const char *def)
{
	char **list;
	MdAxisLabel *label, **ptr;
	char *fmt;

	size_t dim = mstrsplit(def, ".", &list);
	label = NULL;
	ptr = &label;
	fmt = (dim > 1 ? "%s%zd" : "%s");

	for (size_t i = 0; i < dim; i++)
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

	if	(!p)
		return 1;

	while (label != NULL)
	{
		if	(label->next == NULL)
		{
			i = get_idx(&label->idxtab, mstrcpy(p));
			io_putc((i >> 8) & 0xFF, tmp);
			io_putc(i & 0xFF, tmp);
			break;
		}

		x = NULL;

		for (i = 0; p[i] != 0; i++)
		{
			if	(p[i] == '.')
			{
				x = mstrncpy(p, i++);
				p += i;
				break;
			}
		}

		i = get_idx(&label->idxtab, x ? x : mstrcpy(p));
		io_putc((i >> 8) & 0xFF, tmp);
		io_putc(i & 0xFF, tmp);
		label = label->next;
	}

	return 0;
}
