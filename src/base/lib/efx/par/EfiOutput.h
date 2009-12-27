/*	Ausgabedefintionen für Efeu-Datenobjekte

	$Copyright (C) 2006 Erich Frühstück
	A-3423 St.Andrä/Wördern, Wildenhaggasse 38
*/

#ifndef	EFEU_EfiOutput_h
#define	EFEU_EfiOutput_h	1

#include <EFEU/EfiPar.h>

extern EfiPar EfiPar_output;

typedef struct {
	EPC_VAR;
	void *(*out_open) (IO *out, const EfiType *type,
		void *par, const char *opt, const char *arg);
	size_t (*out_write) (EfiType *type, void *data, void *par);
	void *par;
} EfiOutput;

#define	EPC_OUTPUT(type, name, label, info, open, write, par)	\
	{ EPC_DATA(&EfiPar_output, type, name, label, info), \
		open, write, par }

extern EfiOutput EfiOutput_binary;

void SetupEfiOutput (void);

typedef struct {
	unsigned flag;	/* Flag */
	int key;	/* Flag-Kennung */
	char *desc;	/* Beschreibungstext */
} EfiPrintFlag;

typedef struct {
	int (*func) (IO *io, void *data, unsigned flags);
	EfiPrintFlag *ftab;	/* Tabelle mit Ausgabeflags */
	size_t fdim;		/* Zahl der Ausgabeflags */
} EfiPrintDef;

unsigned EfiPrint_flag (EfiPrintDef *pdef, const char *def);
void EfiPrint_list (EfiPrintDef *pdef, IO *io, const char *fmt);
void *EfiPrint_open (IO *out, const EfiType *type,
	void *par, const char *opt, const char *arg);
size_t EfiPrint_write (EfiType *type, void *data, void *par);
void EfiPrint_info (IO *io, const void *data);

#define	EPC_PRINT(type, name, label, par)	\
	{ EPC_DATA(&EfiPar_output, type, name, label, EfiPrint_info), \
		EfiPrint_open, EfiPrint_write, par }

#endif	/* EFEU/EfiOutput.h */
