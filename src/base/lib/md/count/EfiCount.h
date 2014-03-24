/*
Zähldefintionen für Efeu-Datenobjekte

$Copyright (C) 2007 Erich Frühstück
A-3423 St.Andrä/Wördern, Wildenhaggasse 38
*/

#ifndef	EFEU_EfiCount_h
#define	EFEU_EfiCount_h	1

#include <EFEU/EfiPar.h>
#include <EFEU/mdcount.h>

extern EfiPar EfiPar_EfiCount;

typedef struct EfiCount EfiCount;

struct EfiCount {
	EPC_VAR;
	char *syntax;
	void (*create) (MdCountPar *par, EfiCount *def, EfiObj *obj,
		const char *opt, const char *arg);
	char *par;
};

void EfiCount_info (IO *io, const void *data);

#define	EFI_COUNT(type, name, syntax, create, par, label) \
	{ EPC_DATA(&EfiPar_EfiCount, type, name, label, EfiCount_info), \
		syntax, create, par }

void AddEfiCount (EfiCount *def);
void SetupEfiCount (void);
void EfiCountInfo (InfoNode *info);

extern EfiCount EfiCount_record;

#endif	/* EFEU/EfiCount.h */
