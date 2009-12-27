/*	Eingabestruktur für TexMerge
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#include "texmerge.h"

/*	Eingabestruktur generieren
*/

static int file_get(Input_t *in)
{
	return getc((FILE *) in->data);
}

static void file_close(Input_t *in)
{
	fclose((FILE *) in->data);
}

static void file_ident(Input_t *in)
{
	fprintf(stderr, "File \"%s\"", in->name ? in->name : "");
}

static int str_get(Input_t *in)
{
	char *p = (char *) in->data;

	if	(*p == 0)
		return EOF;

	in->data = p + 1;
	return *p;
}

static void str_ident(Input_t *in)
{
	char *p;
	int i;

	p = in->data;
	fprintf(stderr, "String ...");

	for (i = 0; i < 10 && p[i] != 0; i++)
	{
		if	(p[i] < ' ' || p[i] >= 127)
			putc('.', stderr);
		else	putc(p[i], stderr);
	}

	fprintf(stderr, "...");
}

Input_t *CreateInput(char *name, FILE *file, const char *str)
{
	Input_t *in;

	in = AllocData("INPUT", sizeof(Input_t));
	in->name = name;
	in->nl = 1;
	in->line = 0;
	in->save = EOF;
	in->eof = 0;

	if	(file)
	{
		in->data = file;
		in->get = file_get;
		in->close = file_close;
		in->ident = file_ident;
	}
	else if	(str)
	{
		in->data = (void *) str;
		in->get = str_get;
		in->close = NULL;
		in->ident = str_ident;
	}
	else
	{
		in->data = NULL;
		in->get = NULL;
		in->close = NULL;
		in->ident = NULL;
	}

	return in;
}


static int interact_get(Input_t *in)
{
#if	UNIX_VERSION
	return io_getc(in->data);
#else
	if	(in->nl)
		printf("%s[%d]: ", ProgramName, in->line);

	return getchar();
#endif
}

static void interact_close(Input_t *in)
{
#if	UNIX_VERSION
	if	(in->save != EOF)
		io_ungetc(in->save, in->data);
	io_close(in->data);
#else
	putchar('\n');
#endif
}

Input_t *OpenInteractive(void)
{
	Input_t *in;

	in = CreateInput(CopyString("<stdin>"), NULL, NULL);
#if	UNIX_VERSION
	in->data = io_readline(NULL, "~/.tm_history");
#else
	in->data = NULL;
#endif
	in->get = interact_get;
	in->close = interact_close;
	in->ident = file_ident;
	return in;
}

#if	UNIX_VERSION
Input_t *OpenIOInput(io_t *io)
{
	Input_t *in;

	in = CreateInput(CopyString("<io>"), NULL, NULL);
	in->data = io_refer(io);
	in->get = interact_get;
	in->close = interact_close;
	in->ident = file_ident;
	return in;
}
#endif

/*	Eingabestruktur schließen
*/

void CloseInput(Input_t *in)
{
	if	(in->close)
		in->close(in);

	FreeData("STR", in->name);
	FreeData("INPUT", in);
}


/*	Zeichen lesen
*/

int in_getc(Input_t *in)
{
	int c;

	if	(in == NULL || in->get == NULL || in->eof)
		return EOF;

	if	(in->save != EOF)
	{
		c = in->save;
		in->save = EOF;
		return c;
	}

	in->line += in->nl;
	c = in->get(in);

/*
	if	(in->nl)
	{
		fprintf(stderr, "%s %d:\t", in->name ? in->name : "<noname>",
			in->line);
	}

	if	(c != EOF)
		putc(c, stderr);
*/

	in->nl = (c == '\n' ? 1 : 0);
	in->eof = (c == EOF ? 1 : 0);
	return c;
}

void in_skipline(Input_t *in)
{
	if	(in && in->get)
	{
		while (in->eof == 0 && in->nl == 0)
			in_getc(in);
	}
}

/*	Zeichen zurückschreiben
*/

void in_ungetc(int c, Input_t *in)
{
	if	(in == NULL || in->eof || c == EOF)
		return;

	if	(in->save == EOF)
		in->save = c;
	else	InputError(in, 32, c);
}


void InputError (Input_t *in, int num, ...)
{
	va_list args;

	if	(in->ident)
	{
		in->ident(in);
	}
	else	fprintf(stderr, "Eingabe");

	fprintf(stderr, ", Zeile %d: ", in->line);
	va_start(args, num);
	PrintError(num, args);
	va_end(args);
}


/*	Eingabestruktur öffnen
*/

Input_t *OpenTemplate(const char *name)
{
	char *fname;
	FILE *file;

	fname = FileName(name, TPL_EXT);
	file = fopen(fname, "r");

	if	(file == NULL)
		Error(11, fname);

	return CreateInput(fname, file, NULL);
}
