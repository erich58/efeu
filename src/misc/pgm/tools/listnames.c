#include <EFEU/Resource.h>
#include <EFEU/strbuf.h>
#include <EFEU/io.h>
#include <ctype.h>

static int check (const char *name)
{
	char *p = strrchr(name, '_');

	if	(p == NULL)	return 0;

	if	(strcmp(p, "_t") != 0)	return 0;
	if	(strcmp(name, "size_t") == 0)	return 0;
	if	(strcmp(name, "time_t") == 0)	return 0;
	if	(strcmp(name, "off_t") == 0)	return 0;
	if	(strcmp(name, "mode_t") == 0)	return 0;
	if	(strcmp(name, "regex_t") == 0)	return 0;
	if	(strcmp(name, "regmatch_t") == 0)	return 0;

	if	(strcmp(name, "Type_t") == 0)	return 0;
	if	(strcmp(name, "Lval_t") == 0)	return 0;
	if	(strcmp(name, "List_t") == 0)	return 0;
	if	(strcmp(name, "Expr_t") == 0)	return 0;

	return 1;
}

static void eval (StrBuf *buf, const char *name)
{
	IO *io;
	int c;
	int line;
	
	io = io_fileopen(name, "rz");
	sb_setpos(buf, 0);
	line = 1;

	while ((c = io_getc(io)) != EOF)
	{
		if	(isalnum(c) || c == '_')
		{
			sb_putc(c, buf);
			continue;
		}

		if	(sb_getpos(buf))
		{
			sb_putc(0, buf);

			if	(check((char *) buf->data))
				printf("%s:%d: %s\n", name, line,
					(char *) buf->data);
			sb_begin(buf);
		}

		if	(c == '\n')	line++;
	}

	io_close(io);
}

int main (int argc, char **argv)
{
	int i;
	StrBuf *buf;

	ParseCommand(&argc, argv);
	buf = sb_create(1024);

	for (i = 1; i < argc; i++)
		eval(buf, argv[i]);

	sb_destroy(buf);
	return EXIT_SUCCESS;
}
