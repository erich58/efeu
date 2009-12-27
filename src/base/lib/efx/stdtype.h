/*	Standardtypen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#ifndef	EFEU_STDTYPE_H
#define	EFEU_STDTYPE_H	1

#include <EFEU/object.h>

void Clean_obj (const Type_t *st, void *data);
void Clean_ref (const Type_t *st, void *data);
void Clean_str (const Type_t *st, void *data);

void Copy_obj (const Type_t *st, void *tg, const void *src);
void Copy_ref (const Type_t *st, void *tg, const void *src);
void Copy_str (const Type_t *st, void *tg, const void *src);

size_t IOData_std (const Type_t *t, iofunc_t f, void *p, void *d, size_t n);
size_t IOData_str (const Type_t *t, iofunc_t f, void *p, void *d, size_t n);


#define	COMPLEX_TYPE(name, size, recl, iodata, base, eval, clean, copy)	\
{ name, size, recl, iodata, base, eval, clean, copy, \
0, NULL, NULL, NULL, NULL, VB_DATA(8, sizeof(Func_t *)), NULL, NULL }

#define	EVAL_TYPE(name, type, eval, clean, copy)	\
COMPLEX_TYPE(name, sizeof(type), 0, NULL, NULL, eval, clean, copy)

#define	SIMPLE_TYPE(name, type, base)	\
COMPLEX_TYPE(name, sizeof(type), sizeof(type), IOData_std, base, NULL, NULL, NULL)

#define	STD_TYPE(name, type, base, clean, copy)	\
COMPLEX_TYPE(name, sizeof(type), 0, NULL, base, NULL, clean, copy)

#define	REF_TYPE(name, type)	\
COMPLEX_TYPE(name, sizeof(type), 0, NULL, &Type_ref, NULL, Clean_ref, Copy_ref)

#endif	/* EFEU_STDTYPE_H */
