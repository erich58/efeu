/*	Ausgabe einer Eingabekonvertierung
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include <EFEU/InputEnc.h>
#include <ctype.h>

int InputEnc_print (io_t *io, InputEnc_t *enc)
{
	InputEncEntry_t *x;
	size_t n;
	int k;

	if	(enc == NULL)
		return io_puts("NULL\n", io);

	k = io_printf(io, "%s(%s) = {", enc->reftype->label, enc->name);

	for (n = enc->tab.used, x = enc->tab.data; n-- > 0; x++)
	{
		k += io_puts("\n\t", io);
		k += io_xputs(x->name, io, " \t");
		k += io_puts("\t", io);
		k += io_xputs(x->value, io, " \t");
	}

	k += io_puts("\n}\n", io);
	return k;
}
