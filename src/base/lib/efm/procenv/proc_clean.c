/*
:*:controll of process clean up
:de:Kontrolle von Aufräumarbeiten

$Copyright (C) 2004 Erich Frühstück
This file is part of EFEU.

EFEU is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

EFEU is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public
License along with EFEU; see the file COPYING.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <EFEU/procenv.h>
#include <EFEU/memalloc.h>
#include <signal.h>

#define	CLEAN_BSIZE	4

struct CleanStack {
	struct CleanStack *next;
	void (*clean) (void *par);
	void *par;
};

static struct CleanStack *freelist = NULL;
static struct CleanStack *stack = NULL;

static void load ()
{
	struct CleanStack *list;
	size_t n;

	n = CLEAN_BSIZE;
	list = lmalloc(n * sizeof *list);

	while (n-- > 0)
	{
		list->next = freelist;
		list->clean = NULL;
		freelist = list++;
	}
}

static void sig_exit (int sig)
{
	proc_doclean();
	exit(EXIT_FAILURE);
}

static void set_sig (int sig)
{
	void (*save) (int) = signal(sig, sig_exit);

	if	(save != SIG_DFL)
		signal(sig, save);
}

static void proc_init (void)
{
	static int proc_init_done = 0;

	if	(proc_init_done)	return;

	proc_init_done = 1;
	atexit(proc_doclean);
	set_sig(SIGHUP);
	set_sig(SIGINT);
	set_sig(SIGTERM);
	set_sig(SIGPIPE);
}


/*
:*:The function |$1| registers <clfunc> to be called at process termination.
The Argument <par> is passed to the cleaning function <clfunc>.
The cleaning function is never called twice.
:de:Die Funktion |$1| registriert <clfunc> als Aufräumfunktion bei Prozessende.
Der Parameter <par> wird als Argument an die Aufräumfunktion <clfunc>
weitergereicht. Die Aufräumfunktion wird nur einmal aufgerufen.
*/

void proc_clean (void (*clfunc) (void *par), void *par)
{
	struct CleanStack *x;

	if	(!clfunc)	return;

	proc_init();

	if	(!freelist)	load();

	x = freelist;
	freelist = x->next;

	x->next = stack;
	x->clean = clfunc;
	x->par = par;
	stack = x;
}

/*
:*:The function |$1| calls all cleaning functions registered by |proc_clean|
in reverse order (last registerd, first called) and deletes it from
the internal register. The function |$1| itself is registered by |atexit|
to be called at program termination.
The first call to |proc_clean| registers |$1| with |atexit|
to be called at program termination.
:de:Die Funktion |$1| ruft alle mit |proc_clean| registrierten
Funktionen in umgekehrter Reihenfolge (Die zuletzt registrierte
Funktion wird als erstes aufgerufen) auf und löscht sie aus dem internen
Register. Der erste Aufruf von |proc_clean| registriert |$1| mit
|atexit| für den Aufruf bei Prozessende.
*/

void proc_doclean (void)
{
	struct CleanStack *p;

	while (stack)
	{
		p = stack;
		stack = p->next;
		p->clean(p->par);
		
		p->next = freelist;
		p->clean = NULL;
		p->par = NULL;
		freelist = p;
	}
}
