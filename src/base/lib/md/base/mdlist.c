/*	Listenzerlegung
	(c) 1994 Erich Frühstück
*/

#include <EFEU/mdmat.h>
#include <ctype.h>

static int copy_opt (IO *in, StrBuf *buf)
{
	int c;
	int depth;

	depth = 0;

	while ((c = io_mgetc(in, 1)) != EOF)
	{
		if	(c == '[')
		{
			depth++;
		}
		else if	(c == ']')
		{
			if	(depth <= 0)	break;
			else			depth--;
		}
		else if	(c == '\\')
		{
			c = io_mgetc(in, 1);

			if	(!strchr("[]\\", c))
				sb_putc('\\', buf);
		}

		sb_putc(c, buf);
	}

	return c;
}

static int copy_name (IO *in, StrBuf *buf, int delim, int flag)
{
	int c;

	while ((c = io_mgetc(in, 1)) != EOF)
	{
		if	(c == '\\')
		{
			if	(c != '\\' && c != '[' && c != ']' &&
					c != delim && !isspace(c))
				sb_putc('\\', buf);
		}
		else if	(c == '[')
		{
			if	(flag)	break;

			sb_putc(c, buf);
			c = copy_opt(in, buf);

			if	(c == EOF)	break;
		}
		else if	(c == delim || isspace(c))
		{
			break;
		}

		sb_putc(c, buf);
	}

	sb_putc(0, buf);

	if	(c == '[')
	{
		copy_opt(in, buf);
		c = io_mgetc(in, 1);
	}

	sb_putc(0, buf);
	return c;
}

static char *list_next (char **ptr)
{
	char *p;
	int c;

	p = *ptr;

	do
	{
		c = **ptr;
		(*ptr)++;
	}
	while (c != 0 && c != EOF);

	return (*p != 0 ? p : NULL);
}


/*
Die Funktion |$1| liest eine Achsenliste aus einer IO-Struktur
*/

mdlist *io_mdlist (IO *io, int flag)
{
	IO *tmp;
	StrBuf *buf;
	mdlist *list;
	size_t dim, space;	
	char *ptr;
	int c;
	int lflag;

	if	(io_eat(io, "%s") == EOF)
		return NULL;

	buf = sb_create(0);
	c = copy_name(io, buf, '=', flag & MDLIST_NAMEOPT);
	tmp = io_strbuf(buf);
	dim = 0;

	if	(c == '=')
	{
		do
		{
			dim++;

			if	(flag & MDLIST_LISTOPT)
				lflag = 1;
			else if	(flag & MDLIST_NEWOPT)
				lflag = (io_peek(io) == ':');
			else	lflag = 0;
		}
		while ((c = copy_name(io, buf, ',', lflag)) == ',');

		if	(c != EOF)	io_ungetc(c, io);
	}
	else if	(c != EOF && !isspace(c))
	{
		dbg_error(NULL, "[mdmat:81]", "c", c);
		io_close(tmp);
		return NULL;
	}
	else	io_ungetc(c, io);

	io_close(tmp);
	sb_putc(0, buf);
	space = sb_size(buf);

	list = memalloc(sizeof(mdlist) + 2 * dim * sizeof(char *) + space);
	list->list = (char **) (list + 1);
	list->lopt = list->list + dim;
	ptr = (char *) (list->lopt + dim);
	memcpy(ptr, buf->data, space);
	sb_destroy(buf);

	list->next = NULL;
	list->name = list_next(&ptr);
	list->option = list_next(&ptr);
	list->dim = dim;

	for (dim = 0; dim < list->dim; dim++)
	{
		list->list[dim] = list_next(&ptr);
		list->lopt[dim] = list_next(&ptr);
	}

	return list;
}


/*
Die Funktion |$1| generiert eine Achsenliste aus einer Zweichenkette
*/

mdlist *str2mdlist (const char *str, int flag)
{
	IO *io;
	mdlist *list;
	mdlist **ptr;

	if	(str == NULL)	return NULL;

	list = NULL;
	ptr = &list;
	io = io_cstr(str);

	while ((*ptr = io_mdlist(io, flag)) != NULL)
		ptr = &(*ptr)->next;

	io_close(io);
	return list;
}


/*
Die Funktion |$1| gibt eine Achsenliste frei.
*/

void del_mdlist (mdlist *list)
{
	if	(list != NULL)
	{
		del_mdlist(list->next);
		memfree(list);
	}
}

