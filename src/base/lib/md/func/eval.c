/*	Daten auswerten
	(c) 1995 Erich Frühstück
*/

#include <EFEU/mdeval.h>


static void *cpyidx(const void *idx, char *name)
{
	return (void *) idx;
}

static void noclean(void *idx)
{
	;
}


typedef struct {
	mdmat *md;
	void *par;
	unsigned mask;
	unsigned base;
	int lag;
} DATAPAR;

typedef struct {
	mdaxis *axis;
	int mode;
	void *idx;
	char *data;
	char *base;
} WALKPAR;

#define	MARKED(x)	(x->flags & MDXFLAG_MARK)
#define	TIME(x)		(x->flags & MDXFLAG_TIME)
#define	HIDDEN(x)	(x->flags & MDXFLAG_HIDE)

static void datawalk (const WALKPAR *walk, DATAPAR *par, MdEvalDef *eval);

void *md_eval(MdEvalDef *eval, void *par, mdmat *md,
	unsigned mask, unsigned base, int lag)
{
	DATAPAR data;
	WALKPAR walk;

	if	(md == NULL)	return NULL;

	walk.axis = md->axis;
	walk.mode = 0;
	walk.idx = NULL;
	walk.data = md->data;
	walk.base = NULL;

	data.md = md;
	data.par = eval->init ? eval->init(par, md->type) : par;
	data.mask = mask;
	data.base = base;
	data.lag = lag;
	datawalk(&walk, &data, eval);
	return eval->exit ? eval->exit(data.par) : NULL;
}


static void datawalk (const WALKPAR *walk, DATAPAR *data, MdEvalDef *eval)
{
	WALKPAR next;
	mdaxis *x;
	int n, k;
	int flag;
	void *(*newidx) (const void *idx, char *name);
	void (*clridx) (void *idx);
	char *ptr;

	x = walk->axis;

/*	Achsen überlesen
*/
	while (x != NULL)
	{
		if	(MARKED(x))
		{
			if	(walk->mode == 1)	break;
		}
		else
		{
			if	(walk->mode == 0)	break;
		}

		x = x->next;
	}

/*	Ende der Durchwanderung
*/
	if	(x == NULL)
	{
		if	(walk->mode == 0)
		{
			next.axis = data->md->axis;
			next.mode = 1;
			next.idx = walk->idx;
			next.data = walk->data;
			next.base = walk->base;

			if	(eval->start)	eval->start(data->par, next.idx);

			datawalk(&next, data, eval);

			if	(eval->end)	eval->end(data->par);
		}
		else if	(eval->data)
		{
			eval->data(data->par, data->md->type, walk->data, walk->base);
		}

		return;
	}

/*	Achsenwerte durchwandern
*/
	flag = (walk->mode == 0 && !HIDDEN(x));
	newidx = (flag && eval->newidx) ? eval->newidx : cpyidx;
	clridx = (flag && eval->clridx) ? eval->clridx : noclean;

	next.axis = x->next;
	next.mode = walk->mode;
	next.base = walk->base;

/*	Initialisieren voin ptr und flag
*/
	if	(walk->base)
	{
		ptr = walk->base;
		flag = 2;
	}
	else
	{
		ptr = walk->data;
		flag = 0;
	}

/*	Test, ob ein Index als Indexbasis gekennzeichnet ist
	und Initialisieren von base mit der 1. Definition
*/
	if	(data->base && data->lag == 0)
	{
		for (n = 0; n < x->dim; n++)
		{
			if	(x->idx[n].flags & data->base)
			{
				next.base = ptr + n * x->size;
				flag = 1;
				break;
			}
		}
	}
	else if	(TIME(x))
	{
		flag = 3;
	}

	for (n = 0; n < x->dim; n++)
	{
		if	(x->idx[n].flags & data->mask)	continue;

		if	(flag == 3)
		{
			k = n - data->lag;

			if	(k >= 0 && k < x->dim)
			{
				next.base = walk->data + k * x->size;
			}
			else	next.base = NULL;
		}
		else if	(flag == 2 || (x->idx[n].flags & data->base))
		{
			next.base = ptr + n * x->size;
		}

		next.idx = newidx(walk->idx, x->idx[n].name);
		next.data = walk->data + n * x->size;
		datawalk(&next, data, eval);
		clridx(next.idx);
	}
}
