/*	Hilfsprogramme zum öffnen von Files
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/ftools.h>
#include <EFEU/vecbuf.h>
#include <EFEU/patcmp.h>
#include <EFEU/procenv.h>


int filedebugflag = 0;


typedef struct {
	FILE *file;
	char *name;
	int (*close) (FILE *file);
} FileTab_t;

static int file_cmp (const FileTab_t *a, const FileTab_t *b)
{
	if	(a->file < b->file)	return -1;
	else if	(a->file > b->file)	return 1;
	else				return 0;
}

static VECBUF(filetab, 32, sizeof(FileTab_t));

static void file_debug(const char *type, FileTab_t *tab)
{
	if	(tab && filedebugflag)
	{
		if	(tab->name)
			fprintf(stderr, "%s(%s)\n", type, tab->name);
		else	fprintf(stderr, "%s(%p)\n", type, tab->file);
	}
}


void filenotice (char *name, FILE *file, int (*close) (FILE *file))
{
	FileTab_t tab;

	tab.name = name;
	tab.file = file;
	tab.close = close;
	file_debug("open", &tab);
	vb_search(&filetab, &tab, (comp_t) file_cmp, VB_REPLACE);
}


/*	Datei schließen, falls sie verschieden von stdin und
	stdout ist.
*/

int fileclose (FILE *file)
{
	FileTab_t *tab;
	FileTab_t key;
	int stat;

	if	(file == NULL || file == stdin ||
		 file == stdout || file == stderr)
		return 0;

	key.file = file;
	tab = vb_search(&filetab, &key, (comp_t) file_cmp, VB_DELETE);

	if	(tab)
	{
		file_debug("close", tab);
		stat = (tab->close ? tab->close(tab->file) : 0);
		memfree(tab->name);
		return stat;
	}

	message("fileclose", MSG_FTOOLS, 11, 0);
	return EOF;
}


void closeall(void)
{
	FileTab_t *tab;
	int i;

	tab = filetab.data;

	for (i = 0; i < filetab.used; i++)
	{
		file_debug("close", tab + i);

		if	(tab[i].close)
			tab[i].close(tab[i].file);

		memfree(tab[i].name);
	}

	filetab.used = 0;
}


char *fileident (FILE *file)
{
	if	(file == NULL)		return NULL;
	else if	(file == stdin)		return "<stdin>";
	else if	(file == stdout)	return "<stdout>";
	else if	(file == stderr)	return "<stderr>";
	else
	{
		FileTab_t key, *tab;

		key.file = file;
		tab = vb_search(&filetab, &key, (comp_t) file_cmp, VB_SEARCH);
		return (tab ? tab->name : "<unknown>");
	}
}


void filemessage(FILE *file, const char *name, int num, int narg, ...)
{
	va_list list = va_start(list, narg);
	vmessage(fileident(file), name, num, narg, list);
	va_end(list);
}

void fileerror(FILE *file, const char *name, int num, int narg, ...)
{
	va_list list = va_start(list, narg);
	vmessage(fileident(file), name, num, narg, list);
	va_end(list);
	procexit(EXIT_FAILURE);
}
