/*	Pipe - IO-Struktur
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/


#include <EFEU/io.h>
#include <ctype.h>

#if	QC
#define	NEED_POPEN	1
#else
#define	NEED_POPEN	0
#endif


/*	Ein/Ausgabe auf File
*/

FILE *popen (const char *cmd, const char *type);
int pclose (FILE *stream);

io_t *io_popen(const char *proc, const char *mode)
{
	io_t *io;
	strbuf_t *sb;
	FILE *file;
	int i;

	if	(proc == NULL || mode == NULL)
		return NULL;

	switch (*mode)
	{
	case 'r':	file = popen(proc, "r"); break;
	case 'w':	file = popen(proc, "w"); break;
	default:	file = NULL; break;
	}

	if	(!file)	return NULL;

	sb = new_strbuf(32);
	sb_puts("<!", sb);

	for (i = 0; proc[i] && !isspace(proc[i]); i++)
		sb_putc(proc[i], sb);

	sb_puts(">", sb);
	sb_putc(0, sb);
	io = io_stream((char *) sb->data, file, pclose);
	del_strbuf(sb);
	return io;
}


/*	Implementation von popen()
*/

#if	NEED_POPEN

typedef struct KEY_S {
	struct KEY_S *next;	/* nächster Eintrag */
	FILE *file;		/* File Struktur */
	char *proc;		/* Prozeß */
	char *name;		/* Temporärer Filename */
} KEY;


static KEY *root = NULL;	/* Wurzel für offene Pseudo-Pipes */


/*	Pseudo-Pipe schließen
*/

int pclose(FILE *file)
{
	KEY *entry, **ptr;

	ptr = &root;

	while (*ptr != NULL)
	{
		if	((*ptr)->file == file)
		{
			entry = *ptr;
			*ptr = (*ptr)->next;
			fileclose(file);

			if	(entry->proc)
			{
				system(entry->proc);
				FREE(entry->proc);
			}

			remove(entry->name);
			FREE(entry->name);
			FREE(entry);
			return 0;
		}
		else	ptr = &(*ptr)->next;
	}

	return EOF;
}


/*	Pseudo-Pipe öffnen
*/

FILE *popen(const char *name, const char *mode)
{
	KEY *key, **ptr;
	char *p;

	key = ALLOC(1, KEY);
	key->next = NULL;
	key->file = NULL;
	key->proc = NULL;
	key->name = mstrcpy(tmpnam(NULL));

	switch (*mode)
	{
	case 'r':

		p = mstrcat(" > ", name, key->name, NULL);
		system(p);
		FREE(p);
		key->file = fileopen(key->name, "r");
		break;

	case 'w':

		key->file = fileopen(key->name, "w");
		key->proc = mstrcat(" < ", name, key->name, NULL);
		break;

	default:

		break;
	}

	ptr = &root;

	while (*ptr != NULL)
		ptr = &(*ptr)->next;

	*ptr = key;
	return key->file;
}

#endif	/* NEED_POPEN */
