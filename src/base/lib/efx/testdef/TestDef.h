/*
:*:	test definition
:de:	Testdefinitionen

$Header <EFEU/$1>

$Copyright (C) 2005 Erich Frühstück
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

#ifndef	EFEU_TestDef_h
#define	EFEU_TestDef_h	1

#include <EFEU/object.h>

/*	Testparameter
*/

typedef struct {
	EfiType *type;		/* Datentype */
	char *syntax;
	void *(*create) (const char *opt, const char *arg, void *ptr);
	void (*clean) (void *ptr);
	void *data;
	char *desc;
} TestParDef;

extern void *CreateTestPar (TestParDef *def, const char *opt, const char *arg);
extern void CleanTestPar (TestParDef *def, void *par);

extern TestParDef TestPar_Num;
extern TestParDef TestPar_64;
extern TestParDef TestPar_Dat;
extern TestParDef TestPar_Dat1900;

void *MakeValPar (const char *opt, const char *arg, void *data);
int TestVal (void *par, unsigned data);
int TestVal64 (void *par, uint64_t data);

extern TestParDef TestPar_a37l;
extern TestParDef TestPar_enum;

void *MakeCodePar (const char *opt, const char *arg, void *data);
void *MakeEnumPar (const char *opt, const char *arg, void *data);
void *TestBase37Par (const char *def);
int TestCode (void *par, unsigned data);

extern TestParDef TestPar_double;
int Test_double (void *par, double data);

/*	Testdefinition
*/

typedef int (*TestDefFunc) (void *data, const EfiObj *obj);

typedef struct TestDefEntryStruct TestDefEntry;
typedef struct TestDefStruct TestDef;

struct TestDefEntryStruct {
	TestDefEntry *next;
	EfiType *type;
	size_t dim;
	size_t offset;
	int (*tst)(TestDefEntry *entry, void *data);
	void (*clean)(void *par);
	void *par;
};

struct TestDefStruct {
	REFVAR;
	EfiType *type;
	TestDefEntry *list;
	TestParDef *par;
	TestDefFunc test;
	void *data;
};

TestDef *test_create (TestParDef *def, TestDefFunc test,
	const char *opt, const char *arg);

TestDef *TestDef_create (EfiType *type, const char *def);
int TestDef_test (TestDef *def, const void *data);
int TestDef_obj (TestDef *def, const EfiObj *obj);

int EfiVec_test (EfiVec *buf, TestDef *def);
int EfiVec_select (EfiVec *buf, TestDef *def);

void SetupTestDef(void);

#endif	/* EFEU/TestDef.h */
