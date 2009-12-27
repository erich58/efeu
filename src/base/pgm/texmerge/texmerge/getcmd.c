/*	Steuerbefehle abfragen
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#include "texmerge.h"
#include <ctype.h>

int SkipSpace (Input_t *in)
{
	int c;

	do	c = in_getc(in);
	while (c == ' ' || c == '\t');

	in_ungetc(c, in);
	return c;
}

int GetArgStatus = 0;

char *GetArg (Input_t *in, int start, int end)
{
	Buffer_t *sb;
	int c, depth;

	if	(start != 0)
	{
		c = in_getc(in);

		if	(c != start)
		{
			in_ungetc(c, in);
			GetArgStatus = 0;
			return NULL;
		}
	}

	sb = CreateBuffer();
	depth = 0;

	while ((c = in_getc(in)) != EOF)
	{
		if	(c == '@')
		{
			MergeBuf(in, sb);
			continue;
		}
		else if	(end == 0 && isspace(c))
		{
			break;
		}
		else if	(c == end)
		{
			if	(depth <= 0)	break;

			depth--;
		}
		else if	(c == start)	depth++;

		buf_putc(c, sb);
	}

	GetArgStatus = 1;
	return CloseBuffer(sb);
}

char *GetName (Input_t *in, int c)
{
	Buffer_t *sb;
	char *name;

	sb = CreateBuffer();

	do
	{
		buf_putc(c, sb);
		c = in_getc(in);
	}
	while (isalnum(c) || c == '_');

	name = CloseBuffer(sb);

	if	(c == '=')
	{
		char *fmt;

		fmt = GetArg(in, 0, 0);
		AssignCommand(name, fmt);
		FreeData("STR", name);
		return NULL;
	}
	else	in_ungetc(c, in);

	return name;
}

char *GetCommand(Input_t *in, Output_t *out)
{
	int c;

	c = in_getc(in);

	if	(isalpha(c) || c == '_')
	{
		return GetName(in, c);
	}
	else if	(c == '!')
	{
		do	c = in_getc(in);
		while	(c != EOF && c != '\n');
	}
	else	out_putc(c, out);

	return NULL;
}
