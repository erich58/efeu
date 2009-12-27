/*	Standardverarbeitung
	(c) 2000 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5

	Version 1.0
*/

#include "src2doc.h"
#include <EFEU/strbuf.h>
#include <EFEU/patcmp.h>
#include <EFEU/ftools.h>
#include <ctype.h>

/*
#define	BEG "\n\\spage[3]\n\n---- verbatim\n"
#define	END "----\n\\end\n"
*/
#define	BEG "\n---- verbatim\n"
#define	END "----\n\n"

static void copyline (io_t *ein, io_t *aus)
{
	int c;

	while ((c = io_getc(ein)) != EOF && c != '\n')
		io_putc(c, aus);

	io_putc('\n', aus);
}


static void subcopy (const char *name, io_t *ein, io_t *aus, io_t *src)
{
	strbuf_t *buf;
	int c, flag, nlcount;

	buf = new_strbuf(0);
	flag = 0;
	nlcount = 0;

/*	Das erste Kommentar wird gesondert behandelt
*/
	c = io_skipcom(ein, buf, 1);

	if	(sb_getpos(buf))
	{
		char *p;

		sb_putc(0, buf);
		/*
		io_puts("\\title\t", aus);
		*/

		for (p = (char *) buf->data; *p != 0 && *p != '\n'; p++)
			io_putc(*p, aus);

		io_putc('\n', aus);
		sb_clear(buf);
	}
	else if	(name)
	{
		io_printf(aus, "\\title\t%s\n\n");
	}

	do
	{
		if	(sb_getpos(buf))
		{
			if	(flag)
				io_puts(END, src);

			sb_putc(0, buf);
			io_putc('\n', aus);
			io_puts((char *) buf->data, aus);
			sb_clear(buf);
			flag = 0;
		}

		if	(c != '\n')
		{
			if	(!flag)
			{
				io_puts(BEG, src);
				flag = 1;
				nlcount = 0;
			}

			io_nputc('\n', src, nlcount);
			nlcount = 0;
			io_putc(c, src);
			copyline(ein, src);
		}
		else	nlcount++;
	}
	while ((c = io_skipcom(ein, buf, 1)) != EOF);

	if	(flag)
		io_puts(END, src);

	del_strbuf(buf);
}

void s2d_std (const char *name, io_t *ein, io_t *aus)
{
	subcopy(name, ein, aus, aus);
}

void s2d_com (const char *name, io_t *ein, io_t *aus)
{
	subcopy(name, ein, aus, NULL);
}

void s2d_doc (const char *name, io_t *ein, io_t *aus)
{
	io_copy(ein, aus);
}

void s2d_man (const char *name, io_t *ein, io_t *aus)
{
	fname_t *fn = strtofn(name);

	if	(fn)
	{
		io_printf(aus, "\\mpage[%s] %s\n", Secnum ? Secnum : fn->type,
			fn->name);
		memfree(fn);
	}

	io_copy(ein, aus);
}
