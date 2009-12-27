/*	Daten addieren
	(c) 1994 Erich Frühstück
*/

#include <EFEU/mdmat.h>
#include <EFEU/mdcount.h>

static void subadd (mdaxis_t *x, char *ptr, MdCount_t *cnt);


void md_count(mdmat_t *ctab, const void *data)
{
	register cgrp_t *s;
	register mdaxis_t *x;
	MdCount_t *cnt;

/*	Zähler setzen
*/
	cnt = ctab->priv;

	if	(cnt->set && cnt->set(data) == 0)
		return;

/*	Selektionsindizes setzen
*/
	for (x = ctab->axis; x && x->priv; x = x->next)
	{
		for (s = x->priv; s != NULL; s = s->next)
		{
			s->idx = MdClassify(s->cdef, data);
			s->flag = (s->idx >= 0 && s->idx < s->cdef->dim);
		}
	}

/*	Addition durchführen
*/
	subadd(ctab->axis, (char *) ctab->data, cnt);
}


/*	Additionshilfsprogramm
*/

static void subadd (mdaxis_t *x, char *ptr, MdCount_t *cnt)
{
	if	(x && x->priv)
	{
		cgrp_t *s;

		for (s = x->priv; s != NULL; s = s->next)
		{
			if	(s->flag)
			{
				subadd(x->next, ptr + s->idx * x->size, cnt);
			}
	
			ptr += s->cdef->dim * x->size;
		}
	}
	else	cnt->add(ptr);
}
