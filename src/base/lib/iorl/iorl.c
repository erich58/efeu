/*	Standard Ein/Ausgabe mit readline - Interface
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/Readline.h>
#include <EFEU/parsedef.h>
#include <EFEU/ioctrl.h>

#define	PROMPT_STD	PS1
#define	PROMPT_SUB	PS2
#define	PROMPT_MARK	"> "

#define	HAS_READLINE	1

#define	NEED_STTYCALL	0	/* Ab RedHat 5.0 nicht mehr notwendig */

#if	HAS_READLINE

#include <readline/readline.h>
#include <readline/history.h>

/*
char *readline (const char *prompt);
void add_history (const char *str);
*/
int isatty (int fileld);


typedef struct {
	int line;
	int depth;
	int mark;
	char *prompt;
	char *buf;
	int pos;
	char *histname;
} RLBuf_t;

/*	Hilfsvariablen
*/

int iorl_maxhist = 100;
int iorl_key = '!';

#define	RL_KEY_EOF	"eof"
#define	RL_KEY_LIST	"h:istory:"
#define	RL_KEY_EDIT	"fc"
#define	RL_KEY_REPEAT	"r"

#define	RL_MODE_LIST	0
#define	RL_MODE_EDIT	1
#define	RL_MODE_REPEAT	2

#define	RL_DEF_LIST	16


/*	Hilfsfunktionen
*/

static void iorl_addentry (char *line)
{
	if	(line && *line)
	{
		HIST_ENTRY *hist;

		using_history();
		hist = previous_history();

		if	(!hist || mstrcmp(hist->line, line) != 0)
			add_history(line);
	}
}


static char *iorl_fc (int mode, const char *args)
{
	HIST_ENTRY **list;
	int i, n;
	int start, end;
	strbuf_t *buf;
	char *p;
	char *tname;
	io_t *io;

	list = history_list();

	if	(list == NULL)	return NULL;

	for (n = 0; list[n] != NULL; n++);

	start = strtol(args, &p, 10);
	end = n;

	if	(p != args)
	{
		args = p;
		end = strtol(args, &p, 10);

		if	(p != args)
		{
			args = p;
		}
		else if	(mode == RL_MODE_LIST)
		{
			end = n;
			start = n - start + 1;
		}
		else	end = start;
	}
	else if	(mode == RL_MODE_LIST)
	{
		start = n - RL_DEF_LIST + 1;
	}
	else	start = n;

	if	(start < 1)	start = 1;
	if	(end < start)	end = n;

	if	(mode == RL_MODE_LIST)
	{
		while (*args == ' ' || *args == '\t')
			args++;

		if	(*args == '|')
		{
			io = io_fileopen(args, "w");
		}
		else	io = io_refer(iostd);

		for (i = start - 1; i < end; i++)
			io_printf(io, "%d: %s\n", i + 1, list[i]->line);

		io_close(io);
		return NULL;
	}

	buf = new_strbuf(512);
	tname = NULL;

	if	(mode == RL_MODE_EDIT)
	{
		tname = mstrcpy(tmpnam(NULL));
		io = io_fileopen(tname, "w");
	}
	else	io = io_strbuf(buf);

	for (i = start - 1; i < end; i++)
		io_printf(io, "%s\n", list[i]->line);

	io_close(io);

	if	(mode == RL_MODE_EDIT)
	{
		io = io_fileopen(tname, "r");
		p = msprintf("vi %s", tname);
		system(p);
#if	NEED_STTYCALL
		system("stty min 1");
#endif
		memfree(p);

		while ((i = io_getc(io)) != EOF)
			sb_putc(i, buf);

		io_close(io);
		remove(tname);
	}

	n = sb_size(buf);
	p = lmalloc(n);
	memcpy(p, buf->data, n);
	p[n - 1] = 0;
	return p;
}

static char *iorl_builtin (char *line)
{
	char *arg;

	if	(patcmp(RL_KEY_LIST, line + 1, &arg))
	{
		iorl_addentry(line);
		return iorl_fc(RL_MODE_LIST, arg);
	}

	if	(patcmp(RL_KEY_EDIT, line + 1, &arg))
		return iorl_fc(RL_MODE_EDIT, arg);

	if	(patcmp(RL_KEY_REPEAT, line + 1, &arg))
		return iorl_fc(RL_MODE_REPEAT, arg);

	return line;
}


/*	Readline - Interface
*/

static int iorl_get (RLBuf_t *rlbuf);
static int iorl_put (int c, RLBuf_t *rlbuf);
static int iorl_ctrl (RLBuf_t *rlbuf, int req, va_list list);


/*	Lesen von der Standardeingabe
*/

static int iorl_get(RLBuf_t *rlbuf)
{
	while (rlbuf->buf == NULL)
	{
		char *p;

		rlbuf->line++;

		if	(rlbuf->mark)	p = PROMPT_MARK;
		else if	(rlbuf->depth)	p = PROMPT_SUB;
		else if	(rlbuf->prompt)	p = rlbuf->prompt;
		else			p = PROMPT_STD;

		p = parsub(p);
		rlbuf->buf = readline(p);
		rlbuf->mark = 0;
		FREE(p);

		if	(rlbuf->buf == NULL)
		{
			putchar('\n');
			return EOF;
		}

		if	(rlbuf->buf[0] == iorl_key)
		{
			if	(patcmp(RL_KEY_EOF, rlbuf->buf + 1, NULL))
			{
				free(rlbuf->buf);
				return EOF;
			}

			rlbuf->buf = iorl_builtin(rlbuf->buf);

			if	(rlbuf->buf == NULL)
				continue;
		}

		iorl_addentry(rlbuf->buf);
		rlbuf->pos = 0;
	}

	if	(rlbuf->buf[rlbuf->pos] == 0)
	{
		free(rlbuf->buf);
		rlbuf->buf = NULL;
		rlbuf->pos = 0;
		return ('\n');
	}

	return rlbuf->buf[rlbuf->pos++];
}


/*	Zeichen zur Standardausgabe schreiben
*/

static int iorl_put(int c, RLBuf_t *rlbuf)
{
	putchar(c);
	return 1;
}


/*	Kontrollfunktion
*/

static int iorl_ctrl(RLBuf_t *rlbuf, int req, va_list list)
{
	char **ptr;

	switch (req)
	{
	case IO_IDENT:

		*va_arg(list, char **) = msprintf("<readline>:%d",
			rlbuf->line ? rlbuf->line : 1);
		return 0;

	case IO_LINEMARK:
		
		rlbuf->mark = 1;
		return 0;

	case IO_PROTECT:
	case IO_SUBMODE:
	
		if	(va_arg(list, int))	rlbuf->depth++;
		else if	(rlbuf->depth > 0)	rlbuf->depth--;

		return rlbuf->depth;

	case IO_PROMPT:

		if	((ptr = va_arg(list, char **)) != NULL)
		{
			char *save = rlbuf->prompt;
			rlbuf->prompt = *ptr;
			*ptr = save;
			return 0;
		}

		return EOF;

	case IO_CLOSE:

		if	(rlbuf->histname)
		{
			if	(iorl_maxhist)
				stifle_history(iorl_maxhist);

			write_history(rlbuf->histname);
		}

		memfree(rlbuf->prompt);
		memfree(rlbuf->buf);
		memfree(rlbuf);
		return 0;
		
	case IO_LINE:

		return (rlbuf->line ? rlbuf->line : 1);

	case IO_ERROR:

		return 0;

	default:

		break;
	}

	return EOF;
}

#endif	/* HAS_READLINE */

io_t *io_readline (const char *prompt, const char *histname)
{
#if	HAS_READLINE
	if	(isatty(0) && isatty(1))
	{
		io_t *io;
		RLBuf_t *rlbuf;

		rlbuf = memalloc(sizeof(RLBuf_t));
		rlbuf->prompt = mstrcpy(prompt);
		rlbuf->histname = histname ? ExpandPath(histname) : NULL;
#if	NEED_STTYCALL
		system("stty min 1");
#endif
		rl_initialize();

		if	(rlbuf->histname)
			read_history(rlbuf->histname);

		io = io_alloc();
		io->get = (io_get_t) iorl_get;
		io->put = (io_put_t) iorl_put;
		io->ctrl = (io_ctrl_t) iorl_ctrl;
		io->data = rlbuf;
		return io;
	}
	else
#endif
	return io_interact(NULL, NULL);
}

void SetupReadline ()
{
	SetupInteract();

#if	HAS_READLINE
	if	(isatty(0) && isatty(1))
		_interact_open = io_readline;
#endif
}

