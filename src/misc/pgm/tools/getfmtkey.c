#include <EFEU/Resource.h>
#include <EFEU/strbuf.h>
#include <EFEU/io.h>
#include <ctype.h>


static SB_DECL(buf, 0);

static void add_char (int c)
{
	switch (c)
	{
	case '\\':	sb_puts("\\\\", &buf); break;
	case '\n':	sb_puts("\\n", &buf); break;
	case '\t':	sb_puts("\\t", &buf); break;
	default:	sb_putc(c, &buf); break;
	}
}

static void eval (const char *name)
{
	IO *io;
	int c;
	int stat;
	
	io = io_fileopen(name, "rz");
	stat = 0;

	while ((c = io_getc(io)) != EOF)
	{
		if	(c == '[')
		{
			sb_trunc(&buf);
			sb_putc(c, &buf);
			stat = 1;
		}
		else if	(!stat)
		{
			;
		}
		else if	(stat > 1 && c == ']')
		{
			sb_putc(c, &buf);

			while ((c = io_getc(io)) && c != EOF)
				add_char(c);

			sb_putc('\n', &buf);
			sb_putc(0, &buf);
			io_puts((char *) buf.data, iostd);
			stat = 0;
		}
		else if	(isalnum(c))
		{
			stat = 2;
			add_char(c);
		}
		else if	(c == '_' || c == ':')
		{
			add_char(c);
		}
		else	stat = 0;
	}

	io_close(io);
}

int main (int argc, char **argv)
{
	int i;

	ParseCommand(&argc, argv);

	for (i = 1; i < argc; i++)
		eval(argv[i]);

	return EXIT_SUCCESS;
}
