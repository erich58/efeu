/*	Abhängigkeitslisten
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5

	Version 0.6

$Header	<EFEU/$1>
*/

#ifndef	_EFEU_MakeDepend_h
#define	_EFEU_MakeDepend_h	1

#include <EFEU/mstring.h>
#include <EFEU/vecbuf.h>
#include <EFEU/io.h>

extern int MakeDepend;
extern vecbuf_t DependList;
extern vecbuf_t TargetList;

void AddTarget (const char *name);
void AddDepend (const char *name);
void MakeDependRule (io_t *io);

#endif	/* EFEU/MakeDepend.h */
