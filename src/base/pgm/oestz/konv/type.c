/*	Gegenstands/Kriterienbezeichner bestimmen
	(c) 1993 Erich Fruehstueck
	A-1090 Wien, Waehringer Strasse 64/6
*/

#include "oestz.h"
#include <ctype.h>


void oestz_type(io_t *io, OESTZ_HDR *hdr, const char *name)
{
	int i;
	char *p;

	p = mstrcpy(name ? name : "X");

	for (i = 0; p[i] != 0; i++)
		p[i] = toupper(p[i]);

	reg_set(0, p);
	reg_cpy(1, hdr->erhebung);
	reg_cpy(2, hdr->objekt);
	oestz_msg(io, 51);
	reg_set(1, msprintf("%d", hdr->gdim));
	reg_set(2, msprintf("%d", hdr->kdim));
	oestz_msg(io, 52);

	for (i = 0; i < hdr->gdim; i++)
	{
		reg_cpy(1, hdr->gname[i]);
		reg_set(2, msprintf("%d", i));
		oestz_msg(io, 53);
	}

	for (i = 0; i < hdr->kdim; i++)
	{
		reg_cpy(1, hdr->kname[i]);
		reg_set(2, msprintf("%d", hdr->gdim + i));
		oestz_msg(io, 53);
	}

	oestz_msg(io, 54);
	io_close(iomsg);
}
