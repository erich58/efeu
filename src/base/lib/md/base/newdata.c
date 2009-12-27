/*	Datenwerte in Liste konvertieren, Neue Version
	(c) 1995 Erich Frühstück
*/

#include <EFEU/mdmat.h>

#define	M_NULL	0	/* Keine Transformation */
#define	M_KONV	1	/* Konvertierungsfunktion */
#define	M_BASIS	2	/* Basisvergleich */
#define	M_TIME	3	/* Zeitpunktvergleich */

typedef struct {
	mdaxis_t *axis;
	Type_t *type;
	ObjList_t **ptr;
	Func_t *func;
	int imode;
	int lag;
} EVALPAR;


static void add_data(EVALPAR *par, void *data, void *base);

static void stdwalk(EVALPAR *par, int mode, mdaxis_t *x,
	char *data, char *base);


void MF_data(Func_t *func, void *rval, void **arg)
{
	EVALPAR par;
	ObjList_t *list;
	VirFunc_t *vtab;
	mdmat_t *md;

	Val_list(rval) = NULL;

	if	((md = Val_mdmat(arg[0])) == NULL)
		return;

/*	Auswertungsparameter bestimmen
*/
	vtab = Val_vfunc(arg[2]);
	par.func = NULL;
	par.lag = 0;
	par.imode = 0;

	if	(vtab && vtab->tab.used)
	{
		Func_t **ftab;
		int i, dim;

		ftab = vtab->tab.data;
		par.lag = Val_int(arg[3]);

		if	(par.lag == 0)
		{
			dim = 1;

			for (i = 0; i < vtab->tab.used; i++)
			{
				if	(ftab[i]->dim == 2)
				{
					par.imode = 1;
					dim = 2;
					break;
				}
			}
		}
		else	dim = 2;

		par.func = GetFunc(NULL, vtab, dim, md->type, 0, md->type, 0);

		if	(par.func == NULL)
		{
			reg_cpy(1, ftab[0]->name);
			reg_cpy(2, md->type->name);
			errmsg(MSG_MDMAT, 95);
			return;
		}
	}

/*	Datenwerte generieren
*/
	list = NULL;
	par.type = md->type;
	par.axis = md->axis;
	par.ptr = &list;
	md_setflag(md, Val_str(arg[1]), 0, NULL, 0, mdsf_lock, MDFLAG_TEMP);
	stdwalk(&par, 0, md->axis, md->data, NULL);
	md_allflag(md, 0, NULL, 0, mdsf_clear, MDFLAG_TEMP);
	Val_list(rval) = list;
}


/*	Primärschleife für Standardauswertung
*/

static void stdwalk(EVALPAR *par, int mode, mdaxis_t *x, char *data, char *base)
{
	char *ptr;
	int i, k;
	int flag;

/*	Achsen überlesen
*/
	while (x && (x->flags & MDXFLAG_MARK) != mode)
		x = x->next;

/*	Ende der Durchwanderung
*/
	if	(x == NULL)
	{
		if	(mode == 0)
		{
			stdwalk(par, MDXFLAG_MARK, par->axis, data, base);
		}
		else	add_data(par, data, base);

		return;
	}

/*	Initialisieren von ptr und flag
*/
	if	(base)
	{
		ptr = base;
		flag = 2;
	}
	else
	{
		ptr = data;
		flag = 0;
	}

/*	Test, ob ein Index als Indexbasis gekennzeichnet ist
	und Initialisieren von base mit der 1. Definition
*/
	if	(par->imode)
	{
		for (i = 0; i < x->dim; i++)
		{
			if	(x->idx[i].flags & MDFLAG_BASE)
			{
				base = ptr + i * x->size;
				flag = 1;
				break;
			}
		}
	}

/*	Achsenwerte durchwandern
*/
	for (i = 0; i < x->dim; i++)
	{
		if	(x->idx[i].flags & (MDFLAG_LOCK|MDFLAG_TEMP))
			continue;

		if	(par->lag && (x->flags & MDXFLAG_TIME))
		{
			k = i - par->lag;

			if	(k >= 0 && k < x->dim)
			{
				base = data + k * x->size;
			}
			else	base = NULL;
		}
		else if	(flag == 2 || (par->imode && (x->idx[i].flags & MDFLAG_BASE)))
		{
			base = ptr + i * x->size;
		}

		stdwalk(par, mode, x->next, data + i * x->size, base);
	}
}


static void add_data(EVALPAR *par, void *data, void *base)
{
	Obj_t *obj;
	void *arg[2];

	if	(par->func)
	{
		arg[0] = data;
		arg[1] = base;

		if	(base == NULL && par->func->dim == 2)
		{
			obj = ptr2Obj(NULL);
		}
		else	obj = ConstRetVal(par->func, arg);
	}
	else	obj = LvalObj(&Lval_ptr, par->type, data);

	if	(obj)
	{
		*par->ptr = NewObjList(obj);
		par->ptr = &(*par->ptr)->next;
	}
}
