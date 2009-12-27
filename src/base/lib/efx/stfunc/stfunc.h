/*	Strukturfunktionen
	(c) 1995 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/


#ifndef	EFEU_STFUNC_H
#define	EFEU_STFUNC_H	1

#include <EFEU/object.h>
#include <EFEU/vecbuf.h>

typedef struct StFunc_s StFunc_t;

struct StFunc_s {
	StFunc_t *next;
	Func_t *func;
	size_t offset;
	size_t size;
	size_t dim;
};

StFunc_t *NewStFunc(void);
void DelStFunc(StFunc_t *func);
StFunc_t *VaGetStFunc (const char *name, int narg, ...);

typedef int (*EvalStFunc_t) (Func_t *func, void *ptr, void **arg);
int CallStFunc (StFunc_t *st, EvalStFunc_t eval, void *ptr, ...);

int IgnoreStRetVal (Func_t *func, void *val, void **arg);
int CountStRetVal (Func_t *func, void *val, void **arg);
int TestStRetVal (Func_t *func, void *val, void **arg);


#endif	/* EFEU_STFUNC_H */
