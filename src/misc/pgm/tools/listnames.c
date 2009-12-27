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

static SB_DECL(buf_name, 0);

static void eval (const char *name)
{
	IO *io;
	int c;
	int line;
	
	io = io_fileopen(name, "rz");
	sb_trunc(&buf_name);
	line = 1;

	while ((c = io_getc(io)) != EOF)
	{
		if	(isalnum(c) || c == '_')
		{
			sb_putc(c, &buf_name);
			continue;
		}

		if	(sb_getpos(&buf_name))
		{
			char *p = sb_nul(&buf_name);

			if	(check(p))
				printf("%s:%d: %s\n", name, line, p);
			sb_trunc(&buf_name);
		}

		if	(c == '\n')	line++;
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
