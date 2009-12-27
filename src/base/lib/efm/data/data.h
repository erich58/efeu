/*	Datenhilfsprogramme
	(c) 1998 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

$Header	<EFEU/$1>
*/

#ifndef EFEU_DATA_H
#define EFEU_DATA_H	1

#include <EFEU/mstring.h>
#include <EFEU/ftools.h>

typedef size_t (*iofunc_t) (void *ptr, void *dp, size_t r, size_t s, size_t n);

size_t xloaddata (void *ptr, void *dp, size_t recl, size_t size, size_t n);
size_t floaddata (void *ptr, void *dp, size_t recl, size_t size, size_t n);
size_t mloaddata (void *ptr, void *dp, size_t recl, size_t size, size_t n);

size_t xsavedata (void *ptr, void *dp, size_t recl, size_t size, size_t n);
size_t fsavedata (void *ptr, void *dp, size_t recl, size_t size, size_t n);
size_t msavedata (void *ptr, void *dp, size_t recl, size_t size, size_t n);

typedef size_t (*iodata_t) (iofunc_t func, void *ptr, void *dp, size_t n);
typedef void (*clrdata_t) (void *dp, size_t n);

/*	Strings mit dynamischer Speicherzuordnung
*/

#define	mstr_size	sizeof(mstr_t)
#define	mstr_recl	0

size_t mstr_iodata (iofunc_t func, void *ptr, mstr_t *dp, size_t n);
void mstr_clrdata (mstr_t *str, size_t n);

#endif	/* EFEU/Data.h */
