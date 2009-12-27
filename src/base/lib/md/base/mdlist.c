/*	Listenzerlegung
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 2
*/

#include <EFEU/mdmat.h>
#include <ctype.h>

/*	Kopierdefinitionen für Listenelemente
*/

static iocpy_t c_name[] = {
	{ "[", NULL, 0, NULL },
	{ "%s=", NULL, 0, NULL },
	{ "\\",	"%s[=\\", 0, iocpy_esc },
};

static iocpy_t c_option[] = {
	{ "]", NULL, 0, NULL },
	{ "[", "]", 1, iocpy_str },
	{ "\\",	"[]\\", 1, iocpy_esc },
};

static iocpy_t c_list[] = {
	{ "[", NULL, 0, NULL },
	{ "%s,", NULL, 0, NULL },
	{ "[", "]", 1, iocpy_str },
	{ "\\",	"%s,[\\", 0, iocpy_esc },
};

static char *list_next (io_t *tmp, char **ptr);
static size_t docopy (io_t *in, io_t *out, iocpy_t *def, size_t dim, int flag);


/*	Listendefinition löschen
*/

void del_mdlist(mdlist_t *list)
{
	if	(list != NULL)
	{
		del_mdlist(list->next);
		FREE(list);
	}
}


/*	Neue Listendefinition lesen
*/

mdlist_t *io_mdlist(io_t *io, int flag)
{
	io_t *tmp;
	mdlist_t *list;
	size_t dim, space;	
	char *ptr;
	int c;
	int lflag;

	if	(io_eat(io, "%s") == EOF)
	{
		return NULL;
	}

	tmp = io_tmpbuf(0);
	space = docopy(io, tmp, c_name, tabsize(c_name), flag & MDLIST_NAMEOPT);
	dim = 0;
	c = io_getc(io);

	if	(c == '=')
	{
		do
		{
			dim++;

			if	(flag & MDLIST_LISTOPT)
			{
				lflag = 1;
			}
			else if	(flag & MDLIST_NEWOPT)
			{
				lflag = (io_eat(io, NULL) == ':');
			}
			else	lflag = 0;

			space += docopy(io, tmp, c_list, tabsize(c_list), lflag);
		}
		while ((c = io_mgetc(io, 1)) == ',');

		if	(c != EOF)	io_ungetc(c, io);
	}
	else if	(c != EOF && !isspace(c))
	{
		reg_fmt(1, "%#c", iocpy_last);
		liberror(MSG_MDMAT, 81);
		io_close(tmp);
		return NULL;
	}
	else	io_ungetc(c, io);

	list = memalloc((ulong_t) sizeof(mdlist_t)
		+ 2 * dim * sizeof(char *) + space);
	list->list = (char **) (list + 1);
	list->lopt = list->list + dim;
	ptr = (char *) (list->lopt + dim);
	io_rewind(tmp);
	list->next = NULL;
	list->name = list_next(tmp, &ptr);
	list->option = list_next(tmp, &ptr);
	list->dim = dim;

	for (dim = 0; dim < list->dim; dim++)
	{
		list->list[dim] = list_next(tmp, &ptr);
		list->lopt[dim] = list_next(tmp, &ptr);
	}

	io_close(tmp);
	return list;
}


/*	Liste aus String bestimmen
*/

mdlist_t *mdlist(const char *str, int flag)
{
	io_t *io;
	mdlist_t *list;
	mdlist_t **ptr;

	if	(str == NULL)	return NULL;

	list = NULL;
	ptr = &list;
	io = io_cstr(str);

	while ((*ptr = io_mdlist(io, flag)) != NULL)
		ptr = &(*ptr)->next;

	io_close(io);
	return list;
}


/*	Nächsten String lesen
*/

static char *list_next(io_t *io, char **ptr)
{
	char *p;
	int c;

	p = *ptr;

	do
	{
		c = io_getc(io);
		**ptr = c;
		(*ptr)++;
	}
	while (c != 0 && c != EOF);

	return (*p != 0 ? p : NULL);
}


static size_t docopy(io_t *in, io_t *out, iocpy_t *def, size_t dim, int flag)
{
	size_t space;

	if	(flag)
	{
		space = iocpy(in, out, def, dim);
	}
	else	space = iocpy(in, out, def + 1, dim - 1);

	space += io_nputc(0, out, 1);

	if	(iocpy_last == '[')
	{
		io_getc(in);
		space += iocpy(in, out, c_option, tabsize(c_option));

		if	(iocpy_last == ']')
		{
			io_getc(in);
		}
	}

	space += io_nputc(0, out, 1);
	return space;
}
