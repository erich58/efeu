/*	Leerzeichen und Wei�e Zeichen �berlesen
	(c) 1999 Erich Fr�hst�ck
	A-3423 St.Andr�/W�rdern, S�dtirolergasse 17-21/5
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
