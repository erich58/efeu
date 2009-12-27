/*	Achsen reduzieren
	(c) 1994 Erich Frühstück
*/

#include <EFEU/mdmat.h>

void md_reduce(mdmat *md, const char *list)
{
	mdaxis **ptr;
	mdaxis *x;

	if	(md == NULL)	return;

	ptr = &(md->axis);

	while (*ptr != NULL)
	{
		if	((*ptr)->dim == 1)
		{
			x = *ptr;
			*ptr = x->next;
			x->next = NULL;
			del_axis(x);
		}
		else	ptr = &(*ptr)->next;
	}

	md_size(md->axis, md->type->size);
}
