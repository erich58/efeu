/*	EDB-Datenbankfilter

	$Copyright (C) 2006 Erich Frühstück
	A-3423 St.Andrä/Wördern, Wildenhaggasse 38
*/

#ifndef	EFEU_EDBFilter_h
#define	EFEU_EDBFilter_h	1

#include <EFEU/EDB.h>
#include <EFEU/EfiPar.h>
#include <EFEU/EDBFilter.h>

extern EfiPar EfiPar_EDBFilter;

void EDBFilter_info (IO *io, const void *data);

typedef struct EDBFilterStruct EDBFilter;

struct EDBFilterStruct {
	EPC_VAR;
	char *syntax;
	EDB *(*create) (EDBFilter *def, EDB *base,
		const char *opt, const char *arg);
	char *par;
};

#define	EDB_FILTER(type, name, syntax, create, par, label)	\
	{ EPC_DATA(&EfiPar_EDBFilter, type, name, label, EDBFilter_info), \
		syntax, create, par }

void AddEDBFilter (EDBFilter *def);
void SetupEDBFilter (void);
void EDBFilterInfo (InfoNode *info);

extern EDBFilter EDBFilter_sort;
extern EDBFilter EDBFilter_uniq;
extern EDBFilter EDBFilter_sum;
extern EDBFilter EDBFilter_test;
extern EDBFilter EDBFilter_expr;
extern EDBFilter EDBFilter_trans;
extern EDBFilter EDBFilter_join;
extern EDBFilter EDBFilter_var;
extern EDBFilter EDBFilter_conv;
extern EDBFilter EDBFilter_dup;
extern EDBFilter EDBFilter_clip;
extern EDBFilter EDBFilter_tee;
extern EDBFilter EDBFilter_script;
extern EDBFilter EDBFilter_nest;
extern EDBFilter EDBFilter_unnest;
extern EDBFilter EDBFilter_sample;


#endif	/* EFEU/EDBFilter.h */
