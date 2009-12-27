/*	Leerzeichen und Weiße Zeichen überlesen
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include <efeudoc.h>
#include <ctype.h>

int DocSkipSpace (io_t *in, int flag)
{
	int c;

	do	c = io_skipcom(in, NULL, 0);
	while	(c == ' ' || c == '\t');

	if	(!(flag && c == '\n'))
		io_ungetc(c, in);

	return c;
}

int DocSkipWhite (io_t *in)
{
	int c;

	do	c = io_skipcom(in, NULL, 1);
	while	(isspace(c));

	io_ungetc(c, in);
	return c;
}
