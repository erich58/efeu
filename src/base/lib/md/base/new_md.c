/*	MDMAT - Datenstruktur generieren/freigeben
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 2.0
*/


#include <EFEU/mdmat.h>


static mdmat_t *md_admin(mdmat_t *tg, const mdmat_t *src)
{
	if	(tg)
	{
		del_axis(tg->axis);
		memfree(tg->title);
		memfree(tg->data);
		memfree(tg);
		return NULL;
	}
	else	return memalloc(sizeof(mdmat_t));
}

char *md_ident(mdmat_t *md)
{
	strbuf_t *sb;
	io_t *io;

	sb = new_strbuf(0);
	io = io_strbuf(sb);
	io_printf(io, "name=%#s, type=", md->title);
	ShowType(io, md->type);
	io_printf(io, ", dim=%d", md_dim(md->axis));
	io_printf(io, ", size=%ld", md->size);
	io_close(io);
	return sb2str(sb);
}

REFTYPE(md_reftype, "MD", md_ident, md_admin);


mdmat_t *new_mdmat(void)
{
	return rd_create(&md_reftype);
}

/*	Datenmatrix kopieren
*/

static void do_copy(mdaxis_t *y, mdaxis_t *x, size_t size, char *py, char *px, unsigned mask)
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


mdmat_t *cpy_mdmat(const mdmat_t *md, unsigned mask)
{
	mdmat_t *m2;
	mdaxis_t *x, **ptr;

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

	m2->size = md_size(m2->axis, m2->type->size);
	m2->data = memalloc(m2->size);
	do_copy(m2->axis, md->axis, m2->type->size, m2->data, md->data, mask);
	return m2;
}
