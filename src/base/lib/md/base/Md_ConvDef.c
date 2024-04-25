/*	Konvertierungsfunktion bestimmen
	(c) 1994 Erich Frühstück
*/

#include <EFEU/mdmat.h>
#include <EFEU/preproc.h>

EfiConv *Md_ConvDef(EfiConv *buf, const EfiType *type1, const EfiType *type2)
{
	EfiConv *konv = GetConv(buf, type1, type2);

	if	(konv == NULL)
		log_note(NULL, "[mdmat:102]",
			"mm", Type2str(type1), Type2str(type2));

	return konv;
}
