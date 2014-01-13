/*	Funktionen abfragen
	(c) 1994 Erich Frühstück
*/

#include <EFEU/mdmat.h>

EfiFunc *mdfunc_add (const EfiType *type)
{
	EfiFunc *func;

	func = GetFunc((EfiType *) type, GetTypeFunc(type, "+="),
		2, type, 1, type, 0); 

	if	(func == NULL)
		log_note(NULL, "[mdmat:16]", "m", type2str(type));

	return func;
}
