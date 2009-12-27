/*	Achsen reduzieren
	(c) 1994 Erich Frühstück
*/

#include <EFEU/mdmat.h>

void md_reduce(mdmat_t *md, const char *list)
{
	mdaxis_t **ptr;
	mdaxis_t *x;

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
