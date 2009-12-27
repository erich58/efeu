/*	Umgebungswechsel
	(c) 1999 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <SynTeX.h>

/*	Ausgabefunktionen
*/

static void put_sec (SynTeX_t *stex, int mode, const char *name, va_list list)
{
	if	(mode)
	{
		char *opt = va_arg(list, char *);

		SynTeX_newline(stex);
		io_putc('\\', stex->out);
		io_puts(name, stex->out);

		if	(opt)
		{
			io_puts("[{", stex->out);
			io_puts(opt, stex->out);
			io_puts("}]", stex->out);
		}

		io_putc('{', stex->out);
	}
	else
	{
		io_puts("}\n", stex->out);
		stex->last = '\n';
		stex->ignorespace = 1;
	}
}


#if	0
static void put_xverb (SynTeX_t *stex, int flag)
{
	if	(flag)
	{
		SynTeX_newline(stex);
		io_puts("{\\parskip0pt", stex->out);
		io_puts("\\obeylines", stex->out);
		io_puts("\\obeyspaces\\tt\n", stex->out);
	}
	else
	{
		io_puts("}\n", stex->out);
	}
}
#endif

static void put_verb (SynTeX_t *stex, int flag)
{
	if	(flag)
	{
		SynTeX_newline(stex);
		/*
		io_puts("\\begin{trivlist}\n", stex->out);
		io_puts("\\strut\\hrulefill\\strut\n", stex->out);
		*/
		io_puts("\n{\\parskip0pt\n", stex->out);
		io_puts("\\nopagebreak\n\\begin{verbatim}\n", stex->out);
	}
	else
	{
		io_puts("\\end{verbatim}\n}\n", stex->out);
		/*
		io_puts("\\end{verbatim}\n\\nopagebreak\n", stex->out);
		io_puts("\\strut\\hrulefill\\strut}\n\n", stex->out);
		io_puts("\\end{trivlist}\n", stex->out);
		*/
		stex->last = '\n';
	}
}

static void spage_flags(io_t *io, const char *flags)
{
	int i, dim;
	char **list;

	dim = strsplit(flags, "%s,", &list);

	for (i = 0; i < dim; i++)
	{
		if	(mstrcmp(list[i], "small") == 0)
			io_puts("\\footnotesize", io);
		else if	(mstrcmp(list[i], "tt") == 0)
			io_puts("\\tt", io);
		else	io_printf(io, "%%%s\n\t", list[i]);
	}

	memfree(list);
}

static void put_env (SynTeX_t *stex, int flag, const char *name, int nl)
{
	SynTeX_newline(stex);
	io_puts(flag ? "\\begin{" : "\\end{", stex->out);
	io_puts(name, stex->out);
	stex->last = io_putc('}', stex->out);

	if	(!flag || nl)
		stex->last = io_putc('\n', stex->out);
}


/*	Umgebung beginnen/beenden
*/

int SynTeX_env (SynTeX_t *stex, int flag, va_list list)
{
	int cmd;
	char *p;

	switch ((cmd = va_arg(list, int)))
	{
	case DOC_PAR_STD:
		SynTeX_newline(stex);

		if	(flag)
		{
			/*
			if	(stex->hang)
				io_puts("\\hangpar\n", stex->out);
			*/

			stex->ignorespace = 0;
		}
		else if	(stex->hang)
			io_puts("\\hangpar\n", stex->out);

		break;

	case DOC_SEC_PART:	put_sec(stex, flag, "part", list); break;
	case DOC_SEC_CHAP:	put_sec(stex, flag, "chapter", list); break;
	case DOC_SEC_MCHAP:	put_sec(stex, flag, "manchapter", list); break;
	case DOC_SEC_SECT:	put_sec(stex, flag, "section", list); break;
	case DOC_SEC_SSECT:	put_sec(stex, flag, "subsection", list); break;
	case DOC_SEC_PARA:	put_sec(stex, flag, "paragraph", list); break;
	/*
	case DOC_SEC_SPARA:	put_sec(stex, flag, "subparagraph", list); break;
	*/
	case DOC_SEC_HEAD:	put_sec(stex, flag, "manhead", list); break;
	case DOC_SEC_SHEAD:	put_sec(stex, flag, "mansubhead", list); break;
	case DOC_SEC_CAPT:	put_sec(stex, flag, "caption", list); break;
	case DOC_SEC_SCAPT:	put_sec(stex, flag, "subcaption", list); break;
	case DOC_SEC_MARG:	put_sec(stex, flag, "Randnotiz", list); break;
	case DOC_SEC_NOTE:	put_sec(stex, flag, "Zitat", list); break;
	case DOC_SEC_FNOTE:	put_sec(stex, flag, "SynTabNote", list); break;

	case DOC_QUOTE_SGL:
		io_puts(flag ? "\\frq{}" : "\\flq{}", stex->out);
		break;
	case DOC_QUOTE_DBL:
		io_puts(flag ? "\\frqq{}" : "\\flqq{}", stex->out);
		break;
	case DOC_ATT_RM:
		io_puts(flag ? "{\\rm{}" : "}", stex->out);
		break;
	case DOC_ATT_BF:
		io_puts(flag ? "{\\bf{}" : "}", stex->out);
		break;
	case DOC_ATT_IT:
		io_puts(flag ? "{\\it{}" : "}", stex->out);
		break;
	case DOC_ATT_TT:
		io_puts(flag ? "{\\tt{}" : "}", stex->out);
		break;

	case DOC_MODE_SKIP:
		stex->skip = flag;
		break;
	case DOC_MODE_TEX:
	case DOC_MODE_PLAIN:
		stex->put = flag ? DocDrv_plain : (DocDrvPut_t) SynTeX_putc;
		break;
	case DOC_MODE_SGML:
	case DOC_MODE_MAN:
		SynTeX_env (stex, flag, list);
		break;
	case DOC_MODE_VERB:
		/*
		put_env(stex, flag, "verbatim", 1);
		*/
		put_verb(stex, flag);
		stex->put = flag ? DocDrv_plain : (DocDrvPut_t) SynTeX_putc;
		/*
		put_xverb(stex, flag);
		stex->put = (DocDrvPut_t) (flag ? SynTeX_plain : SynTeX_putc);
		*/
		break;
	case DOC_LIST_ITEM:
		put_env(stex, flag, "itemize", 1);
		break;
	case DOC_LIST_ENUM:
		put_env(stex, flag, "enumerate", 1);
		break;

	case DOC_LIST_DESC:
		put_env(stex, flag, "description", 1);
		break;
		
	case DOC_ENV_CODE:
		put_env(stex, flag, "CmdLines", 1);
		break;
	case DOC_ENV_HANG:
		put_env(stex, flag, "hangpar", 1);
		/*
		stex->hang = flag;
		*/
		break;
	case DOC_ENV_QUOTE:
		put_env(stex, flag, "quote", 1);
		break;
	case DOC_ENV_INTRO:
		put_env(stex, flag, "Vorwort", 1);
		break;
	case DOC_ENV_FORMULA:
		io_putc('$', stex->out);
		break;
	case DOC_ENV_TAG:
		SynTeX_newline(stex);

		if	(flag)
		{
			io_puts("\\item[{", stex->out);
			stex->put = (DocDrvPut_t) SynTeX_xputc;
			stex->last = 0;
			stex->ignorespace = 1;
		}
		else
		{
			io_puts("}]\n", stex->out);
			stex->put = (DocDrvPut_t) SynTeX_putc;
			stex->last = '\n';
			stex->space = 0;
			stex->ignorespace = 0;
		}

		break;
	case DOC_ENV_MPAGE:
		put_env(stex, flag, "manpage", 0);

		if	(!flag)	break;

		if	((p = va_arg(list, char *)) != NULL)
			io_printf(stex->out, "[%s]", p);

		io_printf(stex->out, "{%s}\n", va_arg(list, char *));
		stex->last = '\n';
		break;
	case DOC_ENV_SPAGE:
		put_env(stex, flag, "Teilseite", 0);

		if	(!flag)	break;

		p = va_arg(list, char *);
		io_printf(stex->out, "[%s]{", mstrlen(p) ? p : "0");
		spage_flags(stex->out, va_arg(list, char *));
		io_puts("}\n", stex->out);
		stex->last = '\n';
		break;
	case DOC_ENV_FIG:
		put_env(stex, flag, "SynTable", 0);

		if	(!flag)	break;

		p = va_arg(list, char *);
		io_printf(stex->out, "[%s]\n", mstrlen(p) ? p : "0");
		stex->last = '\n';
		break;
	case DOC_ENV_TAB:
		put_env(stex, flag, "SynTab", 0);

		if	(!flag)	break;

		io_printf(stex->out, "{%s}", va_arg(list, char *));
		io_printf(stex->out, "{%s}\n", va_arg(list, char *));
		stex->last = '\n';
		break;
	case DOC_ENV_MCOL:
		if	(flag)
		{
			SynTeX_newline(stex);
			io_puts("\t\\", stex->out);

			switch (va_arg(list, int))
			{
			case 'l':	io_putc('l', stex->out); break;
			case 'c':	io_putc('c', stex->out); break;
			default:	break;
			}

			io_printf(stex->out, "multicol{%d}{",
				va_arg(list, int));
			stex->last = 0;
		}
		else	io_putc('}', stex->out);

		break;
	default:
		return EOF;
	}

	return 0;
}
