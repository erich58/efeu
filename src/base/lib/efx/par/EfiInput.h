/*	Eingabedefintionen f�r Efeu-Datenobjekte

	$Copyright (C) 2006 Erich Fr�hst�ck
	A-3423 St.Andr�/W�rdern, Wildenhaggasse 38
*/

#ifndef	EFEU_EfiInput_h
#define	EFEU_EfiInput_h	1

#include <EFEU/EfiPar.h>

extern EfiPar EfiPar_input;

typedef struct {
	EPC_VAR;
	void *(*in_open) (IO *in, const EfiType *type,
		void *par, const char *opt, const char *arg);
	int (*in_read) (EfiType *type, void *data, void *par);
	void *par;
} EfiInput;

#define	EPC_INPUT(type, name, label, info, open, read, par)	\
	{ EPC_DATA(&EfiPar_input, type, name, label, info), \
		open, read, par }

extern EfiInput EfiInput_binary;
extern EfiInput EfiInput_plain;
extern EfiInput EfiInput_label;

void SetupEfiInput (void);


#endif	/* EFEU/EfiInput.h */
