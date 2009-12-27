/*	Ausgabezeilen einrücken
	(c) 1998 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/io.h>
#include <EFEU/ioctrl.h>
#include <EFEU/mstring.h>


typedef struct {
	io_t *io;	/* Ausgabestruktur */
	int c;		/* Einrückungszeichen */
	int n;		/* Multiplikator */
	int depth;	/* Einrücktiefe */
	int newline;	/* Flag für neue Zeile */
} INDENT;

static int indent_put(int c, INDENT *indent)
{
	if	(indent->newline && c != '\n')
	{
		io_nputc(indent->c, indent->io, indent->n * indent->depth);
		indent->newline = 0;
	}

	indent->newline = (c == '\n');
	return io_putc(c, indent->io);
}

static int indent_ctrl(INDENT *indent, int req, va_list list)
{
	size_t stat;

	switch (req)
	{
	case IO_CLOSE:

		stat = io_close(indent->io);
		memfree(indent);
		break;

	case IO_SUBMODE:
		
		if	(va_arg(list, int))	indent->depth++;
		else if	(indent->depth > 0)	indent->depth--;

		return indent->depth;

	case IO_REWIND:

		if	(io_rewind(indent->io) == EOF)
			return EOF;

		indent->depth = 0;
		indent->newline = 1;
		stat = 0;
		break;

	case IO_IDENT:

		*va_arg(list, char **) = io_xident(indent->io, "indent(%*)");
		return 0;

	default:

		stat = io_vctrl(indent->io, req, list);
		break;
	}

	return stat;
}


io_t *io_indent (io_t *io, int c, int n)
{
	INDENT *indent = memalloc(sizeof(INDENT));
	indent->io = io;
	indent->c = c;
	indent->n = n;
	indent->depth = 0;
	indent->newline = 1;
	io = io_alloc();
	io->put = (io_put_t) indent_put;
	io->ctrl = (io_ctrl_t) indent_ctrl;
	io->data = indent;
	return io;
}
