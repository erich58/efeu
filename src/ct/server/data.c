#include "CubeHandle.h"
#include <EFEU/printobj.h>

#define	PRINT_LOCK	(MDFLAG_LOCK|MDFLAG_TEMP)

static int walk (CubeHandle *handle, mdaxis *x, char *ptr)
{
	size_t i, lpos;
	int stat, show;

	if	(!x)
	{
		io_puts(label_get(), handle->out);
		PrintData(handle->out, handle->md->type, ptr);
		io_putc('\n', handle->out);
		return io_err(handle->out);
	}

	show = !(x->flags & MDXFLAG_HIDE);
	lpos = label_pos();

	for (stat = 0, i = 0; i < x->dim && !stat; i++)
	{
		if	(x->idx[i].flags & (MDFLAG_LOCK|MDFLAG_TEMP))
			continue;

		if	(show)
			label_xadd(lpos, x, i);

		stat = walk(handle, x->next, ptr + i * x->size);
	}

	return stat;
}

void CubeHandle_data (CubeHandle *handle, char *arg)
{
	md_setflag(handle->md, arg, 0, NULL, 0, mdsf_lock, MDFLAG_TEMP);
	CubeHandle_head(handle, 0);
	label_beg();
	walk(handle, handle->md->axis, handle->md->data);
	md_allflag(handle->md, 0, NULL, 0, mdsf_clear, MDFLAG_TEMP);
}
