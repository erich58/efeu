/*	Strings laden
	(c) 1995 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/efio.h>

static char *substring(io_t *io, int delim)
{
	char *p;
	char buf[2];

	buf[0] = delim;
	buf[1] = 0;
	io_getc(io);
	p = io_xgets(io, buf);
	io_getc(io);
	return p;
}

static char *loadstring(io_t *io, int delim)
{
	int no_nl;
	int at_begin;
	strbuf_t *buf;
	int c;

	io_getc(io);
	c = io_mgetc(io, 1);

	if	(c == delim)	return NULL;

	io_protect(io, 1);
	buf = new_strbuf(0);
	at_begin = 1;
	no_nl = 1;

	if	(c == '\n')
	{
		no_nl = 0;
		c = io_mgetc(io, 1);
	}

	while (c != EOF)
	{
		if	(at_begin)
		{
			if	(c == '\\')
			{
				c = io_mgetc(io, 1);

				if	(c != delim)
					sb_putc('\\', buf); 
			}
			else if	(c == delim)
			{
				break;
			}
		}

		sb_putc(c, buf);
		at_begin = (no_nl || c == '\n');
		c = io_mgetc(io, 1);
	}

	io_protect(io, 0);
	return sb2str(buf);
}


char *getstring(io_t *io)
{
	int c;

	switch (c = io_eat(io, " \t"))
	{
	case '\n':	io_getc(io);
	case EOF:	return NULL;
	case '(':	return loadstring(io, ')');
	case '{':	return loadstring(io, '}');
	case '[':	return loadstring(io, ']');
	case '<':	return loadstring(io, '>');
	case '`':	return loadstring(io, '\'');
	case '\'':
	case '"':	return substring(io, c);
	case '/':
	case '!':
	case '|':	return loadstring(io, c);
	default:	return io_mgets(io, ";\n");
	}
}
