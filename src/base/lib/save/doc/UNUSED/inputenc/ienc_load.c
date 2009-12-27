/*	Eingabekonvertierung laden
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include <EFEU/InputEnc.h>
#include <ctype.h>

static int enc_cmp (InputEncEntry_t *ma, InputEncEntry_t *mb)
{
	register uchar_t *a = ma->name;
	register uchar_t *b = mb->name;

	if	(a == NULL)	return (b ? -1 : 0);
	if	(b == NULL)	return 1;

	for (;;)
	{
		if	(*a < *b)	return -1;
		if	(*a > *b)	return 1;
		if	(*a == 0)	break;

		a++;
		b++;
	}
	
	return 0;
}


void InputEnc_load (InputEnc_t *enc, io_t *io)
{
	InputEncEntry_t entry;
	int c;
	
	if	(enc == NULL || io == NULL)	return;

	io = io_lnum(io_refer(io));

	while ((c = io_eat(io, "%s")) != EOF)
	{
		entry.name = io_xgets(io, " \t");
		io_eat(io, " \t");
		entry.value = io_xgets(io, "\n");

		vb_search(&enc->tab, &entry, (comp_t) enc_cmp, VB_REPLACE);
		memfree(entry.name);
		memfree(entry.value);
		io_skip(io, "\n");
	}

	io_close(io);
}
