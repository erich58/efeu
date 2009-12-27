/*	Label generieren
	(c) 1994 Erich Frühstück
*/

#include <EFEU/mdlabel.h>


typedef struct {
	char *name;
	int idx;
} KEY;

static ALLOCTAB(label_tab, 32, sizeof(MdLabel));

static void del_key (void *ptr);
static int get_idx (VecBuf *tab, const char *name);
static mdaxis *idx2axis (VecBuf *tab, const char *name);

static void del_key (void *ptr)
{
	memfree(((KEY *) ptr)->name);
}

static int cmp_key (const void *a, const void *b)
{
	return mstrcmp(((const KEY *) a)->name, ((const KEY *) b)->name);
}

static mdaxis *idx2axis (VecBuf *vb, const char *name)
{
	KEY *key;
	mdaxis *x;
	int n;

	x = new_axis(vb->used);
	x->name = mstrcpy(name);

	for (key = vb->data, n = vb->used; n-- > 0; key++)
		x->idx[key->idx].name = mstrcpy(key->name);

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


MdLabel *new_label (void)
{
	MdLabel *x;

	x = new_data(&label_tab);
	x->next = NULL;
	x->name = NULL;
	x->len = 0;
	x->dim = 0;
	x->list = NULL;
	vb_init(&x->idxtab, 1024, sizeof(KEY));
	return x;
}


void del_label (MdLabel *x)
{
	del_label(x->next);
	vb_clean(&x->idxtab, del_key);
	del_data(&label_tab, x);
}


MdLabel *set_label (const char *def)
{
	mdlist *list, *l;
	MdLabel *label, **ptr;

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


MdLabel *init_label (const char *name, const char *def)
{
	int dim;
	int i;
	char **list;
	MdLabel *label, **ptr;
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


mdaxis *label2axis (MdLabel *label)
{
	mdaxis *axis, **ptr;

	axis = NULL;
	ptr = &axis;

	while (label != NULL)
	{
		*ptr = idx2axis(&label->idxtab, label->name);
		ptr = &(*ptr)->next;
		label = label->next;
	}

	return axis;
}


int save_label (IO *tmp, MdLabel *label, char *p)
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
