/*
:*:	pattern matching
:de:	Mustervergleich

$Copyright (C) 2007 Erich Frühstück
This file is part of EFEU.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; see the file COPYING.Library.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <EFEU/MatchPar.h>
#include <EFEU/memalloc.h>
#include <EFEU/mstring.h>
#include <EFEU/strbuf.h>
#include <ctype.h>

static ALLOCTAB(mp_tab, 32, sizeof(MatchPar));

static int pcmp (MatchPar *mp, const char *name, size_t idx)
{
	return patcmp(mp->pattern, name, NULL);
}

static int rcmp (MatchPar *mp, const char *name, size_t idx)
{
	return regexec(&mp->exp, name, 0, NULL, 0) == 0;
}

static int ncmp (MatchPar *mp, const char *name, size_t idx)
{
	return (idx >= mp->minval && idx <= mp->maxval);
}

static int tcmp (MatchPar *mp, const char *name, size_t idx)
{
	return 1;
}

static int fcmp (MatchPar *mp, const char *name, size_t idx)
{
	return 0;
}

static char *mp_ident (const void *ptr)
{
	const MatchPar *mp;
	StrBuf *sb;
	int delim;

	if	(!ptr)	return NULL;

	mp = ptr;
	sb = sb_acquire();
	delim = '[';

	while (mp)
	{
		sb_putc(delim, sb);

		if	(mp->flag == 0)
			sb_putc('-', sb);

		if	(mp->cmp == pcmp)
			sb_puts(mp->pattern, sb);
		else if	(mp->cmp == rcmp)
			sb_puts(mp->pattern, sb);
		else if	(mp->cmp == ncmp)
			sb_printf(sb, "#%u:%u", mp->minval, mp->maxval);
		else if	(mp->cmp == fcmp)
			sb_puts("<false>", sb);
		else	sb_puts("<true>", sb);

		mp = mp->next;
		delim = ' ';
	}

	sb_putc(']', sb);
	return sb_cpyrelease(sb);
}

static void mp_clean (void *ptr)
{
	MatchPar *mp = ptr;
	rd_deref(mp->next);
	memfree(mp->pattern);

	if	(mp->cmp == rcmp)
		regfree(&mp->exp);

	del_data(&mp_tab, mp);
}

RefType MatchPar_reftype = REFTYPE_INIT("MatchPar", mp_ident, mp_clean);

MatchPar *MatchPar_create (const char *def, size_t dim)
{
	MatchPar *x;

	x = new_data(&mp_tab);
	x->next = NULL;
	x->pattern = NULL;
	x->flag = 1;
	x->minval = 0;
	x->maxval = 0;
	x->cmp = tcmp;

	if	(def == NULL)
		return rd_init(&MatchPar_reftype, x);

	switch (*def)
	{	
		case '-':	x->flag = 0; def++; break;
		case '+':	def++; break;
		default:	break;
	}

	if	(*def == '#')
	{
		long a;
		char *p;

		a = strtol(def + 1, &p, 0);

		if	(-a > (long) dim)	a = 0;
		else if	(a < 0)			a += 1 + (long) dim;

		x->minval = a;

		if	(*p == ':')
		{
			a = strtol(p + 1, NULL, 0);
		}
		else	a = x->minval;

		if	(-a > (long) dim)	a = 0;
		else if	(a <= 0)		a += 1 + (long) dim;

		x->maxval = a;
		x->cmp = ncmp;
	}
	else if	(*def == '/')
	{
		char *p;
		int flags;
		int n;

		x->pattern = mstrcpy(def);
		p = strchr(x->pattern + 1, '/');
		flags = REG_EXTENDED | REG_NOSUB;

		if	(p)
		{
			*p = 0;

			if	(p[1] == 'i')
				flags |= REG_ICASE;
		}

		n = regcomp(&x->exp, x->pattern + 1, flags);

		if	(n)
		{
			static char err_buf[256];
			regerror(n, &x->exp, err_buf, sizeof(err_buf));
			io_xprintf(ioerr, "%s: %s\n", def, err_buf);
			x->cmp = fcmp;
		}
		else	x->cmp = rcmp;

		if	(p)
			*p = '/';
	}
	else if	(*def != 0)
	{
		x->pattern = mstrcpy(def);
		x->cmp = pcmp;
	}

	return rd_init(&MatchPar_reftype, x);
}

int MatchPar_exec (MatchPar *mp, const char *str, size_t idx)
{
	int flag;

	if	(mp == NULL)	return 0;

	flag = !mp->flag;

	while (mp != NULL)
	{
		if	(mp->cmp(mp, str, idx))
			flag = mp->flag;

		mp = mp->next;
	}

	return flag;
}

static MatchPar **add_match (MatchPar **ptr, StrBuf *sb, size_t dim)
{
	if	(sb_getpos(sb))
	{
		*ptr = MatchPar_create(sb_nul(sb), dim);
		return &(*ptr)->next;
	}
	else	return ptr;
}

MatchPar *MatchPar_scan (IO *io, int delim, size_t dim)
{
	StrBuf *sb;
	MatchPar *mp, **ptr;
	int depth;
	int c;

	sb = sb_acquire();
	mp = NULL;
	ptr = &mp;
	depth = 0;

	while ((c = io_mgetc(io, 0)) != EOF)
	{
		if	(c == '[')
		{
			depth++;
		}
		else if	(depth)
		{
			if	(c == ']')	depth--;
		}
		else
		{
			if	(c == delim)	break;

			if	(isspace(c) || c == ',')
			{
				ptr = add_match(ptr, sb, dim); 
				sb_trunc(sb);
				continue;
			}
		}

		if	(c == '\\')
		{
			c = io_mgetc(io, 0);

			if	(c == EOF)
			{
				c = '\\';
			}
			else if	(!isspace(c) && c != delim &&
				c != ',' && c != '[' && c != ']')
			{
				sb_putc('\\', sb);
			}
		}

		sb_putc(c, sb);
	}

	ptr = add_match(ptr, sb, dim); 
	sb_release(sb);
	return mp ? mp : MatchPar_create(NULL, dim);
}

MatchPar *MatchPar_list (const char *list, size_t dim)
{
	IO *io;
	MatchPar *par;
	
	io = io_cstr(list);
	par = MatchPar_scan(io, EOF, dim);
	io_close(io);
	return par;
}

MatchPar *MatchPar_vec (char **list, size_t ldim, size_t dim)
{
	MatchPar *match, **ptr;
	int i;

	match = NULL;
	ptr = &match;

	for (i = 0; i < ldim; i++)
	{
		*ptr = MatchPar_create(list[i], dim);
		ptr = &(*ptr)->next;
	}

	return match;
}

