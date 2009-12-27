#include "CubeHandle.h"

static void ucs_xcopy (IO *in, IO *out)
{
	int32_t ucs;

	while ((ucs = io_getucs(in)) != EOF)
	{
		switch (ucs)
		{
		case '\b':	io_puts("\\b", out); break;
		case '\f':	io_puts("\\f", out); break;
		case '\n':	io_puts("\\n", out); break;
		case '\r':	io_puts("\\r", out); break;
		case '\t':	io_puts("\\t", out); break;
		case '\v':	io_puts("\\v", out); break;
		case '\\':	io_puts("\\\\", out); break;
		case '"':	io_puts("\\\"", out); break;
		default:

			if	(ucs < 0x20 || ucs == 0x7f)
			{
				io_printf(out, "\\%03o", ucs);
			}
			else	io_putucs_utf8(ucs, out);

			break;
		}
	}
}

static void ucs_copy (IO *in, IO *out)
{
	int32_t ucs;

	while ((ucs = io_getucs(in)) != EOF)
	{
		switch (ucs)
		{
		case '\t':
		case '\n':
			io_putc(ucs, out); break;
		default:

			if	(ucs < 0x20 || ucs == 0x7f)
			{
				io_printf(out, "\\%03o", ucs);
			}
			else	io_putucs_utf8(ucs, out);

			break;
		}
	}
}

void utf8_put (const char *str, IO *out)
{
	if	(str)
	{
		IO *in = io_cstr(str);
		ucs_copy(in, out);
		io_close(in);
	}
}

void utf8_save (const char *str, StrBuf *buf)
{
	if	(str)
	{
		IO *in = io_cstr(str);
		IO *out = io_strbuf(buf);
		ucs_xcopy(in, out);
		io_close(out);
		io_close(in);
	}
}

void CubeHandle_string (CubeHandle *handle, const char *str)
{
	if	(str)
	{
		IO *in = io_cstr(str);
		io_putc('"', handle->out);
		ucs_xcopy(in, handle->out);
		io_putc('"', handle->out);
		io_close(in);
	}
}

void CubeHandle_label (CubeHandle *handle, const char *str)
{
	if	(str)
	{
		IO *in = io_cstr(str);
		ucs_xcopy(in, handle->out);
		io_close(in);
	}
}

void CubeHandle_stridx (CubeHandle *handle, StrPool *sbuf, size_t pos)
{
	CubeHandle_string(handle, StrPool_get(sbuf, pos));
}
