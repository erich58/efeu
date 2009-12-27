/*	Datenbank ausgeben
	(c) 1995 Erich Fr�hst�ck
	A-1090 Wien, W�hringer Stra�e 64/6

	Version 0.4
*/

#include <EFEU/database.h>
#include <EFEU/printobj.h>

#define	COLSEP	";"

/*	Variablenliste zur Ausgabe generieren
*/

static size_t make_list (Var_t *st, char *list, Var_t ***ptr)
{
	size_t dim, idim;
	char **ilist;
	Var_t *x;
	idxcmp_t *cmp;

	*ptr = NULL;

	if	(st == NULL)	return 0;

	for (x = st, dim = 0; x != NULL; x = x->next)
		dim++;

	idim = strsplit(list, "%s,;", &ilist);
	cmp = idxcmplist(ilist, idim, dim);
	FREE(ilist);
	*ptr = ALLOC(dim, Var_t *);
	dim = 0;

	for (x = st, dim = 0; x != NULL; x = x->next)
	{
		(*ptr)[dim] = idxcmp(cmp, x->name, dim) ? x : NULL;
		dim++;
	}

	del_idxcmp(cmp);
	return dim;
}


/*	Header ausgeben
*/

static void print_header (io_t *io, Var_t **ptr, size_t dim)
{
	char *delim;
	int i;

	for (i = 0, delim = "#"; i < dim; i++)
	{
		if	(ptr[i])
		{
			io_puts(delim, io);
			delim = COLSEP;
			io_puts(ptr[i]->name, io);
		}
	}

	io_putc('\n', io);
}


/*	Daten ausgeben
*/

static void print_data (io_t *io, int mode, char *data, Var_t **ptr, size_t dim)
{
	char *delim;
	int i;

	for (i = 0, delim = NULL; i < dim; i++)
	{
		if	(!ptr[i])	continue;

		io_puts(delim, io);

		if	(mode)
		{
			io_puts(ptr[i]->name, io);
			io_puts(" = ", io);
			delim = "\n";
		}
		else	delim = COLSEP;

		if	(ptr[i]->dim)
			PrintVecData(io, ptr[i]->type, data + ptr[i]->offset, ptr[i]->dim);
		else	PrintData(io, ptr[i]->type, data + ptr[i]->offset);
	}
}


/*	Datenbankwerte ausgeben
*/

void DB_save(io_t *io, DataBase_t *db, int mode, VirFunc_t *test, char *list)
{
	size_t dim;
	Var_t **ptr;
	char *data;
	char *delim;
	Func_t *func;
	int testval;
	int i;

	if	(db == NULL)		return;

	dim = make_list(db->type->list, list, &ptr);
	func = test ? GetFunc(&Type_bool, test, 1, db->type, 0) : NULL;

	if	(mode)	;
	else if	(dim)	print_header(io, ptr, dim);
	else		io_puts("#this\n", io);

	data = db->buf.data;
	delim = NULL;

	for (i = 0; i < db->buf.used; i++)
	{
		testval = 1;

		if	(func)	CallFunc(&Type_bool, &testval, func, data);

		if	(testval)
		{
			io_puts(delim, io);
			delim = mode ? "\n" : NULL;

			if	(dim)	print_data(io, mode, data, ptr, dim);
			else		PrintData(io, db->type, data);

			io_putc('\n', io);
		}

		data += db->type->size;
	}

	FREE(ptr);
}
