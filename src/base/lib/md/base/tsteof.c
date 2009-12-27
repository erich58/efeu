/*	Test auf EOF - Marke
	(c) 1994 Erich Frühstück
*/

#include <EFEU/mdmat.h>


void md_tsteof(io_t *io)
{
	char eof[4];

	io_read(io, eof, 4);

	if	(strncmp(eof, MD_EOFMARK, 4) != 0)
	{
		liberror(MSG_MDMAT, 13);
	}
}
