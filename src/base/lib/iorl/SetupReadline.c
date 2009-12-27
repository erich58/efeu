/*
Standard Ein/Ausgabe mit readline - Interface

$Copyright (C) 1994 Erich Frühstück
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

#include <EFEU/Readline.h>
#include <EFEU/Debug.h>
#include <EFEU/ioctrl.h>
#include <EFEU/mstring.h>
#include <EFEU/patcmp.h>
#include <EFEU/procenv.h>
#include <EFEU/parsub.h>

#define	HAS_READLINE	1	/* Readline verwenden */
#define	RL_COMPLETION	0	/* Readline completion verwenden */
#define	NEED_STTYCALL	0	/* Fix für alte curses-Bibliothek */

#if	HAS_READLINE

#include <unistd.h>
#include <readline.h>
#include <history.h>

#define	PROMPT_STD	"$!: "
#define	PROMPT_SUB	"> "
#define	PROMPT_MARK	"> "

typedef struct {
	int line;
	int depth;
	int mark;
	char *prompt;
	char *buf;
	int pos;
	char *histname;
	int key;
	int hsize;
	int filename_completion;
} RLBuf_t;

static RLBuf_t *current_buf = NULL;

int iorl_begin_of_line = 0;
int iorl_line_offset = 0;


/*	Hilfsvariablen
*/

#define	RL_KEY		'!'
#define	RL_HSIZE	100

#define	RL_KEY_EOF	"eof"
#define	RL_KEY_LIST	"h:istory:"
#define	RL_KEY_EDIT	"fc"
#define	RL_KEY_REPEAT	"r"

#define	RL_MODE_LIST	0
#define	RL_MODE_EDIT	1
#define	RL_MODE_REPEAT	2

#define	RL_DEF_LIST	16

#if	RL_COMPLETION
static char *builtin_tab[] = {
	"eof", "history", "fc", "r"
};

static int test_builtin (const char *text, const char *name)
{
	text++;

	while (*text)
	{
		if	(*text != *name)	return 0;

		text++;
		name++;
	}

	return 1;
}

static char *copy_builtin (const char *text, const char *src)
{
	if	(src)
	{
		char *tg = lmalloc(strlen(src) + 2);
		tg[0] = current_buf->key;
		strcpy(tg + 1, src);
		return tg;
	}
	else	return NULL;
}

#if	0
static char *generator_test (const char *text, int state)
{
	static int n;

	if	(!state)
		n = 0;

	if	(n < 3)
	{
		char *tg = lmalloc(strlen(text) + 2);
		sprintf(tg, "%s%d", text, ++n);
		return tg;
	}

	return NULL;
}
#endif

static char *match_builtin (const char *text, int state)
{
	static int idx;

	if	(!state)
	{
		if	(!iorl_begin_of_line)	return NULL;
		/*
		if	(*text && *text != current_buf->key)	return NULL;
		*/
		if	(*text != current_buf->key)	return NULL;

		idx = tabsize(builtin_tab);
	}

	while (idx > 0)
	{
		char *p = builtin_tab[--idx];

		if	(*text == 0 || test_builtin(text, p))
			return copy_builtin(text, p);
	}

	return NULL;
}



/*	Hilfsfunktionen
*/

static char *iorl_generator (const char *text, int state)
{
	static char *(*func) (const char *text, int stat) = NULL;
	static char *(*next) (const char *text, int stat) = NULL;

	if	(!state)
	{
		func = match_builtin;
		/*
		next = generator_test;
		*/
		next = NULL;
	}

	while (func)
	{
		char *p = func(text, state);

		if	(p)
		{
			return p;
		}

		func = next;
		next = NULL;
		state = 0;
	}

	return NULL;
}

static char **iorl_completion (char *text, int start, int end)
{
	if	(!current_buf)	return NULL;

	iorl_line_offset = start;
	iorl_begin_of_line = (start || current_buf->mark) ? 0 : 1;

	if	(!current_buf->filename_completion)
		rl_attempted_completion_over = 1;

	return completion_matches(text, iorl_generator);
}
#endif

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
#if	RL_COMPLETION
		void *save_func;
#endif

		rlbuf->line++;

		if	(rlbuf->mark)	p = PROMPT_MARK;
		else if	(rlbuf->depth)	p = PROMPT_SUB;
		else if	(rlbuf->prompt)	p = rlbuf->prompt;
		else			p = PROMPT_STD;

		p = parsub(p);
		current_buf = rlbuf;
#if	RL_COMPLETION
		save_func = rl_attempted_completion_function;
		rl_attempted_completion_function = iorl_completion;
#endif
		rlbuf->buf = readline(p);
#if	RL_COMPLETION
		rl_attempted_completion_function = save_func;
#endif
		memfree(p);

		if	(rlbuf->buf == NULL)
		{
			putchar('\n');
			return EOF;
		}

		if	(!rlbuf->mark && rlbuf->buf[0] == rlbuf->key)
		{
			if	(patcmp(RL_KEY_EOF, rlbuf->buf + 1, &p))
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
		rlbuf->mark = 0;
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

static int iorl_ctrl (RLBuf_t *rlbuf, int req, va_list list)
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
			if	(rlbuf->hsize > 0)
				stifle_history(rlbuf->hsize);

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

	case RLCTRL:
		return 0;
	case RLCTRL_HSIZE:
		rlbuf->hsize = va_arg(list, int);
		return 0;
	case RLCTRL_KEY:
		rlbuf->key = va_arg(list, int);
		return 0;
	default:

		break;
	}

	return EOF;
}

static io_t *iorl_open (const char *prompt, const char *histname)
{
	io_t *io;
	RLBuf_t *rlbuf;

	rlbuf = memalloc(sizeof(RLBuf_t));
	rlbuf->prompt = mstrcpy(prompt);
	rlbuf->histname = histname ? ExpandPath(histname) : NULL;
	rlbuf->key = RL_KEY;
	rlbuf->hsize = RL_HSIZE;
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

#endif	/* HAS_READLINE */

/*
Falls ein Kommando interaktiv aufgerufen wird, also sowohl
Standardeingabe als auch Standardausgabe mit einem Terminal verbunden
sind, sorgt |$1| dafür, daß eine mit |io_interact| eingerichtete
IO-Struktur die Readline-Schnittstelle verwendet.
Die Funktion wird üblicherweise in der Hauptfunktion |main| eines
Kommandos aufgerufen.

Die Funktion |io_interact| wird mit den zwei Argumenten <prompt> und
<histname> aufgerufen.
Das Argument <prompt> legt den Prompt für die Eingabe fest.
Falls <histname> verschieden von Null ist, wird die entsprechende Datei
als Historyfile von |readline| verwendet.
*/

void SetupReadline ()
{
#if	HAS_READLINE
	if	(isatty(0) && isatty(1))
		_interact_open = iorl_open;
#else
	;
#endif
}

/*
$SeeAlso
\mref{io_interact(3)},
\mref{io(7)}, \mref{Readline(7)}.
*/
