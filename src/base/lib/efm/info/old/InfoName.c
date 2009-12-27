/*	Informationskontenkennung ausgeben
	(c) 1998 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/Info.h>
#include <EFEU/mstring.h>
#include <EFEU/patcmp.h>
#include <ctype.h>

static int put_string (io_t *io, const char *name, const char *pt)
{
	int c, n;
	int flag;

	if	(name == NULL)	return 0;

	for (n = flag = 0; *name != 0; name++)
	{
		c = *name;

		switch (c)
		{
		case '\\':
		case '.':
			io_putc('\\', io);
			break;
		case ' ':
			if	(flag)
				io_putc('\\', io);
			break;
		case '\t':
			io_putc('\\', io);
			c = 't';
			break;
		case '\n':
			io_putc('\\', io);
			c = 'n';
			break;
		default:
			if	(listcmp(pt, c) || isspace(c))
				io_putc('\\', io);
			break;
		}

		io_putc(c, io);
		flag = (c == ' ');
	}

	return 1;
}


int InfoName (io_t *io, InfoNode_t *root, InfoNode_t *node, const char *pt)
{
	int n;

	if	(node == NULL)	return 0;
	if	(node == root)	return 0;

	if	(node->prev)
	{
		if	((n = InfoName(io, root, node->prev, pt)) != 0)
			io_putc('.', io);
	}
	else	n = 0;

	return put_string(io, node->name, pt) ? 1 : n;
}

int InfoHead (io_t *io, InfoNode_t *node, const char *pt)
{
	return node ? put_string(io, node->head, pt) : 0;
}
