/*
Klassifikationen für Efeu-Datenobjekte

$Copyright (C) 2007 Erich Frühstück
A-3423 St.Andrä/Wördern, Wildenhaggasse 38
*/

#ifndef	EFEU_EfiClass_h
#define	EFEU_EfiClass_h	1

#include <EFEU/EfiPar.h>
#include <EFEU/object.h>
#include <EFEU/Info.h>

extern EfiPar EfiPar_class;

typedef struct EfiClass EfiClass;

typedef struct {
	EfiType *type;
	int (*update) (const EfiObj *base, void *par);
	void *par;
} EfiClassArg;

struct EfiClass {
	EPC_VAR;
	char *syntax;
	void (*create) (EfiClassArg *def, const EfiType *type,
		const char *opt, const char *arg, void *par);
	void *par;
};

void EfiClass_info (IO *io, const void *data);

#define	EFI_CLASS(type, name, syntax, create, par, label) \
	{ EPC_DATA(&EfiPar_class, type, name, label, EfiClass_info), \
		syntax, create, par }

void SetupEfiClass (void);
void EfiClassEnum (void);
void EfiClassInfo (InfoNode *info);

extern EfiClass EfiClass_flag;
extern EfiClass EfiClass_generic;
extern EfiClass EfiClass_test;
extern EfiClass EfiClass_switch;

void AddEfiClass (EfiVarTab *tab, const char *name, const char *desc,
	EfiType *type, int (*update) (const EfiObj *, void *), void *par);

typedef struct {
	const char *name;
	const char *desc;
	size_t dim;
	Label *label;
	int (*update) (const EfiObj *base, void *par);
	void *par;
} EfiClassDef;

void AddEfiClassDef (EfiVarTab *tab, EfiClassDef *def, size_t dim);
int MakeEfiClass (EfiType *type, const char *name, const char *def);
EfiObj *EfiClassExpr (EfiObj *base, const char *def);

void DayTime (EfiClassArg *def, const EfiType *type,
		const char *opt, const char *arg, void *par);
void CalClass (EfiClassArg *def, const EfiType *type,
		const char *opt, const char *arg, void *par);

#endif	/* EFEU/EfiClass.h */
