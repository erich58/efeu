#include "CubeHandle.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <ctype.h>
#include <EFEU/parsearg.h>
#include <EFEU/ioctrl.h>

#define	MIN_LINE	1
#define	MAX_LINE	10000

#define	MIN_BYTE	1024
#define	MAX_BYTE	32768

#define	E_LINE	"Line to long.\n"

char *test_key (const char *key, char *cmd)
{
	int flag;

	if	(!cmd)	return NULL;

	flag = 0;

	while (*key && *key != ']')
	{
		if	(*key == '[')
		{
			flag = 1;
			key++;
		}
		else if	(*key == *cmd)
		{
			key++;
			cmd++;
		}
		else if	(flag)
		{
			break;
		}
		else	return 0;
	}

	if	(*cmd == 0)	return cmd;

	if	(isspace((unsigned char) *cmd))
	{
		do	cmd++;
		while (isspace((unsigned char) *cmd));

		return cmd;
	}
	else	return NULL;
}


static int test_break (CubeHandle *handle)
{
	char *p = CubeHandle_read(handle);

	if	(p == NULL)	return 1;
	if	(*p == 0)	return 0;

	if	((p = test_key("n[ext]", p)))
	{
		if	(*p)
		{
			handle->max_lines = strtoul(p, NULL, 0);

			if	(handle->max_lines == 0)
				handle->max_lines = MAX_LINE;
			else if	(handle->max_lines > MAX_LINE)
				handle->max_lines = MAX_LINE;
			else if	(handle->max_lines < MIN_LINE)
				handle->max_lines = MIN_LINE;

			handle->max_lines++;
		}
		else	handle->max_lines = handle->save_limit;

		return 0;
	}

	CubeHandle_unread(handle);
	return 1;
}

static int out_put (int c, void *ptr)
{
	CubeHandle *handle = ptr;

	if	(handle->out_eof)	return EOF;

	if	(handle->buf_out->pos >= handle->max_byte ||
			handle->line >= handle->max_lines)
	{
		size_t n;

		if	(handle->line <= 1)
		{
			CubeHandle_error(handle, E_LINE);
			CubeHandle_send(handle);
			handle->out_eof = 1;
			return EOF;
		}

		memcpy(handle->buf_out->data, handle->seg, 3);
		sprintf((char *) handle->buf_out->data + 3,
			"%6d%6d", (unsigned) handle->line - 1,
			(unsigned) handle->last_pos - 16);
		handle->buf_out->data[15] = '\n';
		send(handle->fd, handle->buf_out->data, handle->last_pos, 0);

		if	(test_break(handle))
		{
			handle->out_eof = 1;
			return EOF;
		}

		memcpy(handle->buf_out->data, handle->key, 3);
		sprintf((char *) handle->buf_out->data + 3,
			"%6d%6d\n", 0, 0);
		n = handle->buf_out->pos - handle->last_pos;
		memmove(handle->buf_out->data + 16,
			handle->buf_out->data + handle->last_pos, n);
		handle->line = 1;
		handle->last_pos = 16;
		handle->buf_out->pos = 16 + n;
		sb_sync(handle->buf_out);
	}

	sb_putc(c, handle->buf_out);

	if	(c == '\n')
	{
		handle->line++;
		handle->last_pos = sb_getpos(handle->buf_out);
	}

	return c;
}

static int out_ctrl (void *ptr, int req, va_list list)
{
	CubeHandle *handle = ptr;

	switch (req)
	{
	case IO_REWIND:
		sb_begin(handle->buf_out);
		sb_sync(handle->buf_out);
		return 0;
	case IO_CLOSE:
		return 0;
	default:
		return EOF;
	}
}

void CubeHandle_error (CubeHandle *handle, const char *fmt, ...)
{
	va_list list;

	va_start(list, fmt);
	CubeHandle_beg(handle, "ERR", NULL);
	io_vprintf(handle->out, fmt, list);
	va_end(list);
}

CubeHandle *CubeHandle_init (CubeHandle *handle, int fd)
{
	static CubeHandle buf;

	if	(!handle)
		handle = &buf;

	handle->fd = fd;
	handle->stat = 0;
	handle->max_byte = MAX_BYTE;
	handle->max_lines = MAX_LINE;
	handle->save_limit = handle->max_lines;
	handle->headline = 1;
	handle->md = NULL;
	handle->buf_in = sb_create(1000);
	handle->buf_out = sb_create(1000);
	handle->out = io_alloc();
	handle->out->data = handle;
	handle->out->put = out_put;
	handle->out->ctrl = out_ctrl;
	return handle;
}

void CubeHandle_close (CubeHandle *handle)
{
	handle->stat = 0;
	rd_deref(handle->md);
	rd_deref(handle->buf_in);
	rd_deref(handle->out);
	rd_deref(handle->buf_out);
}

char *CubeHandle_read (CubeHandle *handle)
{
	unsigned char *p;

	if	(handle->stat == EOF)	return NULL;

	if	(handle->stat == 0)
	{
		if	(!sb_read(handle->fd, handle->buf_in))
		{
			handle->stat = EOF;
			return NULL;
		}
	}

	handle->stat = 0;
	p = handle->buf_in->data;

	while (isspace(*p))
		p++;

	return (char *) p;
}

void CubeHandle_unread (CubeHandle *handle)
{
	if	(handle->stat == 0)	handle->stat = 1;
}

void CubeHandle_beg (CubeHandle *handle, const char *key, const char *seg)
{
	handle->line = 0;
	handle->last_pos = 0;
	handle->out_eof = 0;
	handle->key = key;
	handle->seg = seg ? seg : key;
	handle->max_lines = handle->save_limit;
	io_rewind(handle->out);
	io_printf(handle->out, "%3.3s%6d%6d\n", handle->key, 0, 0);
}

void CubeHandle_send (CubeHandle *handle)
{
	if	(handle->out_eof)	return;

	sprintf((char *) handle->buf_out->data + 3,
		"%6d%6d", (unsigned) handle->line - 1,
		(unsigned) handle->buf_out->pos - 16);
	handle->buf_out->data[15] = '\n';
	send(handle->fd, handle->buf_out->data, handle->buf_out->pos, 0);
}

void CubeHandle_set (CubeHandle *handle, char *arg)
{
	AssignArg *x;
	char *p;

	while ((x = assignarg(arg, &p, " ,;\t\n")))
	{
		if	(test_key("l[ines]", x->name))
		{
			handle->max_lines = strtoul(x->arg, NULL, 0);

			if	(handle->max_lines == 0)
				handle->max_lines = MAX_LINE;
			else if	(handle->max_lines > MAX_LINE)
				handle->max_lines = MAX_LINE;
			else if	(handle->max_lines < MIN_LINE)
				handle->max_lines = MIN_LINE;

			handle->max_lines++;
		}
		else if	(test_key("b[ytes]", x->name))
		{
			handle->max_byte = strtoul(x->arg, NULL, 0);

			if	(handle->max_byte == 0)
				handle->max_byte = MAX_BYTE;
			else if	(handle->max_byte > MAX_BYTE)
				handle->max_byte = MAX_BYTE;
			else if	(handle->max_byte < MIN_BYTE)
				handle->max_byte = MIN_BYTE;
		}
		else if	(test_key("h[eadline]", x->name))
		{
			handle->headline = !!strtoul(x->arg, NULL, 0);
		}

		arg = p;
		memfree(x);
	}

	handle->save_limit = handle->max_lines;

	if	(handle->headline)
		io_puts("#par:value;min;max\n", handle->out);
	io_printf(handle->out, "lines;%u;%u;%u\n",
		(unsigned) handle->max_lines, MIN_LINE, MAX_LINE);
	io_printf(handle->out, "bytes;%u;%u;%u\n",
		(unsigned) handle->max_byte, MIN_BYTE, MAX_BYTE);
	io_printf(handle->out, "headline;%d;0;1\n", handle->headline);
}
