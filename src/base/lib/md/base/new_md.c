/*	MDMAT - Datenstruktur generieren/freigeben
	(c) 1994 Erich Fr�hst�ck
*/


#include <EFEU/mdmat.h>


static void md_clean (void *data)
{
	mdmat *tg = data;
	del_axis(tg->axis);
	memfree(tg->title);

	if	(tg->data != rd_data(tg->x_data))
		memfree(tg->data);

	rd_deref(tg->x_data);
	memfree(tg);
}

static char *md_ident (const void *data)
{
	const mdmat *md;
	StrBuf *sb;
	IO *io;

	md = data;
	sb = sb_create(0);
	io = io_strbuf(sb);
	io_printf(io, "name=%#s, type=", md->title);
	ShowType(io, md->type);
	io_printf(io, ", dim=%d", md_dim(md->axis));
	io_printf(io, ", size=%ld", md->size);
	io_close(io);
	return sb2str(sb);
}

RefType md_reftype = REFTYPE_INIT("MD", md_ident, md_clean);


mdmat *new_mdmat(void)
{
	return rd_init(&md_reftype, memalloc(sizeof(mdmat)));
}

void *(*md_alloc_hook) (size_t size) = NULL;

void md_alloc (mdmat *md)
{
	if	(!md)	return;

	md->size = md_size(md->axis, md->type->size);
	md->x_data = md_alloc_hook ? md_alloc_hook(md->size) :
		rd_wrap(lmalloc(md->size), lfree);
	md->data = rd_data(md->x_data);
	memset(md->data, 0, md->size);
}

/*	Datenmatrix kopieren
*/

static void do_copy (mdaxis *y, mdaxis *x, size_t size,
	char *py, char *px, unsigned mask)
{
	int i;

	if	(x == NULL)
	{
		memcpy(py, px, size);
		return;
	}

	for (i = 0; i < x->dim; i++)
	{
		if	(!(x->idx[i].flags & mask))
		{
			do_copy(y->next, x->next, size, py, px, mask);
			py += y->size;
		}

		px += x->size;
	}
}


mdmat *cpy_mdmat (const mdmat *md, unsigned mask)
{
	mdmat *m2;
	mdaxis *x, **ptr;

	if	(md == NULL)	return NULL;

	m2 = new_mdmat();
	m2->axis = NULL;
	m2->type = md->type;

	ptr = &m2->axis;

	for (x = md->axis; x != NULL; x = x->next)
	{
		*ptr = cpy_axis(x, mask);
		ptr = &(*ptr)->next;
	}

	md_alloc(m2);
	do_copy(m2->axis, md->axis, m2->type->size, m2->data, md->data, mask);
	return m2;
}

mdmat *md_clone (const mdmat *base)
{
	mdmat *md;
	mdaxis *x, **ptr;

	if	(base == NULL)	return NULL;

	md = new_mdmat();
	md->axis = NULL;
	md->type = base->type;

	ptr = &md->axis;

	for (x = base->axis; x != NULL; x = x->next)
	{
		*ptr = cpy_axis(x, 0);
		ptr = &(*ptr)->next;
	}

	md->title = mstrcpy(base->title);
	md->size = base->size;
	md->x_data = rd_refer(base->x_data);
	md->data = rd_data(md->x_data);
	return md;
}
