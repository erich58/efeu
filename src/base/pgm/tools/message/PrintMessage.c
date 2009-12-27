/*	Fehlermeldungen ausgeben
	(c) 1997 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 3.0
*/

#include <EFEU/pconfig.h>
#include <EFEU/vecbuf.h>
#include <EFEU/strbuf.h>
#include <EFEU/msgtab.h>
#include <EFEU/ioctrl.h>

#define	VSIZE	16
#define	BSIZE	1024

static VECBUF(vb, VSIZE, sizeof(char *));
static strbuf_t *sb = NULL;

static void add_entry (void)
{
	size_t n = sb_size(sb);
	char **ptr = vb_next(&vb);

	*ptr = n ? mstrncpy((char *) sb->data, n) : 0;
	sb_clear(sb);
}


static void show_message(char **list, size_t dim)
{
	char *name;
	int num;
	char *type;
	char *fmt;
	char *p;
	int i;

	if	(dim < 2)	return;

	type = list[0];
	name = list[1];

	if	(name)
	{
		if	((p = strchr(name, ':')) != NULL)
		{
			*p = 0;
			num = atoi(p + 1);
		}
		else
		{
			num = atoi(name);
			name = NULL;
		}
	}
	else	num = 0;

	list += 2;
	dim -= 2;

	fmt = getmsg(name, num, NULL);

	if	(fmt != NULL)
	{
		reg_clear();

		for (i = 0; i < dim; i++)
		{
			reg_set(i, list[i]);
			list[i] = NULL;
		}

		io_psub(ioerr, fmt);
	}
	else
	{
		if	(ProgName)
			io_printf(ioerr, "%s: ", ProgName);

		if	(type)
			io_printf(ioerr, "%s ", type);

		if	(name)
			io_printf(ioerr, "%s:", name);

		io_printf(ioerr, "%d\n", num);
	}

	io_ctrl(ioerr, IO_FLUSH);
}


static void eval_line(void)
{
	char **list;
	int i;

	if	(sb_size(sb) != 0)
		add_entry();

	list = vb.data;
	show_message(list, vb.used);

	for (i = 0; i < vb.used; i++)
		memfree(list[i]);
		
	vb.used = 0;
	memfree(ProgName);
	ProgName = NULL;
}


int main(int narg, char **arg)
{
	int c;
	int flag;
	int get_ident;

	libinit(arg[0]);
	memfree(ProgName);
	ProgName = NULL;

	sb = new_strbuf(BSIZE);
	flag = 0;
	get_ident = 1;

	while ((c = getchar()) != EOF)
	{
		if	(c == '\\')
		{
			c = getchar();
			sb_putc(c, sb);
		}
		else if	(c == '"')
		{
			flag = !flag;
		}
		else if	(flag)
		{
			sb_putc(c, sb);
		}
		else if	(c == '-' && sb_size(sb) == 0)
		{
			c = getchar();

			if	(c == EOF)	break;

			if	(c == ' ' || c == '\t' || c == '\n')
			{
				add_entry();
			}
			else	sb_putc('-', sb);

			ungetc(c, stdin);
		}
		else if	(get_ident && c == ':')
		{
			size_t n = sb_size(sb);
			ProgName = n ? mstrncpy((char *) sb->data, n) : NULL;
			sb_clear(sb);
			get_ident = 0;
		}
		else if	(c == ' ' || c == '\t')
		{
			if	(sb_size(sb) == 0)	continue;

			add_entry();
			get_ident = 0;
		}
		else if	(c == '\n')
		{
			eval_line();
		}
		else	sb_putc(c, sb);
	}

	eval_line();
	return(0);
}
