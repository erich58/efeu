/*
EFEU-Parameter

$Header <EFEU/$1>

$Copyright (C) 2006 Erich Frühstück
This file is part of EFEU.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; see the file COPYING.Library.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef	EFEU_EfiPar_h
#define	EFEU_EfiPar_h	1

#include <EFEU/object.h>

typedef struct {
	const char *name;	/* Parametername */
	const char *label;	/* Parameterbezeichnung */
} EfiPar;

#define	EPC_VAR		\
	const EfiPar *epc_par; \
	const EfiType *epc_type; \
	const char *epc_name; \
	const char *epc_label; \
	void (*epc_info) (IO *io, const void *data)

#define	EPC_DATA(par,type,name,label,info)	\
	par, type, name, label, info

typedef struct {
	EPC_VAR;
} EfiParClass;

void *GetEfiPar (EfiType *type, EfiPar *par, const char *name);
void *SearchEfiPar (EfiType *type, EfiPar *par, const char *name);
void AddEfiPar (EfiType *type, void *data);
void AddEfiParTab (EfiType *type, void *data, size_t dim, size_t size);
void PrintEfiPar (IO *io, void *data);
void EfiParWalk (EfiType *type, EfiPar *par,
	void (*visit) (EfiParClass *entry, void *ptr), void *ptr);
void ListEfiPar (IO *io, EfiType *type, EfiPar *par,
	const char *pfx, int verbosity);

/*
Steuerparameter
*/

extern EfiPar EfiPar_control;
void EfiControl_info (IO *io, const void *data);

typedef struct {
	EPC_VAR;
	char *data;
} EfiControl;

#define	EPC_CONTROL(type, name, label, par)	\
	{ EPC_DATA(&EfiPar_control, type, name, label, EfiControl_info), par }

char *GetEfiControl (EfiType *type, char *name);
char *SearchEfiControl (EfiType *type, char *name);

#endif	/* EFEU/EfiPar.h */
