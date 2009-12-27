/*	Achsen reduzieren
	(c) 1994 Erich Frühstück
*/

#include <EFEU/mdmat.h>

void md_reduce (mdmat *md, const char *def)
{
	mdaxis **ptr;
	mdaxis *x;
	char **list;
	size_t n;

	if	(md == NULL)	return;

	ptr = &(md->axis);
	n = mstrsplit(def, "%s,;", &list);

	while (*ptr != NULL)
	{
		if	((*ptr)->dim == 1 && patselect((*ptr)->name, list, n))
		{
			x = *ptr;
			*ptr = x->next;
			x->next = NULL;
			del_axis(x);
		}
		else	ptr = &(*ptr)->next;
	}

	memfree(list);
	md_size(md->axis, md->type->size);
}
