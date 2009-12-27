/*	Zeichen überlesen
	(c) 1998 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#include "eisdoc.h"
#include <ctype.h>

void SkipLine (void)
{
	int c;

	do	c = io_getc(MergeStat.in);
	while	(c == ' ' || c == '\t');

	if	(c == '\n')
	{
		MergeStat.linestart = 1;
		return;
	}

	io_ungetc(c, MergeStat.in);
	MergeStat.linestart = 0;
}
