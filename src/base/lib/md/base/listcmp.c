/*	Listenvergleich
	(c) 1994 Erich Frühstück
*/

#include <EFEU/mdmat.h>


mdlist_t *mdlistcmp(const char *name, int depth, mdlist_t *list)
{
	for ( ; list != NULL; list = list->next)
	{
		if	(list->name == NULL)
		{
			continue;
		}
		else if	(list->name[0] == '#')
		{
			if	(atoi(list->name + 1) == depth)
				break;
		}
		else if	(patcmp(list->name, name, NULL))
		{
			break;
		}
	}

	return list;
}
