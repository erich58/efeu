/*	Kommentarbuffer umkopieren 
	(c) 2000 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include "src2doc.h"
#include <EFEU/parsub.h>
#include <EFEU/mstring.h>


static int cmp (const char *name, const char *arg)
{
	int flag = 0;

	while (*name && *name == tolower(*arg))
	{
		do
		{
			name++;

			if	(*name == '[')	flag = 1;
			else if	(*name != ']')	break;
		}
		while (*name);

		arg++;

		if	(*arg == 0 && (flag || *name == 0))	return 1;
	}

	return 0;
}

static strbuf_t *set_var (SrcData_t *data, int par, char **pptr)
{
	char *ptr = *pptr;
	
	do	ptr++;
	while	(*ptr && *ptr != '\n');

	while (isspace(*ptr))
		*(ptr++) = 0;

	memfree(data->var[par]);
	data->var[par] = parsub(*pptr);
	*pptr = ptr;
	return NULL;
}

static strbuf_t *get_buf (SrcData_t *data, int par, char **ptr)
{
	return data->tab[par];
}

static strbuf_t *get_synopsis (SrcData_t *data, int par, char **ptr)
{
	return data->synopsis;
}

static struct {
	char *key;
	strbuf_t *(*eval) (SrcData_t *data, int par, char **ptr);
	int par;
} evaltab[] = {
	{ "head[er]",		set_var, VAR_HEAD },
	{ "name",		set_var, VAR_NAME },
	{ "title",		set_var, VAR_TITLE },
	{ "desc[ription]",	get_buf, BUF_DESC },
	{ "example[s]",		get_buf, BUF_EX },
	{ "see[also]",		get_buf, BUF_SEE },
	{ "diag[nostics]",	get_buf, BUF_DIAG },
	{ "note[s]",		get_buf, BUF_NOTE },
	{ "warn[ings]",		get_buf, BUF_WARN },
	{ "err[ors]",		get_buf, BUF_ERR },
	{ "syn[opsis]",		get_synopsis, 0 },
};

static strbuf_t *eval_key (SrcData_t *data, const char *key, char **ptr)
{
	int i;

	for (i = 0; i < tabsize(evaltab); i++)
		if	(cmp(evaltab[i].key, key))
			return evaltab[i].eval(data, evaltab[i].par, ptr);

	return NULL;
}

/*	Stringbuffer umkopieren
*/

void SrcData_copy (SrcData_t *data, strbuf_t *buf)
{
	char *ptr;
	int need_nl;

	if	(sb_getpos(data->buf) == 0)	return;

	sb_putc(0, data->buf);
	ptr = (char *) data->buf->data;
	need_nl = (buf && sb_getpos(buf));

	while (*ptr != 0)
	{
		if	(*ptr == '$')
		{
			ptr++;

			if	(isdigit(*ptr))
			{
				int n = strtoul(ptr, &ptr, 10);

				if	(buf)
					sb_puts(reg_get(n), buf);

				continue;
			}
			else if	(isalpha(*ptr))
			{
				char *key = ptr;

				do	ptr++;
				while	(isalpha(*ptr));

				while (isspace(*ptr))
					*(ptr++) = 0;

				buf = eval_key(data, key, &ptr);
				need_nl = (buf && sb_getpos(buf));
				continue;
			}
		}

		if	(buf)
		{
			if	(need_nl)
			{
				sb_putc('\n', buf);
				need_nl = 0;
			}

			sb_putc(*ptr, buf);
		}

		ptr++;
	}

	sb_clear(data->buf);
}
