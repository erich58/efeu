/*	Mischvorgang
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include <EFEU/Document.h>
#include <ctype.h>

#define	PROMPT	"arg >>> "	/* Argumentprompt */

/*	Mischbefehl ausführen
*/

int Merge_stdcmd (Merge_t *merge, const char *name)
{
	DocMac_t *cmd = DocMacStack_cmd(NULL, name);

	if	(cmd == NULL)	return 0;
	if	(cmd->eval)	cmd->eval(cmd, merge, merge->out);

	return 1;
}


int Merge_getc (Merge_t *merge)
{
	return io_getc(merge->input);
}

int Merge_xgetc (Merge_t *merge)
{
	int c;

	while ((c = io_getc(merge->input)) != EOF)
	{
		if	(Merge_expand(merge, c))	continue;

		if	(c == '\\' && io_peek(merge->input) == '/')
			c = io_getc(merge->input);

		break;
	}
	
	return c;
}

int Merge_copy (Merge_t *merge, io_t *out, int begkey, int endkey, int flag)
{
	int c, depth;

	depth = 0;

	while ((c = Merge_getc(merge)) != EOF)
	{
		if	(flag && Merge_expand(merge, c))	continue;

		if	(c == endkey)
		{
			if	(depth <= 0)	break;
			else			depth--;
		}
		else if	(c == begkey)		depth++;
		else if	(c == '\\')
		{
			c = io_getc(merge->input);

			if	(c != begkey && c != endkey)
			{
				io_ungetc(c, merge->input);
				c = '\\';
			}
		}

		io_putc(c, out);
	}

	return c;
}

void Merge_eval (Merge_t *merge)
{
	int c;

	while ((c = Merge_xgetc(merge)) != EOF)
		DocOut_putc(c, merge->out);
}

void Merge_ioeval (Merge_t *merge, io_t *input)
{
	if	(merge->input)
		io_push(merge->input, input);
	else	merge->input = input;

	Merge_eval(merge);
}

void Merge_streval (Merge_t *merge, const char *str)
{
	if	(str)
		Merge_ioeval(merge, io_cstr(str));
}

char *Merge_arg (Merge_t *merge, int begkey, int endkey, int flag)
{
	io_t *out;
	strbuf_t *buf;
	char *prompt;

	if	(begkey)
	{
		int c = io_getc(merge->input);

		if	(c != begkey)
		{
			io_ungetc(c, merge->input);
			return NULL;
		}
	}

	buf = new_strbuf(0);
	out = io_strbuf(buf);
	prompt = io_prompt(merge->input, PROMPT);
	Merge_copy(merge, out, begkey, endkey, flag);
	io_prompt(merge->input, prompt);
	rd_deref(out);
	sb_putc(0, buf);
	return sb2mem(buf);
}


/*	Namen einlesen
*/

char *Merge_cmdname (Merge_t *merge, int c)
{
	while (c == ' ' || c == '\t')
		c = io_getc(merge->input);

	if	(c == '{')
	{
		io_ungetc(c, merge->input);
		return Merge_arg(merge, c, '}', 1);
	}

	if	(c == '\\')
	{
		int d = io_getc(merge->input);

		if	(isalpha(d) || d == '_')
			return MergeParse_name(merge->input, d);

		io_ungetc(d, merge->input);
	}

	io_ungetc(c, merge->input);
	return NULL;
}
