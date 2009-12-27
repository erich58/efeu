/*	Hilfsfunktionen für Dokumentfunktionen
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include "efeudoc.h"

char *ListArg_str (ObjList_t *list, int n)
{
	while (n-- > 0 && list != NULL)
		list = list->next;

	return list ? Obj2str(RefObj(list->obj)) : NULL;
}

int ListArg_int (ObjList_t *list, int n)
{
	while (n-- > 0 && list != NULL)
		list = list->next;

	return list ? Obj2int(RefObj(list->obj)) : 0;
}
