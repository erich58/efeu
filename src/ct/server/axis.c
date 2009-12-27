#include "CubeHandle.h"
#include <EFEU/MatchPar.h>
#include <ctype.h>

static void put_xflag (int flags, IO *out)
{
	if	(flags & MDXFLAG_MARK)	io_putc('C', out);
	if	(flags & MDXFLAG_TIME)	io_putc('T', out);
	if	(flags & MDXFLAG_HIDE)	io_putc('H', out);
}

static void put_flag (int flags, IO *out)
{
	if	(flags & MDFLAG_LOCK)	io_putc('L', out);
	if	(flags & MDFLAG_BASE)	io_putc('B', out);
	if	(flags & MDFLAG_HIDE)	io_putc('H', out);
}

void CubeHandle_axis (CubeHandle *handle, char *arg)
{
	mdaxis *x;

	if	(handle->headline)
		io_puts("#name;dim;flags;desc\n", handle->out);

	for (x = handle->md->axis; x; x = x->next)
	{
		CubeHandle_stridx(handle, x->sbuf, x->i_name);
		io_putc(';', handle->out);
		io_printf(handle->out, "%d;", x->dim);
		put_xflag(x->flags, handle->out);
		io_putc(';', handle->out);
		CubeHandle_string(handle, mdx_head(x));
		io_putc('\n', handle->out);
	}
}

void CubeHandle_index (CubeHandle *handle, char *arg)
{
	mdaxis *x;
	mdindex *idx;
	int n, k;
	MatchPar *mp;

	if	(handle->headline)
		io_puts("#axis;name;flags;desc\n", handle->out);

	if	(arg)
	{
		char *p;

		for (p = arg; *p; p++)
			if (isspace((unsigned char) *p)) *p = ',';
	}

	mp = MatchPar_create(arg, md_dim(handle->md->axis));

	for (x = handle->md->axis, n = 1; x != NULL; x = x->next, n++)
	{
		if	(MatchPar_exec(mp, StrPool_get(x->sbuf, x->i_name), n))
		{
			for (idx = x->idx, k = 0; k < x->dim; k++, idx++)
			{
				CubeHandle_stridx(handle, x->sbuf, x->i_name);
				io_putc(';', handle->out);
				CubeHandle_stridx(handle, x->sbuf, idx->i_name);
				io_putc(';', handle->out);
				put_flag(idx->flags, handle->out);
				io_putc(';', handle->out);
				CubeHandle_string(handle, mdx_label(x, k));
				io_putc('\n', handle->out);
			}
		}
	}

	rd_deref(mp);
}
