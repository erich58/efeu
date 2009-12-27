/*	Parse-Definitionen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#ifndef	EFEU_PARSEDEF_H
#define	EFEU_PARSEDEF_H	1

#include <EFEU/object.h>

typedef Obj_t *(*ParseFunc_t) (io_t *io, void *data);

typedef struct {
	char *name;
	ParseFunc_t func;
	void *data;
} ParseDef_t;

ParseDef_t *NewParse (const Type_t *type, const char *name, size_t dim);
void DelParse (ParseDef_t *var);

void *NewParseTab (size_t dim);
void DelParseTab (void *tab);
void PushParseTab (void *tab);
void *PopParseTab (void);

void AddParseDef (ParseDef_t *pdef, size_t dim);
ParseDef_t *GetParseDef (const char *name);
void ListParseDef (io_t *io);

Obj_t *PFunc_typedef (io_t *io, void *data);
Obj_t *PFunc_struct (io_t *io, void *data);

Obj_t *PFunc_bool (io_t *io, void *data);
Obj_t *PFunc_int (io_t *io, void *data);
Obj_t *PFunc_str (io_t *io, void *data);
Obj_t *PFunc_type (io_t *io, void *data);

Obj_t *PFunc_for (io_t *io, void *data);
Obj_t *PFunc_while (io_t *io, void *data);
Obj_t *PFunc_do (io_t *io, void *data);
Obj_t *PFunc_if (io_t *io, void *data);
Obj_t *PFunc_break (io_t *io, void *data);
Obj_t *PFunc_return (io_t *io, void *data);


#endif	/* EFEU_PARSEDEF_H */
