/*
Teildatenbankeingabe
$Copyright (C) 2006 Erich Fr�hst�ck
A-3423 St.Andr�/W�rdern, Wildenhaggasse 38
*/

#include <EFEU/EDB.h>
#include <EFEU/konvobj.h>

#define	ERR_CONV "[edb:conv]don't know how to convert $1 in $2.\n"

typedef struct {
	REFVAR;
	EDB *sub;
	EfiKonv conv;
} CPAR;

static void cpar_clean (void *data)
{
	CPAR *cpar = data;
	rd_deref(cpar->sub);
}

static RefType cpar_reftype = REFTYPE_INIT("EDB_CONV", NULL, cpar_clean);

static int read_conv (EfiType *type, void *data, void *par)
{
	CPAR *cpar = par;

	if	(edb_read(cpar->sub))
	{
		KonvData(&cpar->conv, data, cpar->sub->obj->data);
		return 1;
	}
	
	return 0;
}

EDB *edb_paste (EDB *base, EDB *sub)
{
	EfiKonv conv;

	if	(!base)
		return sub;

	if	(!sub)
		return base;

	if	(base->obj->type == sub->obj->type)
	{
		edb_input(base, sub->read, sub->ipar);
		sub->read = NULL;
		sub->ipar = NULL;
		rd_deref(sub);
	}
	else if	(!GetKonv(&conv, sub->obj->type, base->obj->type))
	{
		dbg_note(NULL, ERR_CONV, "mm", sub->obj->type->name,
			base->obj->type->name);
		rd_deref(sub);
	}
	else
	{
		CPAR *cpar = memalloc(sizeof *cpar);
		cpar->sub = sub;
		cpar->conv = conv;
		rd_init(&cpar_reftype, cpar);
		edb_input(base, read_conv, cpar);
	}

	return base;
}
