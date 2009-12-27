/*	Kommentare lesen/zwischenspeichern
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include <EFEU/io.h>

#define	PROMPT	"/* "

static void comment_line (io_t *io, strbuf_t *buf, int flag)
{
	int c, escape;

	do	c = io_getc(io);
	while	(c == ' ' || c == '\t');

	escape = 0;

	while (escape || c != '\n')
	{
		if	(c == EOF)	return;
		if	(c == '\n')	io_linemark(io);

		if	(buf)	sb_putc(c, buf);

		escape = (c == '\\') ? !escape : 0;
		c = io_getc(io);
	}
	
	if	(buf)	sb_putc(c, buf);

	if	(!flag)
		io_ungetc(c, io);
}

static void comment_cstyle (io_t *io, strbuf_t *buf, int flag)
{
	int indent, c, depth;
	char *prompt;

	if	(buf)
		sb_clear(buf);

	prompt = io_prompt(io, PROMPT);
	c = io_getc(io);

	for (depth = 0; c == '\t'; depth++)
		c = io_getc(io);

	while (c == '\n' && depth == 0)
		c = io_getc(io);

	indent = 0;

	while (c != EOF)
	{
		if	(c == '*')
		{
			if	(io_peek(io) == '/')
			{
				io_getc(io);
				break;
			}
		}

		if	(c == '\t' && indent)
		{
			indent--;
		}
		else
		{
			if	(c == '\n')
			{
				indent = depth;
			}
			else if	(indent)
			{
				depth -= indent;
				indent = 0;
			}

			if	(buf)
				sb_putc(c, buf);
		}

		c = io_getc(io);
	}

	io_prompt(io, prompt);

	if	(flag && io_peek(io) == '\n')
		io_getc(io);
}


int io_skipcom (io_t *io, strbuf_t *buf, int flag)
{
	int c;

	while ((c = io_getc(io)) == '/')
	{
		switch ((c = io_getc(io)))
		{
		case '/':	comment_line(io, buf, flag); break;
		case '*':	comment_cstyle(io, buf, flag); break;
		default:	io_ungetc(c, io); /* FALLTROUGH */
		case EOF:	return '/';
		}
	}

	return c;
}
