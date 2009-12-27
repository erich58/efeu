/*	Befehlsausführung
	(c) 1998 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5

	Version 0.5
*/

#ifndef	EFEU_CMDEVAL_H
#define	EFEU_CMDEVAL_H	1

#include <EFEU/object.h>
#include <EFEU/parsedef.h>

#define	CmdEval_Normal	0	/* Normale Verarbeitung */
#define	CmdEval_Cont	1	/* Fortsetzung */
#define	CmdEval_Break	2	/* Abbruch */
#define	CmdEval_Return	3	/* Rücksprung */

extern Obj_t *CmdEval_retval;
extern int CmdEval_stat;

extern void CmdEval_setup (void);

extern io_t *CmdEval_cin;
extern io_t *CmdEval_cout;

/*	Befehlsinterpreter
*/

void CmdEvalFunc(io_t *in, io_t *out, int flag);
int CmdPreProc (io_t *in, int key);
void CmdEval (io_t *in, io_t *out);

io_t *io_cmdeval (io_t *in, const char *delim);

#endif	/* EFEU_CMDEVAL_H */
