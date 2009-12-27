/*	Hilfsfunktionen f�r Dokumentfunktionen
	(c) 1999 Erich Fr�hst�ck
	A-3423 St.Andr�/W�rdern, S�dtirolergasse 17-21/5
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
