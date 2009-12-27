/*
EDB-Metadatenfile
*/

#include <EFEU/EDB.h>
#include <EFEU/EDBMeta.h>
#include <EFEU/ioctrl.h>
#include <EFEU/printobj.h>
#include <EFEU/preproc.h>
#include <ctype.h>

#define	ERR_MH	"[edb:mhdr]$0: Kennung fehlt.\n"
#define	ERR_UC	"[edb:uchar]$0: unexpected character $1\n"
#define	ERR_CMD "[edb:cmd]$!: unknown command $1.\n"
#define	ERR_TYP	"[edb:type]$0: data type not defined\n"

static char *parse_key (IO *io, StrBuf *buf)
{
	int c;

	sb_clean(buf);

	while ((c = io_skipcom (io, NULL, 0)) != EOF)
	{
		if	(!isalnum(c) && c != '_')
		{
			while (c == ' ' || c == '\t')
				c = io_skipcom(io, NULL, 0);

			io_ungetc(c, io);
			break;
		}

		sb_putc(c, buf);
	}

	sb_putc(0, buf);
	buf->pos--;
	return (char *) buf->data;
}

static char *parse_arg (IO *io, StrBuf *buf)
{
	int c;

	sb_clean(buf);

	while ((c = io_skipcom (io, NULL, 0)) != EOF)
	{
		if	(c == '$')
		{
			sb_puts(psubexpand(NULL, io, 0, NULL), buf);
			continue;
		}

		if	(c == '\\')
		{
			c = io_getc(io);

			if	(c == '\n')
			{
				do	c = io_skipcom(io, NULL, 0);
				while	(c == ' ' || c == '\t');
			}
			else	sb_putc('\\', buf);
		}

		if	(c == '\n')
			break;

		sb_putc(c, buf);
	}

	sb_putc(0, buf);
	buf->pos--;

	while (buf->pos && isspace(buf->data[buf->pos - 1]))
		buf->data[--buf->pos] = 0;

	return buf->pos ? (char *) buf->data : NULL;
}

char *EDBMeta_par (EDBMeta *meta, int flag)
{
	IO *io = meta->ctrl;
	StrBuf *buf = &meta->buf;
	int last = '\n';
	int c;
	
	sb_clean(buf);

	while ((c = io_mgetc(io, 1)) != EOF)
	{
		if	(c == '@' && last == '\n')
		{
			io_ungetc(c, io);
			break;
		}

		if	(flag && c == '$')
		{
			sb_puts(psubexpand(NULL, io, 0, NULL), buf);
		}
		else if	(c != '\n' || buf->pos)
		{
			sb_putc(c, buf);
		}

		last = c;
	}

	sb_putc(0, buf);
	buf->pos--;

	while (buf->pos && buf->data[buf->pos - 1] == '\n')
		buf->data[--buf->pos] = 0;

	return buf->pos ? (char *) buf->data : NULL;
}

static EDB *meta_eval (EDBMeta *meta)
{
	EDBMetaDef *def;
	int c;
	char *p;

	do	c = io_skipcom(meta->ctrl, &meta->buf, 1);
	while	(isspace(c));

	p = sb_nul(&meta->buf);

	while (*p == ' ' || *p == '\t')
		p++;

	meta->desc = *p ? mstrcpy(p) : NULL;
	io_ungetc(c, meta->ctrl);

	if	(c != '@')
		EDBMeta_type(NULL, meta, NULL);

	while ((c = io_skipcom(meta->ctrl, NULL, 1)) != EOF)
	{
		if	(c == '@')
		{
			p = parse_key(meta->ctrl, &meta->buf);

			if	((def = GetEDBMetaDef(p)) != NULL)
			{
				p = parse_arg(meta->ctrl, &meta->buf);

				if	(def->flag && !meta->cur)
				{
					io_error(meta->ctrl, ERR_TYP, NULL);
					break;
				}

				if	(def->eval)
					def->eval(def, meta, p);
			}
			else
			{
				io_error(meta->ctrl, ERR_CMD, "s", p);
				parse_arg(meta->ctrl, &meta->buf);
			}
		}
		else if	(isspace(c))
		{
			;
		}
		else
		{
			io_error(meta->ctrl, ERR_UC, "c", c);
			EDBMeta_clean(meta);
			break;
		}
	}

	return EDBMeta_edb(meta);
}

EDB *edb_meta (IO *base, char *path)
{
	EDBMeta meta;
	IO *io;
	int c;

	io = io_lnum(base);

	if	(!io_testkey(io, "EDB"))
	{
		io_error(io, ERR_MH, NULL);
		io_close(io);
		memfree(path);
		return NULL;
	}

	do	c = io_getc(io);
	while	(c != '\n' && c != EOF);

	EDBMeta_init(&meta, rd_refer(base), io);
	meta.path = path;
	return meta_eval(&meta);
}
