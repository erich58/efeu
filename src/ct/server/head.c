#include "CubeHandle.h"
#include <EFEU/printobj.h>

void CubeHandle_head (CubeHandle *handle, int ext)
{
	mdaxis *x;

	if	(!handle->headline)
		return;

	io_putc('#', handle->out);

	for (x = handle->md->axis; x; x = x->next)
	{
		if	(x->flags & MDXFLAG_HIDE)
			continue;

		CubeHandle_stridx(handle, x->sbuf, x->i_name);
		io_putc(';', handle->out);
	}

	io_putc('"', handle->out);
	PrintType(handle->out, handle->md->type, 1);

	if	(ext)
	{
		io_puts(" data\";\"", handle->out);
		PrintType(handle->out, handle->md->type, 1);
		io_puts(" base\"\n", handle->out);
	}
	else	io_puts("\"\n", handle->out);
}
