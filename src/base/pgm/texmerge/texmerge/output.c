/*	Ausgabestruktur für TexMerge
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#include "texmerge.h"


static void file_put(int c, Output_t *out)
{
	putc(c, (FILE *) out->data);
}

static void file_puts(const char *str, Output_t *out)
{
	fputs(str, (FILE *) out->data);
}

static void ofile_close(Output_t *out)
{
	fclose((FILE *) out->data);
}

static void str_put(int c, Output_t *out)
{
	buf_putc(c, (Buffer_t *) out->data);
}

static void str_puts(const char *str, Output_t *out)
{
	buf_puts(str, (Buffer_t *) out->data);
}


/*	Ausgabestruktur generieren
*/

Output_t *CreateOutput(char *name, FILE *file, Buffer_t *buf)
{
	Output_t *out;

	out = AllocData("OUTPUT", sizeof(Output_t));
	out->name = name;
	out->pos = 0;
	out->mode = 0;
	out->data = NULL;
	out->put = NULL;
	out->puts = NULL;
	out->close = NULL;

	if	(file)
	{
		out->data = file;
		out->put = file_put;
		out->puts = file_puts;
		out->close = ofile_close;
	}
	else if	(buf)
	{
		out->data = buf;
		out->put = str_put;
		out->puts = str_puts;
	}
	return out;
}


/*	Ausgabestruktur schließen
*/

void CloseOutput(Output_t *out)
{
	if	(out->close)
		out->close(out);

	FreeData("STR", out->name);
	FreeData("OUTPUT", out);
}


Output_t *OpenDummy(void)
{
	return CreateOutput(NULL, NULL, NULL);
}

Output_t *OpenStdout(void)
{
	return CreateOutput(CopyString("<stdout>"), stdout, NULL);
}


/*	Texfile öffnen
*/

#define	HDR	"%% Nicht editieren, Datei wurde mit %s generiert.\n"

Output_t *OpenTexfile(const char *name)
{
	char *fname;
	FILE *file;

	if	(name == NULL)	return OpenStdout();

	fname = FileName(name, TEX_EXT);
	file = fopen(fname, "w");

	if	(file == NULL)
	{
		Error(12, fname);
	}
	else	fprintf(file, HDR, ProgramName);

	return CreateOutput(fname, file, NULL);
}


/*	Ausgabemodus verändern
*/

int set_mode (Output_t *out, int mode)
{
	if	(out)
	{
		int save = out->mode;
		out->mode = mode;
		return save;
	}
	else	return 0;
}


/*	Zeichen ausgeben
*/

int out_putc(int c, Output_t *out)
{
	if	(out->put && c != EOF)
	{
		if	(c == '\n')
		{
			out->put(c, out);
			out->pos = 0;
		}
		else if	(c == '\t')
		{
			int n;

			n = 8 * (out->pos / 8 + 1);

			if	(out->mode & OUT_EXPAND)
			{
				while (out->pos < n)
				{
					out->put(' ', out);
					out->pos++;
				}
			}
			else	out->put(c, out);

			out->pos = n;
		}
		else
		{
			if	(out->mode & OUT_TEXMODE)
			{
				tex_putc(c, out);
			}
			else if	(out->mode & OUT_STRMODE)
			{
				str_putc(c, out);
			}
			else	out->put(c, out);

			out->pos++;
		}
	}

	return 1;
}


void out_newline(Output_t *out)
{
	if	(out->put && out->pos != 0)
	{
		out->put('\n', out);
		out->pos = 0;
	}
}


/*	String ausgeben
*/

void out_puts(const char *str, Output_t *out)
{
	if	(str == NULL || out->put == NULL)
		return;

	for (; *str != 0; str++)
		out_putc(*str, out);
}


/*	String formatiert ausgeben
*/

void out_fputs(const char *str, Output_t *output, const char *fmt)
{
	char buf[512];
	char varfmt[4];
	int key, save;

	save = set_mode(output, 0);
	key = fmt ? fmt[0] : 0;

	switch (key)
	{
	case 's':

		if	(str != NULL)
		{
			out_putc('"', output);
			set_mode(output, OUT_STRMODE);
			out_puts(str, output);
			set_mode(output, 0);
			out_putc('"', output);
		}
		else	out_puts("NULL", output);

		break;
	case 't':
		set_mode(output, OUT_TEXMODE|OUT_EXPAND);
		out_puts(str, output);
		break;
	case ' ':
		set_mode(output, OUT_EXPAND);
		out_puts(str, output);
		break;
	case 'v':
		out_newline(output);
		out_puts("\\begin{verbatim}\n", output);
		set_mode(output, OUT_EXPAND);
		out_puts(str, output);
		out_newline(output);
		set_mode(output, 0);
		out_puts("\\end{verbatim}\n", output);
		break;
	case 'c':
		out_putc(str ? str[0] : 0, output);
		break;
	case 'i':
	case 'd':
	case 'o':
	case 'x':
	case 'X':
		sprintf(varfmt, "%%#%c", key);
		sprintf(buf, varfmt, str ? atoi(str) : 0);
		out_puts(buf, output);
		break;
	case 'e':
	case 'E':
	case 'f':
	case 'F':
	case 'g':
	case 'G':
		sprintf(varfmt, "%%#%c", key);
		sprintf(buf, varfmt, str ? atof(str) : 0.);
		out_puts(buf, output);
		break;
	default:
		out_puts(str, output);
		break;
	}

	set_mode(output, save);
}

#if	UNIX_VERSION

io_t *io_Output(Output_t *output)
{
	io_t *io;

	if	(output->put == NULL)	return NULL;

	io = io_alloc();
	io->put = (io_put_t) out_putc;
	io->data = output;
	return io;
}

#endif
