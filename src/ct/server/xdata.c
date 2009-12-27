#include "CubeHandle.h"
#include <EFEU/printobj.h>

#define	PRINT_LOCK	(MDFLAG_LOCK|MDFLAG_TEMP)

static int xwalk (CubeHandle *handle, mdaxis *x, char *data, char *base)
{
	char *ptr;
	size_t i, lpos;
	int flag;
	int stat, show;

	if	(!x)
	{
		io_puts(label_get(), handle->out);
		PrintData(handle->out, handle->md->type, data);
		io_putc(';', handle->out);

		if	(base)
			PrintData(handle->out, handle->md->type, base);

		io_putc('\n', handle->out);
		return io_err(handle->out);
	}

	show = !(x->flags & MDXFLAG_HIDE);
	lpos = label_pos();

	if	(base)
	{
		ptr = base;
		flag = 1;
	}
	else
	{
		ptr = data;
		flag = 0;
	}

	for (i = 0; i < x->dim; i++)
	{
		if	(x->idx[i].flags & MDFLAG_BASE)
		{
			base = ptr + i * x->size;
			flag = 0;
			break;
		}
	}

	for (stat = 0, i = 0; i < x->dim && !stat; i++)
	{
		if	(x->idx[i].flags & (MDFLAG_LOCK|MDFLAG_TEMP))
			continue;

		if	(show)
			label_xadd(lpos, x, i);

		if	(flag || (x->idx[i].flags & MDFLAG_BASE))
			base = ptr + i * x->size;

		stat = xwalk(handle, x->next, data + i * x->size, base);
	}

	return stat;
}

void CubeHandle_xdata (CubeHandle *handle, char *arg)
{
	md_setflag(handle->md, arg, 0, NULL, 0, mdsf_lock, MDFLAG_TEMP);
	CubeHandle_head(handle, 1);
	label_beg();
	xwalk(handle, handle->md->axis, handle->md->data, NULL);
	md_allflag(handle->md, 0, NULL, 0, mdsf_clear, MDFLAG_TEMP);
}
