#include "CubeHandle.h"
#include <EFEU/printobj.h>

#define	PRINT_LOCK	(MDFLAG_LOCK|MDFLAG_TEMP)

static int twalk (CubeHandle *handle, mdaxis *x,
	char *data, char *base, int lag)
{
	char *ptr;
	int i;
	size_t lpos;
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

	flag = (x->flags & MDXFLAG_TIME);

	for (stat = 0, i = 0; i < x->dim && !stat; i++)
	{
		if	(x->idx[i].flags & (MDFLAG_LOCK|MDFLAG_TEMP))
			continue;

		if	(show)
			label_xadd(lpos, x, i);

		ptr = NULL;

		if	(flag)
		{
			int k = i - lag;

			if	(k >= 0 && k < x->dim)
				ptr = data + k * x->size;
		}
		else if	(base)
		{
			ptr = base + i * x->size;
		}

		stat = twalk(handle, x->next, data + i * x->size, ptr, lag);
	}

	return stat;
}

void CubeHandle_tdata (CubeHandle *handle, char *arg)
{
	int lag = arg ? strtol(arg, &arg, 0) : 0;
	md_setflag(handle->md, trim_arg(arg), 0, NULL, 0,
		mdsf_lock, MDFLAG_TEMP);
	CubeHandle_head(handle, 1);
	label_beg();
	twalk(handle, handle->md->axis, handle->md->data, NULL, lag);
	md_allflag(handle->md, 0, NULL, 0, mdsf_clear, MDFLAG_TEMP);
}
