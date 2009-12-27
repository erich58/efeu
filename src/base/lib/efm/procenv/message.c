/*
Fehlermeldungen generieren

$Copyright (C) 1997 Erich Frühstück
This file is part of EFEU.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; see the file COPYING.Library.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <EFEU/ftools.h>
#include <EFEU/procenv.h>

char *MessageHandler = "PrintMessage";

void message(const char *id, const char *name, int num, int narg, ...)
{
	va_list list;

	va_start(list, narg);
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
