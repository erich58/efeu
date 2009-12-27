/*	Zählerstruktur generieren
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 2
*/

#include <EFEU/mdmat.h>
#include <EFEU/mdcount.h>


typedef struct {
	MdInit_t init;
	size_t dim;
	size_t *idx;
} INIT_t;

static void subinit (INIT_t *init, int depth, mdaxis_t *axis, char *data);
static void dummy (void *data);


static void dummy(void *data)
{
	;
}


/*	Zähltabelle aus IO-Struktur generieren
*/

mdmat_t *md_ioctab(const char *title, io_t *io, void *gtab, MdCount_t *counter)
{
	mdmat_t *tab;
	mdaxis_t **ptr;

/*	Matrixheader generieren
*/
	tab = new_mdmat();
	tab->title = mstrcpy(title);
	tab->axis = NULL;
	ptr = &tab->axis;

/*	Matrixdimensionen initialisieren
*/
	while ((*ptr = md_ctabaxis(io, gtab)) != NULL)
	{
		ptr = &(*ptr)->next;
	}

	md_ctabinit(tab, counter);
	return tab;
}


/*	Zähltabelle initialisieren
*/

void md_ctabinit(mdmat_t *tab, MdCount_t *counter)
{
/*	Parameter testen
*/
	if	(counter == NULL)
	{
		counter = stdcount;
	}

	if	(counter->add == NULL)
	{
		liberror(MSG_MDMAT, 41);
		counter->add = dummy;
	}

/*	Datenmatrix initialisieren
*/
	tab->type = mdtype(counter->type);
	tab->priv = counter;
	tab->size = md_size(tab->axis, tab->type->size);
	tab->data = memalloc(tab->size);

	if	(counter->init)
	{
		INIT_t init;

		init.init = counter->init;
		init.dim = md_dim(tab->axis);
		init.idx = memalloc(init.dim * sizeof(size_t));
		memset(init.idx, 0, init.dim * sizeof(size_t));

		subinit(&init, 0, tab->axis, (char *) tab->data);
	}
	else	memset(tab->data, 0, tab->size);
}


/*	Zähltabelle aus Definitionsstring generieren
*/

mdmat_t *md_ctab(const char *title, const char *def, void *gtab, MdCount_t *counter)
{
	io_t *io;
	mdmat_t *md;

	io = io_cstr(def);
	md = md_ioctab(title, io, gtab, counter);
	io_close(io);
	return md;
}


/*	Initialisierungshilfsprogramm
*/

static void subinit (INIT_t *init, int depth, mdaxis_t *axis, char *ptr)
{
	if	(axis != NULL)
	{
		int i;

		for (i = 0; i < axis->dim; i++)
		{
			init->idx[depth] = i;
			subinit(init, depth + 1, axis->next, ptr);
			ptr += axis->size;
		}
	}
	else	init->init(ptr, init->idx, init->dim);
}
