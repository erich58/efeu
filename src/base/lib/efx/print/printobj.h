/*	Ausgabedefinitionen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#ifndef	EFEU_PRINTOBJ_H
#define	EFEU_PRINTOBJ_H	1

#include <EFEU/efmain.h>
#include <EFEU/efio.h>
#include <EFEU/object.h>


Func_t *GetPrintFunc (const Type_t *type);
int PrintData (io_t *io, const Type_t *type, const void *data);
int PrintVecData (io_t *io, const Type_t *type, const void *data, size_t dim);

int PrintObj (io_t *io, const Obj_t *obj);
int PrintFmtList (io_t *io, const char *fmt, ObjList_t *list);
int PrintFmtObj (io_t *io, const char *fmt, const Obj_t *obj);
int PrintAny (io_t *io, const Type_t *type, const void *data);

extern char *PrintListBegin;
extern char *PrintListDelim;
extern char *PrintListEnd;

#endif	/* EFEU_PRINTOBJ_H */
