/*	Fehlermeldungen generieren
	(c) 1997 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/ftools.h>
#include <EFEU/procenv.h>

char *MessageHandler = "PrintMessage";

void message(const char *id, const char *name, int num, int narg, ...)
{
	va_list list;

	list = va_start(list, narg);
	vmessage(id, name, num, narg, list);
	va_end(list);
}

static void save_str(FILE *file, const char *str)
{
	if	(str == NULL)
	{
		putc('-', file);
		return;
	}

	if	(*str == '-')
		putc('\\', file);

	while (*str != 0)
	{
		switch (*str)
		{
		case '\\':
		case ' ':
		case '\t':
		case '\n':
			putc('\\', file);
			break;
		default:
			break;
		}

		putc(*str, file);
		str++;
	}
}

void vmessage(const char *id, const char *name, int num, int narg, va_list list)
{
	FILE *file;
	int i;

	file = MessageHandler ? popen(MessageHandler, "w") : NULL;

	if	(file == NULL)
		file = stderr;

	if	(ProgIdent)
	{
		save_str(file, ProgIdent);
		putc(':', file);
		putc(' ', file);
	}

	fprintf(file, "ERROR ");

	if	(name == NULL)	name = ProgName;

	if	(name)
	{
		save_str(file, name);
		putc(':', file);
	}

	fprintf(file, "%d\t", num);
	save_str(file, id);

	for (i = 0; i < narg; i++)
	{
		putc('\t', file);
		save_str(file, va_arg(list, char *));
	}

	putc('\n', file);
	fflush(file);

	if	(file != stderr)
		pclose(file);
}
