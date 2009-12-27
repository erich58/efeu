/*	Test auf EOF - Marke
	(c) 1994 Erich Frühstück
*/

#include <EFEU/mdmat.h>


void md_tsteof(IO *io)
{
	char eof[4];

	io_read(io, eof, 4);

	if	(strncmp(eof, MD_EOFMARK, 4) != 0)
		io_error(io, "[mdmat:13]", NULL);
}
