/*	Steuerbefehle abfragen
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#include "eisdoc.h"
#include <ctype.h>

int ParseArg (char **ptr, int beg, int end)
{
	strbuf_t *sb;
	Merge_t save;
	int ignorespace;
	int c;

	if	(beg > 0)
	{
		c = io_getc(MergeStat.in);

		if	(c != beg)
		{
			io_ungetc(c, MergeStat.in);
			return 0;
		}

		if	(end <= 0)	beg = 0;
	}
	else if	(beg == 0)
	{
		do	c = io_getc(MergeStat.in);
		while	(c == ' ' || c == '\t');

		if	(c == EOF)	return 0;

		io_ungetc(c, MergeStat.in);
	}

	ignorespace = 0;

	if	(beg > 0)
	{
		switch ((c = io_getc(MergeStat.in)))
		{
		case EOF:	return 0;
		case '\n':	ignorespace = 1; break;
		default:	io_ungetc(c, MergeStat.in); break;
		}
	}

	PushMerge(&save);
	MergeFilter(NULL);
	MergeStat.ignorespace = ignorespace;

	if	(ptr)
	{
		sb = new_strbuf(0);
		MergeOutput(io_strbuf(sb));
	}
	else	MergeOutput(NULL);

	MergeEval(beg, end);
	PopMerge(&save);

	if	(ptr)	*ptr = sb2str(sb);

	return 1;
}

char *ParseName (void)
{
	strbuf_t *sb;
	int c;

	c = io_getc(MergeStat.in);

	if	(!(isalpha(c) || c == '_'))
	{
		io_ungetc(c, MergeStat.in);
		return NULL;
	}

	sb = new_strbuf(0);

	do
	{
		sb_putc(c, sb);
		c = io_getc(MergeStat.in);
	}
	while (isalnum(c) || c == '_');

	io_ungetc(c, MergeStat.in);
	return sb2str(sb);
}
