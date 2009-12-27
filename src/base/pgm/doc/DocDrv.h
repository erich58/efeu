/*	Dokumenttreiberstruktur
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5

	Version 0.4
*/

#ifndef	_DocDrv_h
#define	_DocDrv_h	1

#include <EFEU/io.h>
#include <EFEU/ioctrl.h>
#include <EFEU/parsub.h>
#include <EFEU/mstring.h>
#include <EFEU/stack.h>
#include <EFEU/vecbuf.h>
#include <EFEU/object.h>
#include <DocCtrl.h>
#include <DocSym.h>

#ifndef	CFGEXT
#define	CFGEXT	"cfg"
#endif


typedef struct DocDrv_s DocDrv_t;

typedef void (*DocDrvSym_t) (DocDrv_t *drv, const char *symbol);
typedef void (*DocDrvRem_t) (DocDrv_t *drv, const char *remark);
typedef void (*DocDrvHdr_t) (DocDrv_t *drv, int mode);
typedef int (*DocDrvCmd_t) (DocDrv_t *drv, va_list list);
typedef int (*DocDrvEnv_t) (DocDrv_t *drv, int mode, va_list list);
typedef int (*DocDrvPut_t) (DocDrv_t *drv, int c);

#define	DOCDRV_VAR		\
	const char *name;	\
	unsigned submode : 22;	\
	unsigned skip : 1;	\
	unsigned hflag : 1;	\
	char last;		\
	io_t *out;		\
	VarTab_t *vartab;	\
	vecbuf_t varbuf;	\
	DocSym_t *symtab;	\
	DocDrvPut_t put;	\
	DocDrvSym_t sym;	\
	DocDrvRem_t rem;	\
	DocDrvHdr_t hdr;	\
	DocDrvCmd_t cmd;	\
	DocDrvEnv_t env		\

struct DocDrv_s {
	DOCDRV_VAR;
};

extern void *DocDrv_alloc (const char *name, size_t size);
extern void DocDrv_free (void *drv);
extern void DocDrv_var (void *drv, Type_t *type, const char *name, void *ptr);
extern void DocDrv_eval (void *ptr, const char *name);
extern io_t *DocDrv_io (void *drv);
extern int DocDrv_plain (DocDrv_t *drv, int c);

#endif	/* DocDrv.h */
