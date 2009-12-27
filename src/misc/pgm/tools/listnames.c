#include <EFEU/Resource.h>
#include <EFEU/strbuf.h>
#include <EFEU/io.h>
#include <ctype.h>

static char *tab[] = {
	"size_t", "ssize_t", "ptrdiff_t",
	"time_t",
	"off_t",
	"mode_t",
	"regex_t",
	"regmatch_t",
	"wchar_t",

	"int8_t", "int16_t", "int32_t", "int64_t", "intmax_t",
	"uint8_t", "uint16_t", "uint32_t", "uint64_t", "uintmax_t",

#if	1
	"Type_t",
	"Lval_t",
	"List_t",
	"Expr_t",
#endif
};

static int check (const char *name)
{
	char *p = strrchr(name, '_');
	int i;

	if	(p == NULL)	return 0;
	if	(strcmp(p, "_t") != 0)	return 0;

	for (i = 0; i < tabsize(tab); i++)
		if (strcmp(name, tab[i]) == 0) return 0;

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
