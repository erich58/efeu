/*	MDMAT - Datenstruktur generieren/freigeben
	(c) 1994 Erich Frühstück
*/

#include <EFEU/mdmat.h>

/*	Strukturgrößen berechnen
*/

static size_t set_size(mdaxis *x, size_t size)
{
	if	(x != NULL)
	{
		x->size = set_size(x->next, size);
		return x->size * x->dim;
	}
	else	return size;
}

size_t md_size(mdmat *md, mdaxis *axis)
{
	return set_size(axis, md->type->size);
}

static void md_clean (void *data)
{
	mdmat *tg = data;
	del_axis(tg->axis);

	if	(!tg->map && tg->data != rd_data(tg->x_data))
		memfree(tg->data);

	rd_deref(tg->sbuf);
	rd_deref(tg->x_data);
	rd_deref(tg->map);
	memfree(tg);
}

static char *md_ident (const void *data)
{
	const mdmat *md;
	StrBuf *sb;
	IO *io;

	md = data;
	sb = sb_acquire();
	io = io_strbuf(sb);
	io_xprintf(io, "name=%#s, type=", StrPool_get(md->sbuf, md->i_name));
	ShowType(io, md->type);
	io_xprintf(io, ", dim=%d", md_dim(md->axis));
	io_xprintf(io, ", size=%ld", md->size);
	io_close(io);
	return sb_cpyrelease(sb);
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

	md->size = md_size(md, md->axis);
	md->x_data = md_alloc_hook ? md_alloc_hook(md->size) :
		rd_wrap(lmalloc(md->size), lfree);
	md->data = rd_data(md->x_data);
	memset(md->data, 0, md->size);
}
