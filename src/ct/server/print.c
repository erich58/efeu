#include "CubeHandle.h"
#include <EFEU/printobj.h>

#define	PRINT_LOCK	(MDFLAG_LOCK|MDFLAG_TEMP)

static int col_label (CubeHandle *handle, mdaxis *x)
{
	int stat = 0;

	while (x && !(x->flags & MDXFLAG_MARK))
		x = x->next;

	if	(x)
	{
		size_t i;
		size_t lpos;
		int show;

		show = !(x->flags & MDXFLAG_HIDE);
		lpos = label_pos();

		for (i = 0; i < x->dim && !stat; i++)
		{
			if	(x->idx[i].flags & PRINT_LOCK)
				continue;

			if	(show)
				label_add(lpos, x, i);

			stat = col_label(handle, x->next);
		}
	}
	else
	{
		io_putc(';', handle->out);
		CubeHandle_string(handle, label_get());
		stat = io_err(handle->out);
	}

	return stat;
}

static int c_walk (CubeHandle *handle, mdaxis *x, char *ptr)
{
	int stat = 0;

	while (x && !(x->flags & MDXFLAG_MARK))
		x = x->next;

	if	(x)
	{
		size_t i;

		for (i = 0; i < x->dim && !stat; i++)
		{
			if	(x->idx[i].flags & PRINT_LOCK)
				continue;

			stat = c_walk(handle, x->next, ptr + i * x->size);
		}
	}
	else
	{
		io_putc(';', handle->out);
		PrintData(handle->out, handle->md->type, ptr);
		return io_err(handle->out);
	}

	return stat;
}

static int l_walk (CubeHandle *handle, mdaxis *x, char *ptr)
{
	while (x && (x->flags & MDXFLAG_MARK))
		x = x->next;

	if	(x)
	{
		size_t i, lpos;
		int stat, show;

		show = !(x->flags & MDXFLAG_HIDE);
		lpos = label_pos();

		for (stat = 0, i = 0; i < x->dim && !stat; i++)
		{
			if	(x->idx[i].flags & MDFLAG_LOCK)
				continue;

			if	(show)
				label_add(lpos, x, i);

			stat = l_walk(handle, x->next, ptr + i * x->size);
		}

		return stat;
	}
	else
	{
		CubeHandle_string(handle, label_get());
		c_walk(handle, handle->md->axis, ptr);
		io_putc('\n', handle->out);
		return io_err(handle->out);
	}
}

void CubeHandle_print (CubeHandle *handle, char *arg)
{
	md_setflag(handle->md, arg, 0, NULL, 0, mdsf_lock, MDFLAG_TEMP);

	if	(handle->headline)
	{
		io_putc('#', handle->out);
		label_beg();
		col_label(handle, handle->md->axis);
		io_putc('\n', handle->out);
	}

	label_beg();
	l_walk(handle, handle->md->axis, handle->md->data);
	md_allflag(handle->md, 0, NULL, 0, mdsf_clear, MDFLAG_TEMP);
}
