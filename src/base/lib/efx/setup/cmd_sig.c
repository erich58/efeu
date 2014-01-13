/*
:*:	Signal haendling
:de:	Signalsteuerung

$Copyright (C) 2008 Erich Frühstück
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

#include <EFEU/stdtype.h>
#include <EFEU/parsedef.h>
#include <EFEU/cmdsetup.h>
#include <signal.h>

#define	NSIG	64

static EfiObj *sig_expr[NSIG] = { NULL };

static void handle_signal (int sig)
{
	EfiVarStack *vstack = SaveVarStack();
	LocalVar = RefVarTab(GlobalVar);
	LocalObj = int2Obj(sig);
	UnrefEval(RefObj(sig_expr[sig]));
	RestoreVarStack(vstack);
	signal(sig, handle_signal);
}

static EfiParseDef pdef_sig[] = {
	{ "SIG_DFL", PFunc_int, (void *) 0 },
	{ "SIG_IGN", PFunc_int, (void *) 1 },

	{ "SIGHUP", PFunc_int, (void *) SIGHUP },
	{ "SIGINT", PFunc_int, (void *) SIGINT },
	{ "SIGQUIT", PFunc_int, (void *) SIGQUIT },
	{ "SIGILL", PFunc_int, (void *) SIGILL },
	{ "SIGTRAP", PFunc_int, (void *) SIGTRAP },
	{ "SIGABRT", PFunc_int, (void *) SIGABRT },
	{ "SIGFPE", PFunc_int, (void *) SIGFPE },
	{ "SIGKILL", PFunc_int, (void *) SIGKILL },
	{ "SIGUSR1", PFunc_int, (void *) SIGUSR1 },
	{ "SIGSEGV", PFunc_int, (void *) SIGSEGV },
	{ "SIGUSR2", PFunc_int, (void *) SIGUSR2 },
	{ "SIGPIPE", PFunc_int, (void *) SIGPIPE },
	{ "SIGALRM", PFunc_int, (void *) SIGALRM },
	{ "SIGTERM", PFunc_int, (void *) SIGTERM },
	{ "SIGCHLD", PFunc_int, (void *) SIGCHLD },
	{ "SIGCONT", PFunc_int, (void *) SIGCONT },
	{ "SIGSTOP", PFunc_int, (void *) SIGSTOP },
	{ "SIGTSTP", PFunc_int, (void *) SIGTSTP },
	{ "SIGTTIN", PFunc_int, (void *) SIGTTIN },
	{ "SIGTTOU", PFunc_int, (void *) SIGTTOU },
};

static void f_sig_mode (EfiFunc *func, void *rval, void **arg)
{
	int sig = Val_int(arg[0]);

	if	(sig > 0 && sig < NSIG)
	{
		UnrefObj(sig_expr[sig]);
		sig_expr[sig] = NULL;
		signal(sig, Val_int(arg[1]) ? SIG_IGN : SIG_DFL);
		Val_bool(rval) = 1;
	}
	else	Val_bool(rval) = 0;
}

static void f_sig_expr (EfiFunc *func, void *rval, void **arg)
{
	int sig = Val_int(arg[0]);

	if	(sig > 0 && sig < NSIG)
	{
		UnrefObj(sig_expr[sig]);
		sig_expr[sig] = RefObj(Val_obj(arg[1]));
		signal(sig, handle_signal);
		Val_bool(rval) = 1;
	}
	else	Val_bool(rval) = 0;
}

static void f_kill (EfiFunc *func, void *rval, void **arg)
{
	Val_bool(rval) = (kill(Val_int(arg[0]), Val_int(arg[1])) == 0);
}

static void f_raise (EfiFunc *func, void *rval, void **arg)
{
	Val_bool(rval) = (raise(Val_int(arg[0])) == 0);
}

static void f_alarm (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = alarm(Val_int(arg[0]));
}

static EfiFuncDef fdef_sig[] = {
	{ FUNC_VIRTUAL, &Type_bool, "signal (int signum, int mode = SIG_DFL)",
		f_sig_mode },
	{ FUNC_VIRTUAL, &Type_bool, "signal (int signum, Expr_t expr)",
		f_sig_expr },
	{ 0, &Type_bool, "kill (int pid, int sig)", f_kill },
	{ 0, &Type_bool, "raise (int sig)", f_raise },
	{ 0, &Type_int, "alarm (int sec)", f_alarm },
};

void CmdSetup_signal (void)
{
	AddParseDef(pdef_sig, tabsize(pdef_sig));
	AddFuncDef(fdef_sig, tabsize(fdef_sig));
}
