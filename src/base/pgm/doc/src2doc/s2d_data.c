/*	Datenstruktur
	(c) 2000 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include "src2doc.h"
#include <EFEU/strbuf.h>
#include <EFEU/efio.h>

static char *SecHead[] = {
	"\\Description\n",
	"\\Examples\n",
	"\\SeeAlso\n",
	"\\Diagnostics\n",
	"\\Notes\n",
	"\\Warnings\n",
	"\\Errors\n",
};

void SrcData_init (SrcData_t *data, io_t *ein, io_t *aus)
{
	int i;

	memset(data, 0, sizeof(SrcData_t));
	data->buf = new_strbuf(0);
	data->synopsis = new_strbuf(0);

	for (i = 0; i < BUF_DIM; i++)
		data->tab[i] = new_strbuf(0);

	data->ein = ein;
	data->aus = aus;
}


void copy_protect (const char *str, io_t *io)
{
	if	(!str || !io)	return;

	for (; *str; str++)
	{
		switch (*str)
		{
		case '<':
		case '>':
		case '\\':
		case '|':
		case '[':
		case '*':
		case '#':
		case '$':
		case '&':
			io_putc('\\', io);
			break;
		case '/':
			if	(str[1] == '/' || str[1] == '*')
				io_putc('\\', io);
			break;
		default:
			break;
		}

		io_putc(*str, io);
	}
}

void SrcData_clean (SrcData_t *data)
{
	int i;

	io_printf(data->aus, "\\Name\n%s -- ", data->var[VAR_NAME]);
	io_printf(data->aus, "%s\n", data->var[VAR_TITLE]);

	if	(data->var[VAR_HEAD] || sb_getpos(data->synopsis))
	{
		io_puts("\\Synopsis\n\\code\n", data->aus);

		if	(data->var[VAR_HEAD])
		{
			io_puts("\\#include ", data->aus);
			copy_protect(data->var[VAR_HEAD], data->aus);
			io_puts("\n\n", data->aus);
		}

		if	(sb_getpos(data->synopsis))
		{
			sb_putc(0, data->synopsis);
			io_puts((char *) data->synopsis->data, data->aus);
		}

		io_puts("\\end\n", data->aus);
	}

	for (i = 0; i < BUF_DIM; i++)
	{
		if	(sb_getpos(data->tab[i]))
		{
			io_puts(SecHead[i], data->aus);
			sb_putc(0, data->tab[i]);
			io_puts((char *) data->tab[i]->data, data->aus);
		}

		del_strbuf(data->tab[i]);
	}

	del_strbuf(data->buf);
	del_strbuf(data->synopsis);

	for (i = 0; i < VAR_DIM; i++)
		memfree(data->var[i]);
}
